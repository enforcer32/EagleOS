#include <Handshake/HS/Handshake.h>
#include <Handshake/BIOS/Video/Print.h>
#include <Handshake/Video/VGA.h>
#include <Handshake/Stdio/Print.h>
#include <Handshake/Stdio/Panic.h>
#include <Handshake/System/MemoryMap.h>
#include <Handshake/BootInfo.h>
#include <Handshake/Memory/Malloc.h>
#include <Handshake/CPU/PageManager.h>
#include <ESTD/CString.h>
#include <ELIB/LibELF/ELF.h>
#include <ESTD/Algorithm.h>

extern "C" uint32_t __bootloader_physical_start;
extern "C" uint32_t __bootloader_physical_end;

typedef void (*KernelEntry)(const Handshake::BootInfo* bootInfo, uint32_t stack);

namespace Handshake
{
	CPU::PageManager s_PageManager;

	void DumpSystemMemory(const System::MemoryMap* memoryMap)
	{
		Printf("\n----------System Memory Map----------\n");
		for (uint32_t i = 0; i < memoryMap->RegionCount; i++)
		{
			const auto& regionInfo = memoryMap->Regions[i];
			Printf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Printf("Length= 0x%x, ", regionInfo.Length);
			Printf("Type= 0x%x, ", regionInfo.Type);
			Printf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		Printf("----------System Memory Map----------\n\n");
	}

	void DumpBootInfo(const BootInfo* bootInfo)
	{
		Printf("\n----------BootInfo----------\n");
		Printf("Signature: 0x%x\n", bootInfo->Signature);
		Printf("BootloaderPhysicalStartAddress: 0x%x\n", bootInfo->BootloaderPhysicalStartAddress);
		Printf("BootloaderPhysicalEndAddress: 0x%x\n", bootInfo->BootloaderPhysicalEndAddress);
		Printf("BootloaderSize: 0x%x\n", bootInfo->BootloaderSize);
		
		Printf("KernelPhysicalStartAddress: 0x%x\n", bootInfo->KernelPhysicalStartAddress);
		Printf("KernelPhysicalEndAddress: 0x%x\n", bootInfo->KernelPhysicalEndAddress);
		Printf("KernelVirtualStartAddress: 0x%x\n", bootInfo->KernelVirtualStartAddress);
		Printf("KernelVirtualEndAddress: 0x%x\n", bootInfo->KernelVirtualEndAddress);
		Printf("KernelSize: 0x%x\n", bootInfo->KernelSize);
		
		Printf("LowMemoryPhysicalStartAddress: 0x%x\n", bootInfo->LowMemoryPhysicalStartAddress);
		Printf("LowMemoryPhysicalEndAddress: 0x%x\n", bootInfo->LowMemoryPhysicalEndAddress);
		Printf("LowMemoryVirtualStartAddress: 0x%x\n", bootInfo->LowMemoryVirtualStartAddress);
		Printf("LowMemoryVirtualEndAddress: 0x%x\n", bootInfo->LowMemoryVirtualEndAddress);
		Printf("LowMemorySize: 0x%x\n", bootInfo->LowMemorySize);
		
		Printf("HighMemoryPhysicalStartAddress: 0x%x\n", bootInfo->HighMemoryPhysicalStartAddress);
		Printf("HighMemoryPhysicalEndAddress: 0x%x\n", bootInfo->HighMemoryPhysicalEndAddress);
		
		Printf("StackBaseVirtualAddress: 0x%x\n", bootInfo->StackBaseVirtualAddress);
		Printf("StackSize: 0x%x\n", bootInfo->StackSize);
		Printf("----------BootInfo----------\n\n");
	}

	bool SetupMemoryInfo(BootInfo* bootInfo, const ELF::ELF* kernelELF)
	{
		bootInfo->MemoryMap = System::DetectMemoryMap();
		if (!bootInfo->MemoryMap)
		{
			Printf("SetupMemoryInfo->Failed to Detect Memory Map!\n");
			return false;
		}

		// Bootloader Physical Address
		bootInfo->BootloaderPhysicalStartAddress = (uint32_t)&__bootloader_physical_start;
		bootInfo->BootloaderPhysicalEndAddress = (uint32_t)&__bootloader_physical_end;
		bootInfo->BootloaderSize = (bootInfo->BootloaderPhysicalEndAddress - bootInfo->BootloaderPhysicalStartAddress);

		// Kernel Virtual Address
		bootInfo->KernelVirtualStartAddress = kernelELF->GetVirtualAddressLow();
		bootInfo->KernelVirtualEndAddress = kernelELF->GetVirtualAddressHigh();
		bootInfo->KernelSize = (bootInfo->KernelVirtualEndAddress - bootInfo->KernelVirtualStartAddress);

		// Kernel Physical Address
		bootInfo->KernelPhysicalStartAddress = HANDSHAKE_MEMORY_KERNEL_LOAD_ADDRESS;
		bootInfo->KernelPhysicalEndAddress = (bootInfo->KernelPhysicalStartAddress + bootInfo->KernelSize);

		// Low Memory Physical Address
		bootInfo->LowMemoryPhysicalStartAddress = ESTD::AlignUp(bootInfo->KernelPhysicalEndAddress, PAGE_SIZE);
		bootInfo->LowMemoryPhysicalEndAddress = (896 * 1048576);
		bootInfo->LowMemorySize = (bootInfo->LowMemoryPhysicalEndAddress - bootInfo->LowMemoryPhysicalStartAddress);
		
		// Low Memory Virtual Address Address
		bootInfo->LowMemoryVirtualStartAddress = ESTD::AlignUp(bootInfo->KernelVirtualEndAddress, PAGE_SIZE);
		bootInfo->LowMemoryVirtualEndAddress = (bootInfo->LowMemoryVirtualStartAddress * bootInfo->LowMemorySize);

		// High Memory
		bootInfo->HighMemoryPhysicalStartAddress = bootInfo->LowMemoryPhysicalEndAddress;
		bootInfo->HighMemoryPhysicalEndAddress = bootInfo->MemoryMap->GetUpperMemory();

		return true;
	}

	bool SetupPaging(const BootInfo* bootInfo)
	{
		if (s_PageManager.Init() != 0)
		{
			Printf("Failed to Initialize PageManager!\n");
			return false;
		}

		/*
			Identity Map Initially
			Bootloader
			Kernel
			LowMemory
			(0x0 -> LowMemoryPhysicalEndAddress)
		*/
		uint32_t LowMemoryPhysicalEndAddressPage = (bootInfo->LowMemoryPhysicalEndAddress - 1) / PAGE_SIZE;
		if (s_PageManager.MapRange(0x0, 0x0, true, LowMemoryPhysicalEndAddressPage) != 0)
		{
			Printf("SetupPaging->PageManager Failed to Identity Map!\n");
			return false;
		}

		/*
			Map Kernel Address Space
			Kernel
			LowMemory
		*/
		uint32_t kernelPhysicalStartAddressPage = (bootInfo->KernelPhysicalStartAddress / 4096);
		uint32_t pageCount = LowMemoryPhysicalEndAddressPage - kernelPhysicalStartAddressPage;
		if (s_PageManager.MapRange(bootInfo->KernelVirtualStartAddress, bootInfo->KernelPhysicalStartAddress, true, pageCount) != 0)
		{
			Printf("SetupPaging->PageManager Failed to Map Kernel Space!\n");
			return false;
		}

		return true;
	}
	
	bool LoadKernel(const BootInfo* bootInfo, const ELF::ELF* kernelELF)
	{
		return kernelELF->LoadProgramHeadersToVirtualMemory();
	}

	bool Init()
	{
		Video::VGA::Init();
		Video::VGA::ClearScreen();
		Printf("Initializing Handshake Bootloader...!\n");

		// Parse Kernel ELF
		ELF::ELF kernelELF;
		if (!kernelELF.Parse(ATA::ATADrive::Slave))
		{
			Printf("Failed to Parse Kernel.ELF\n");
			return false;
		}

		// Prepare BootInfo
		BootInfo* bootInfo = (BootInfo*)Malloc(sizeof(BootInfo));
		if (!bootInfo)
		{
			Printf("Failed to Initialize BootInfo!\n");
			return false;
		}
		ESTD::Memset(bootInfo, 0, sizeof(BootInfo));

		bootInfo->Signature = HANDSHAKE_BOOT_SIGNATURE;

		// Detect System Memory Map
		if (!SetupMemoryInfo(bootInfo, &kernelELF))
		{
			Printf("Failed to SetupMemoryInfo!\n");
			return false;
		}

		// Setup Paging
		if (!SetupPaging(bootInfo))
		{
			Printf("Failed to SetupPaging!\n");
			return false;
		}

		s_PageManager.LoadPageDirectory();
		s_PageManager.EnablePaging();

		// Setup Kernel Stack (Grows Downwards)
		bootInfo->StackSize = 0x100000; // 1MB
		bootInfo->StackBaseVirtualAddress = bootInfo->LowMemoryVirtualEndAddress;
		bootInfo->LowMemoryPhysicalEndAddress = (bootInfo->LowMemoryPhysicalEndAddress - bootInfo->StackSize);
		bootInfo->LowMemoryVirtualEndAddress = (bootInfo->LowMemoryVirtualEndAddress - bootInfo->StackSize);

		//DumpSystemMemory(bootInfo->MemoryMap);
		DumpBootInfo(bootInfo);

		if (!LoadKernel(bootInfo, &kernelELF))
		{
			Printf("Failed to Load Kernel into Memory!\n");
			return false;
		}

		KernelEntry _kernelstart = (KernelEntry)bootInfo->KernelVirtualStartAddress;
		_kernelstart(bootInfo, bootInfo->StackBaseVirtualAddress);
		
		return false;
	}
}

extern "C" void HSMain()
{
	if (!Handshake::Init())
	{
		Handshake::BIOS::Print("Failed To Initialize Handshake Bootloader!\n");
		return;
	}

	for(;;);
}
