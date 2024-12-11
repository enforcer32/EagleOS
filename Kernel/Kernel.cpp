#include <Handshake/BootInfo.h>
#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/Kern/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Memory/VirtualMemoryAllocator.h>
#include <ESTD/CString.h>

namespace Kernel
{
	Memory::PhysicalMemoryManager _KernelPMM;
	Memory::VirtualMemoryManager _KernelVMM;
	Memory::VirtualMemoryAllocator _KernelVMA;

	void DumpSystemMemoryMap(const Handshake::BootInfo* bootInfo)
	{
		KPrintf("\n----------System Memory Map----------\n");
		for (uint32_t i = 0; i < bootInfo->MemoryMap->RegionCount; i++)
		{
			const auto& regionInfo = bootInfo->MemoryMap->Regions[i];
			Kernel::KPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Kernel::KPrintf("Length= 0x%x, ", regionInfo.Length);
			Kernel::KPrintf("Type= 0x%x, ", regionInfo.Type);
			Kernel::KPrintf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		KPrintf("----------System Memory Map----------\n\n");
	}

	void DumpBootInfo(const Handshake::BootInfo* bootInfo)
	{
		KPrintf("\n----------BootInfo----------\n");
		KPrintf("Signature: 0x%x\n", bootInfo->Signature);
		KPrintf("BootloaderPhysicalStartAddress: 0x%x\n", bootInfo->BootloaderPhysicalStartAddress);
		KPrintf("BootloaderPhysicalEndAddress: 0x%x\n", bootInfo->BootloaderPhysicalEndAddress);
		KPrintf("BootloaderSize: 0x%x\n", bootInfo->BootloaderSize);
		
		KPrintf("KernelPhysicalStartAddress: 0x%x\n", bootInfo->KernelPhysicalStartAddress);
		KPrintf("KernelPhysicalEndAddress: 0x%x\n", bootInfo->KernelPhysicalEndAddress);
		KPrintf("KernelVirtualStartAddress: 0x%x\n", bootInfo->KernelVirtualStartAddress);
		KPrintf("KernelVirtualEndAddress: 0x%x\n", bootInfo->KernelVirtualEndAddress);
		KPrintf("KernelSize: 0x%x\n", bootInfo->KernelSize);
		KPrintf("----------BootInfo----------\n\n");
	}

	bool InitPhysicalMemoryManager(const Handshake::BootInfo* bootInfo)
	{
		// Init Physical Memory Manager
		g_KernelPMM = &_KernelPMM;
		if (!g_KernelPMM->Init(bootInfo))
		{
			KPrintf("Failed to Initialize g_KernelPMM\n");
			return false;
		}
		
		// Reserve Bootloader Pages
		uint32_t bootloaderPageCount = (bootInfo->BootloaderSize / 4096) + 1;
		g_KernelPMM->ReservePages(bootInfo->BootloaderPhysicalStartAddress, bootloaderPageCount);

		// Reserve Kernel Pages
		uint32_t kernelPageCount = bootInfo->KernelSize / 4096;
		g_KernelPMM->ReservePages(bootInfo->KernelPhysicalStartAddress, kernelPageCount);

		// Reserve first 16MB for ISA16 DMA
		uint32_t dma16PageCount = 0x1000000 / 4096;
		g_KernelPMM->ReservePages(0x0, dma16PageCount);

		// Reserve Address 0x0
		g_KernelPMM->ReservePage(0x0);
		return true;
	}

	bool InitVirtualMemoryManager(const Handshake::BootInfo* bootInfo)
	{
		g_KernelVMM = &_KernelVMM;
		if (!g_KernelVMM->Init(bootInfo))
		{
			KPrintf("Failed to Initialize g_KernelVMM\n");
			return false;
		}

		// Identity Map first 4MB (Bootloader/DMA)
		uint32_t bootloaderPageCount = 0x400000 / PAGE_SIZE;
		if (!g_KernelVMM->MapRange(0x0, 0x0, true, bootloaderPageCount))
		{
			KPrintf("InitVirtualMemoryManager->PageManager Failed to Identity Map first 4mb!\n");
			return false;
		}

		// ReSetup Higher Half Kernel
		uint32_t kernelPhysicalStartAddressPageNumber = (bootInfo->KernelPhysicalStartAddress / PAGE_SIZE);
		uint32_t kernelPhysicalEndAddressPageNumber = (bootInfo->KernelPhysicalEndAddress / PAGE_SIZE);
		uint32_t kernelPageCount = (kernelPhysicalEndAddressPageNumber - kernelPhysicalStartAddressPageNumber); // 44 (0xC002C000)
		if (!g_KernelVMM->MapRange(bootInfo->KernelVirtualStartAddress, bootInfo->KernelPhysicalStartAddress, true, kernelPageCount))
		{
			KPrintf("InitVirtualMemoryManager->PageManager Failed to Map Kernel to Higher Half!\n");
			return false;
		}

		// Map Rest of Higher Half Table
		uint32_t pageTablePageCount = 0x400000 / PAGE_SIZE;
		if(kernelPageCount < pageTablePageCount)
		{
			if (!g_KernelVMM->MapRange(bootInfo->KernelVirtualStartAddress + (kernelPageCount * PAGE_SIZE), bootInfo->KernelPhysicalStartAddress + (kernelPageCount * PAGE_SIZE), true, (pageTablePageCount - kernelPageCount)))
			{
				KPrintf("InitVirtualMemoryManager->PageManager Failed to Map Kernel to Rest of the Higher Half!\n");
				return false;
			}
		}

		g_KernelVMM->ReloadPageDirectory();
		return true;
	}

	bool InitVirtualMemoryAllocator(const Handshake::BootInfo* bootInfo)
	{	
		uint32_t kernelVMAStartAddress = bootInfo->KernelVirtualStartAddress + (4096 * 1024 * 4); // 0xC1000000
		uint32_t kernelVMASize = 0xFFFFFFFF - kernelVMAStartAddress;

		g_KernelVMA = &_KernelVMA;
		if (!g_KernelVMA->Init(kernelVMAStartAddress, kernelVMASize))
		{
			KPrintf("Failed to Initialize g_KernelVMA\n");
			return false;
		}


		// Reserve Address 0x0
		g_KernelVMA->ReservePage(0x0);
		
		VirtualAddress addr = g_KernelVMA->AllocatePage();
		KPrintf("Addr: 0x%x\n", addr);

		g_KernelVMA->FreePage(addr);

		return true;
	}
	
	bool InitMemoryManager(const Handshake::BootInfo* bootInfo)
	{
		if(!Memory::MemoryUtil::Init(bootInfo))
		{
			KPrintf("Failed to Initialize MemoryUtil\n");
			return false;
		}

		if (!InitPhysicalMemoryManager(bootInfo))
		{
			KPrintf("Failed to Initialize PhysicalMemoryManager\n");
			return false;
		}

		if (!InitVirtualMemoryManager(bootInfo))
		{
			KPrintf("Failed to Initialize VirtualMemoryManager\n");
			return false;
		}

		if (!InitVirtualMemoryAllocator(bootInfo))
		{
			KPrintf("Failed to Initialize VirtualMemoryAllocator\n");
			return false;
		}

		return true;
	}

	void InitKernel(const Handshake::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		DumpBootInfo(bootInfo);
		//DumpSystemMemoryMap(bootInfo);

		if (x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		if (!InitMemoryManager(bootInfo))
			KPanic("Failed to Initialize Memory Manager\n");

		//KPrintf("\n----------Kernel Memory Map----------\n");
		//KPrintf("KernelPhysicalStartAddress: 0x%x\n", bootInfo->KernelPhysicalStartAddress);
		//KPrintf("KernelPhysicalEndAddress: 0x%x\n", bootInfo->KernelPhysicalEndAddress);
		//KPrintf("KernelVirtualStartAddress: 0x%x\n", bootInfo->KernelVirtualStartAddress);
		//KPrintf("KernelVirtualEndAddress: 0x%x\n", bootInfo->KernelVirtualEndAddress);
		//KPrintf("----------Kernel Memory Map----------\n");
		//DumpSystemMeoryMap(bootInfo->MemoryInfo);

		//PhysicalAddress addr1 = g_KernelPMM->AllocatePage();
		//int* data = (int*)0x810000;
		//if(g_KernelVMM->Map(0x810000, addr1) != 0)
		//{
		//	KPrintf("MAPPING FAILED\n");
		//	for(;;);
		//}
		//data[0] = 1; // PF

		/*
		PhysicalAddress addr1 = g_KernelPMM->AllocatePage();
		KPrintf("Addr1: 0x%x\n", addr1);
		PhysicalAddress addr2 = g_KernelPMM->AllocatePage();
		KPrintf("Addr2: 0x%x\n", addr2);

		PhysicalAddress block = g_KernelPMM->AllocatePage();
		char* data = (char*)block;
		data[0] = 'P';
		data[1] = 0;
		KPrintf("Block Address: 0x%x, Data Address: 0x%x, Data: %c\n", block, data, *data);
		PhysicalAddress block2 = 0x41E000;
		//g_KernelVMM->Map(block, block2, ((uint8_t)x86::PageTableFlags::Present | (uint8_t)x86::PageTableFlags::ReadWrite));
		char* data2 = (char*)block2;
		data2[0] = 'L';
		data2[1] = 0;
		KPrintf("Block2 Address: 0x%x, Data2 Address: 0x%x, Data: %c\n", block2, data2, *data);
		*/

		KPrintf("\nEagle Operating System\n");
	}
}

extern "C" void KMain(const Handshake::BootInfo* bootInfo)
{
	if(bootInfo->Signature != HANDSHAKE_BOOT_SIGNATURE)
		return;

	Kernel::InitKernel(bootInfo);
	for(;;);
}
