#include <Axe/BootInfo.h>
#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/NXN/KPrintf.h>
#include <Kernel/NXN/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/NXN/CString.h>

extern uint64_t __kernel_start;
extern uint64_t __kernel_end;

namespace Kernel
{
	Memory::PhysicalMemoryManager _KernelPMM;

	void DumpSystemMeoryMap(const Axe::SystemMemoryInfo* memoryInfo)
	{
		KPrintf("\n----------System Memory Map----------\n");
		for (uint32_t i = 0; i < memoryInfo->RegionCount; i++)
		{
			const auto& regionInfo = memoryInfo->Regions[i];
			Kernel::KPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Kernel::KPrintf("Length= 0x%x, ", regionInfo.Length);
			Kernel::KPrintf("Type= 0x%x, ", regionInfo.Type);
			Kernel::KPrintf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		KPrintf("----------System Memory Map----------\n\n");
	}

	bool InitMemoryManager(const Axe::BootInfo* bootInfo)
	{
		g_KernelPMM = &_KernelPMM;
		if (g_KernelPMM->Init(bootInfo->MemoryInfo, 4096) != 0)
		{
			KPrintf("Failed to Initialize g_KernelPMM\n");
			return false;
		}

		// Reserve Kernel Pages
		uint64_t kernelSize = (uint64_t)&__kernel_end - (uint64_t)&__kernel_start;
		uint64_t kernelPageCount = kernelSize / 4096 + 1;
		g_KernelPMM->ReservePages((PhysicalAddress)&__kernel_start, kernelPageCount);

		// Reserve Address 0x0
		g_KernelPMM->AllocatePage();

		DumpSystemMeoryMap(bootInfo->MemoryInfo);
		return true;
	}

	void InitKernel(const Axe::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		if (x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		if (!InitMemoryManager(bootInfo))
			KPanic("Failed to Initialize Memory Manager\n");

		PhysicalAddress addr1 = g_KernelPMM->AllocatePage();
		KPrintf("Addr1: 0x%x\n", addr1);
		PhysicalAddress addr2 = g_KernelPMM->AllocatePage();
		KPrintf("Addr2: 0x%x\n", addr2);

		KPrintf("\nEagle Operating System\n");
	}
}

extern "C" void KMain(const Axe::BootInfo* bootInfo)
{
	Kernel::InitKernel(bootInfo);
	for(;;);
}
