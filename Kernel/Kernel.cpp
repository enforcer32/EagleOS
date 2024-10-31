#include <Axe/BootInfo.h>
#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/NXN/KPrintf.h>
#include <Kernel/NXN/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>

namespace Kernel
{
	void InitKernel(const Axe::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		if(x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		KPrintf("----------System Memory Map----------\n");
		for (uint32_t i = 0; i < bootInfo->MemoryInfo->RegionCount; i++)
		{
			const auto& regionInfo = bootInfo->MemoryInfo->Regions[i];
			Kernel::KPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Kernel::KPrintf("Length= 0x%x, ", regionInfo.Length);
			Kernel::KPrintf("Type= 0x%x, ", regionInfo.Type);
			Kernel::KPrintf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		KPrintf("----------System Memory Map----------\n");

		Memory::PhysicalMemoryManager pmm;
		pmm.Init(bootInfo->MemoryInfo, 4096);

		void* data = pmm.AllocatePages(1);
		KPrintf("Data: 0x%x\n", data);

		data = pmm.AllocatePages(1);
		KPrintf("Data: 0x%x\n", data);

		data = pmm.AllocatePages(2);
		KPrintf("Data: 0x%x\n", data);

		data = pmm.AllocatePages(1);
		KPrintf("Data: 0x%x\n", data);

		data = pmm.AllocatePages(1);
		KPrintf("Data: 0x%x\n", data);

		pmm.FreePages(data, 1);

		data = pmm.AllocatePages(1);
		KPrintf("Data: 0x%x\n", data);

		KPrintf("\nEagle Operating System\n");
	}
}

extern "C" void KMain(const Axe::BootInfo* bootInfo)
{
	Kernel::InitKernel(bootInfo);
	for(;;);
}
