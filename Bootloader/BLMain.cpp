#include <Bootloader/BootInfo.h>
#include <Bootloader/ATA/ATA.h>
#include <Bootloader/ELF/ELF.h>

typedef void (*KernelEntry)(const BootInfo* bootInfo);

namespace Bootloader
{
	void Init()
	{
		ELF::ELF elf;
		elf.Parse(ATA::ATADrive::Slave);
		elf.LoadProgramHeaders();

		BootInfo* info = (BootInfo*)(0x00020C70);
		info->Signature = 0xDEADD432;
		info->KernelAddress = (uint32_t*)elf.GetEntryPointAddress();
		
		KernelEntry _kernelstart = (KernelEntry)elf.GetEntryPointAddress();
		_kernelstart(info);
	}
}

extern "C" void BLMain()
{
	Bootloader::Init();
}
