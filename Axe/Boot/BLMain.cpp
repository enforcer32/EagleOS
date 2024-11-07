#include <Axe/BootInfo.h>
#include <Axe/Graphics/VGA.h>
#include <Axe/NXN/BPrintf.h>
#include <Axe/NXN/BPanic.h>
#include <Axe/ATA/ATA.h>
#include <Axe/ELF/ELF.h>
#include <Axe/x86/Video.h>
#include <Axe/System/SystemMemory.h>
#include <Axe/x86/Paging/PageManager.h>
#include <Axe/Memory/BMalloc.h>

typedef void (*KernelEntry)(const Axe::BootInfo* bootInfo);

namespace Axe
{
	x86::PageManager s_PageManager;

	void DumpSystemMemory(const Axe::SystemMemoryInfo* memoryInfo)
	{
		BPrintf("\n----------System Memory Map----------\n");
		for (uint32_t i = 0; i < memoryInfo->RegionCount; i++)
		{
			const auto& regionInfo = memoryInfo->Regions[i];
			BPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			BPrintf("Length= 0x%x, ", regionInfo.Length);
			BPrintf("Type= 0x%x, ", regionInfo.Type);
			BPrintf("ExtendedAttributes= 0x%x}\n", regionInfo.ExtendedAttributes);
		}
		BPrintf("----------System Memory Map----------\n\n");
	}

	uint64_t RequestPhysicalMemory(const Axe::SystemMemoryInfo* memoryInfo, size_t requestedPageCount)
	{
		uint64_t startAddress = 0;
		uint64_t endAddress = 0;

		for (size_t i = 0; i < memoryInfo->RegionCount; i++)
		{
			if (memoryInfo->Regions[i].Type == Axe::SystemMemoryRegionType::Usable)
			{
				if (memoryInfo->Regions[i].BaseAddress > 0)
				{
					startAddress = memoryInfo->Regions[i].BaseAddress;
					endAddress = memoryInfo->Regions[i].BaseAddress + memoryInfo->Regions[i].Length;

					uint32_t pageCount = (endAddress - startAddress) / x86_PAGE_SIZE;
					if (pageCount > requestedPageCount)
						return startAddress;
				}
			}
		}
		return 0;
	}

	bool IdentityMapBootloader()
	{
		// Identity Map first 1MB for Bootloader
		uint32_t offset = 0;
		for (uint32_t i = 0; i < 256; i++)
		{
			//BPrintf("MemoryMapping Bootloader: 0x%x -> 0x%x\n", offset, offset);
			if (s_PageManager.Map(offset, offset, true) != 0)
			{
				BPrintf("Failed to MemoryMap Bootloader: 0x%x -> 0x%x\n", offset, offset);
				return false;
			}
			offset += 4096;
		}
		return true;
	}

	bool SetupKernelVirtualAddress(Axe::BootInfo* bootInfo, const ELF::ELF& kernelELF)
	{
		// Request Memory (4MB)
		uint32_t requestedSize = (4 * 1048576);
		uint32_t requestedPageSize = requestedSize / 4096;
		uintptr_t physicalStartAddress = RequestPhysicalMemory(bootInfo->MemoryInfo, requestedPageSize);
		if (!physicalStartAddress)
		{
			BPrintf("Failed to RequestPhysicalMemory Size: %dBytes\n", requestedSize);
			return false;
		}

		// Memory Map Kernel.ELF Program Headers
		uintptr_t physicalEndAddress = physicalStartAddress;
		uintptr_t virtualStartAddress = kernelELF.GetEntryPointAddress();
		uintptr_t virtualEndAddress = virtualStartAddress;

		for (uint32_t i = 0; i < kernelELF.GetProgramHeaderCount(); i++)
		{
			const auto& phdr = kernelELF.GetProgramHeaders()[i];
			uint32_t pageCount = (phdr->MemorySize / 4096) + 1;

			uint32_t offset = 0;
			for(uint32_t j = 0; j < pageCount; j++)
			{
				bool writable = phdr->Flags == ELF::ELFProgramHeaderFlags::ReadWrite;
				//BPrintf("MemoryMapping Kernel: 0x%x -> 0x%x\n", (phdr->VirtualAddress + offset), physicalEndAddress);
				if (s_PageManager.Map(phdr->VirtualAddress + offset, physicalEndAddress, writable) != 0)
				{
					BPrintf("Failed to MemoryMap Kernel: 0x%x -> 0x%x\n", (phdr->VirtualAddress + offset), physicalEndAddress);
					return -1;
				}
				offset += 4096;
				physicalEndAddress += 4096;
				virtualEndAddress += 4096;
			}
		}

		// Map Rest of Kernel
		uint32_t restSize = (physicalEndAddress - physicalStartAddress);
		while(restSize < requestedSize)
		{
			//BPrintf("MemoryMapping Kernel: 0x%x -> 0x%x\n", virtualEndAddress, physicalEndAddress);
			if (s_PageManager.Map(virtualEndAddress, physicalEndAddress, true) != 0)
			{
				BPrintf("Failed to MemoryMap Kernel: 0x%x -> 0x%x\n", virtualEndAddress, physicalEndAddress);
				return -1;
			}
			physicalEndAddress += 4096;
			virtualEndAddress += 4096;
			restSize += 0x1000;
		}
		physicalEndAddress -= 4096;
		virtualEndAddress -= 4096;

		bootInfo->KernelPhysicalStartAddress = physicalStartAddress;
		bootInfo->KernelPhysicalEndAddress = physicalEndAddress;
		bootInfo->KernelVirtualStartAddress = virtualStartAddress;
		bootInfo->KernelVirtualEndAddress = virtualEndAddress;
		return true;
	}

	bool Init()
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();
		BPrintf("Initializing Axe Bootloader...!\n");

		// Parse Kernel.elf
		ELF::ELF kernelELF;
		if (!kernelELF.Parse(ATA::ATADrive::Slave))
		{
			BPrintf("Failed to Parse Kernel.ELF\n");
			return false;
		}

		// BIOS Detect Memory Map
		Axe::SystemMemoryInfo* memoryInfo = DetectSystemMemory();
		if (!memoryInfo)
		{
			BPrintf("Failed to DetectSystemMemory\n");
			return false;
		}

		// BOOT PARAMS
		BootInfo* bootInfo = (BootInfo*)BMalloc(sizeof(BootInfo));
		if (!bootInfo)
		{
			BPrintf("Failed to Malloc for BootInfo\n");
			return false;
		}

		bootInfo->Signature = AXE_BOOT_SIGNATURE;
		bootInfo->MemoryInfo = memoryInfo;

		// Init Paging
		if (s_PageManager.Init() != 0)
		{
			BPrintf("Failed to Init Page Manager!\n");
			return false;
		}
		
		if (!IdentityMapBootloader())
		{
			BPrintf("Axe Failed to IdentityMapBootloader");
			return false;
		}

		// Memory Map Kernel (Virtual Address -> Kernel's linker.ld)
		if (!SetupKernelVirtualAddress(bootInfo, kernelELF))
		{
			BPrintf("Axe Failed to SetupKernelVirtualAddress");
			return false;
		}

		// Load Kernel Page Directory & Enable Paging
		s_PageManager.LoadKernelDirectory();
		s_PageManager.EnablePaging();

		// Load Kernel ELF PHDRS to Physical RAM
		kernelELF.LoadProgramHeadersToPhysicalMemory();

		KernelEntry _kernelstart = (KernelEntry)kernelELF.GetEntryPointAddress();
		_kernelstart(bootInfo);

		return false; // Should never return
	}
}

extern "C" void BLMain()
{
	//Axe::x86::BIOS::PrintChar('H');
	if (!Axe::Init())
		Axe::BPanic("Failed to Initialize Axe Bootloader!\n");
	for(;;);
}
