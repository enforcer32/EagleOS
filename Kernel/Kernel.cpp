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

	void InitKernel(const Axe::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		if(x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		Memory::PhysicalMemoryManager pmm;
		pmm.Init(bootInfo->MemoryInfo, 4096);

		// Reserve Kernel Pages
		uint64_t kernelSize = (uint64_t)&__kernel_end - (uint64_t)&__kernel_start;
		uint64_t kernelPageCount = kernelSize / 4096 + 1;
		pmm.ReservePages((PhysicalAddress)&__kernel_start, kernelPageCount);

		DumpSystemMeoryMap(bootInfo->MemoryInfo);

		KPrintf("\nEagle Operating System\n");
	}
}

extern "C" void KMain(const Axe::BootInfo* bootInfo)
{
	Kernel::InitKernel(bootInfo);
	for(;;);
}
