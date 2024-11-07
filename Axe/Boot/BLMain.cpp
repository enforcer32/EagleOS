#include <Axe/BootInfo.h>
#include <Axe/ATA/ATA.h>
#include <Axe/ELF/ELF.h>
#include <Axe/x86/Video.h>
#include <Axe/System/SystemMemory.h>
#include <Axe/x86/Paging/PageManager.h>
#include <Axe/Memory/Malloc.h>

typedef void (*KernelEntry)(const Axe::BootInfo* bootInfo);

namespace Axe
{
	x86::PageManager s_PageManager;

	bool SetupKernelVirtualAddress(const ELF::ELF& kernelELF)
	{
		if (s_PageManager.Init() != 0)
			return false;

		// Identity Map first 1MB for Bootloader
		uint32_t offset = 0;
		for (uint32_t i = 0; i < 256; i++)
		{
			if (s_PageManager.Map(offset, offset, true) != 0)
				return -1;
			offset += 4096;
		}

		// Map Kernel Program Headers
		for (uint32_t i = 0; i < kernelELF.GetProgramHeaderCount(); i++)
		{
			const auto& phdr = kernelELF.GetProgramHeaders()[i];
			uint32_t phdrSize = phdr->MemorySize;
			uint32_t pageCount = (phdrSize / 4096) + 1;

			uint32_t offset = 0;
			for(uint32_t j = 0; j < pageCount; j++)
			{
				bool writable = phdr->Flags == ELF::ELFProgramHeaderFlags::ReadWrite;
				if (s_PageManager.Map(phdr->VirtualAddress + offset, phdr->PhysicalAddress + offset, writable) != 0)
					return -1;
				offset += 4096;
			}
		}

		s_PageManager.LoadKernelDirectory();
		s_PageManager.EnablePaging();
		return true;
	}

	void Init()
	{
		ELF::ELF kernelELF;
		kernelELF.Parse(ATA::ATADrive::Slave);
		kernelELF.LoadProgramHeadersToPhysicalMemory();

		if (!SetupKernelVirtualAddress(kernelELF))
			return;

		BootInfo* info = (BootInfo*)Malloc(sizeof(BootInfo));
		info->Signature = AXE_BOOT_SIGNATURE;
		info->KernelPhysicalStartAddress = 0;
		info->KernelPhysicalEndAddress = 0;
		info->KernelVirtualStartAddress = kernelELF.GetEntryPointAddress();
		info->KernelVirtualEndAddress = 0;
		info->MemoryInfo = DetectSystemMemory();
		
		KernelEntry _kernelstart = (KernelEntry)kernelELF.GetEntryPointAddress();
		_kernelstart(info);
	}
}

extern "C" void BLMain()
{
	//Axe::x86::BIOS::PrintChar('H');
	Axe::Init();
	for(;;);
}
