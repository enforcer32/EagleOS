#include <Bootloader/BootInfo.h>
#include <Bootloader/ATA/ATA.h>
#include <Bootloader/ELF/ELF.h>

#define VGA_TEXT_ENCODE(ch, color) ((color << 8) | ch)

typedef void (*KernelEntry)(const BootInfo* bootInfo);

namespace Bootloader
{
	void Init()
	{
		ELF::ELF elf;
		elf.Parse(ATA::ATADrive::Slave);

		/*
		short* VGA = (short*)0xB8000;

		BootInfo* info = (BootInfo*)(0x00020C70);
		info->Signature = 0xDEADD432;
		info->Letter = 'O';
		info->KernelAddress = (uint32_t*)0x00100000;

		// JUNK REMINDER // Cant Offset Elf Header, Code Section
		Bootloader::ATA::ATAReadLBA((uint32_t*)(0x00100000-0x34-0x001000), ATA::ATADrive::Slave, 100, 0);

		VGA[1] = VGA_TEXT_ENCODE('L', 2);
		VGA[2] = VGA_TEXT_ENCODE('-', 2);

		KernelEntry _kernelstart = (KernelEntry)(0x00100000);
		_kernelstart(info);
		*/
	}
}

extern "C" void BLMain()
{
	short* VGA = (short*)0xB8000;
	VGA[0] = VGA_TEXT_ENCODE('B', 2);
	Bootloader::Init();
}
