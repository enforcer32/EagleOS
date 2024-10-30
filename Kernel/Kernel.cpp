#include <Bootloader/BootInfo.h>
#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/NXN/KPrintf.h>
#include <Kernel/NXN/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>

namespace Kernel
{
	void InitKernel()
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		if(x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		//int x = 0;
		//int y = 10 / x;
		//(void)y;

		KPrintf("Eagle Operating System\n");
	}
}

extern "C" void KMain(const BootInfo* bootInfo)
{
	Kernel::InitKernel();

	for(uint32_t i = 0; i < bootInfo->SystemMemoryInfo->RegionCount; i++)
	{
		const auto& regionInfo = bootInfo->SystemMemoryInfo->Regions[i];
		Kernel::KPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
		Kernel::KPrintf("Length= 0x%x, ", regionInfo.Length);
		Kernel::KPrintf("Type= 0x%x, ", regionInfo.Type);
		Kernel::KPrintf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
	}

	for(;;);
}
