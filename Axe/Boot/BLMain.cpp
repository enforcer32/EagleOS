#include <Axe/BootInfo.h>
#include <Axe/ATA/ATA.h>
#include <Axe/ELF/ELF.h>
#include <Axe/x86/Video.h>
#include <Axe/System/SystemMemory.h>

typedef void (*KernelEntry)(const Axe::BootInfo* bootInfo);

namespace Axe
{
	void Init()
	{
		ELF::ELF elf;
		elf.Parse(ATA::ATADrive::Slave);
		elf.LoadProgramHeaders();

		BootInfo* info = (BootInfo*)(0x00020C70);
		info->Signature = 0xDEADD432;
		info->KernelAddress = (uint32_t*)elf.GetEntryPointAddress();
		info->MemoryInfo = DetectSystemMemory();
		
		KernelEntry _kernelstart = (KernelEntry)elf.GetEntryPointAddress();
		_kernelstart(info);
	}
}

extern "C" void BLMain()
{
	//Axe::x86::BIOS::PrintChar('H');
	Axe::Init();
}
