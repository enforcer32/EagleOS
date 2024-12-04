#include <Handshake/BootInfo.h>
#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/Kern/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>
//#include <Kernel/Memory/PhysicalMemoryManager.h>
//#include <Kernel/Memory/VirtualMemoryManager.h>
#include <ESTD/CString.h>

namespace Kernel
{
	//Memory::PhysicalMemoryManager _KernelPMM;
	//Memory::VirtualMemoryManager _KernelVMM;

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
		
		KPrintf("LowMemoryPhysicalStartAddress: 0x%x\n", bootInfo->LowMemoryPhysicalStartAddress);
		KPrintf("LowMemoryPhysicalEndAddress: 0x%x\n", bootInfo->LowMemoryPhysicalEndAddress);
		KPrintf("LowMemoryVirtualStartAddress: 0x%x\n", bootInfo->LowMemoryVirtualStartAddress);
		KPrintf("LowMemoryVirtualEndAddress: 0x%x\n", bootInfo->LowMemoryVirtualEndAddress);
		KPrintf("LowMemorySize: 0x%x\n", bootInfo->LowMemorySize);
		
		KPrintf("HighMemoryPhysicalStartAddress: 0x%x\n", bootInfo->HighMemoryPhysicalStartAddress);
		KPrintf("HighMemoryPhysicalEndAddress: 0x%x\n", bootInfo->HighMemoryPhysicalEndAddress);
		
		KPrintf("StackBaseVirtualAddress: 0x%x\n", bootInfo->StackBaseVirtualAddress);
		KPrintf("StackSize: 0x%x\n", bootInfo->StackSize);
		KPrintf("----------BootInfo----------\n\n");
	}

	/*
	bool InitMemoryManager(const Axe::BootInfo* bootInfo)
	{
		g_KernelPMM = &_KernelPMM;
		if (g_KernelPMM->Init(bootInfo, 4096) != 0)
		{
			KPrintf("Failed to Initialize g_KernelPMM\n");
			return false;
		}

		// Reserve Bootloader Pages (1st 1MB)
		uintptr_t bootloaderStartAddress = 0x0;
		uintptr_t bootloaderEndAddress = 0x100000;
		uint64_t bootloaderSize = bootloaderEndAddress - bootloaderStartAddress;
		uint64_t bootloaderPageCount = bootloaderSize / 4096;
		g_KernelPMM->ReservePages(bootloaderStartAddress, bootloaderPageCount);

		// Reserve Kernel Pages
		uint64_t kernelSize = bootInfo->KernelPhysicalEndAddress - bootInfo->KernelPhysicalStartAddress;
		uint64_t kernelPageCount = kernelSize / 4096;
		g_KernelPMM->ReservePages(bootInfo->KernelPhysicalStartAddress, kernelPageCount);

		// Reserve Address 0x0
		g_KernelPMM->AllocatePage();

		g_KernelVMM = &_KernelVMM;
		if (g_KernelVMM->Init(bootInfo) != 0)
		{
			KPrintf("Failed to Initialize g_KernelVMM\n");
			return false;
		}

		return true;
	}*/

	bool InitMemoryManager(const Handshake::BootInfo* bootInfo)
	{
		DumpBootInfo(bootInfo);

		//g_KernelPMM = &_KernelPMM;
		//if (!g_KernelPMM->Init(bootInfo))
		//{
		//	KPrintf("Failed to Initialize g_KernelPMM\n");
		//	return false;
		//}

		return true;
	}

	void InitKernel(const Handshake::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		//DumpBootInfo(bootInfo);
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
