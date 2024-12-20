#include <Kernel/Drivers/Graphics/VGA.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/Kern/KPanic.h>
#include <Kernel/Arch/x86/Processor.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Memory/VirtualMemoryAllocator.h>
#include <Kernel/Memory/Heap.h>
#include <Kernel/Memory/KMalloc.h>
#include <Kernel/Drivers/Storage/ATA/ATA.h>
#include <Kernel/Boot/Multiboot.h>
#include <Kernel/Boot/BootInfo.h>

namespace Kernel
{
	Memory::PhysicalMemoryManager _KernelPMM;
	Memory::VirtualMemoryManager _KernelVMM;
	Memory::VirtualMemoryAllocator _KernelVMA;
	Memory::Heap _KernelHeap;

	void DumpSystemMemoryMap(const Boot::BootInfo* bootInfo)
	{
		KPrintf("\n----------System Memory Map----------\n");
		Kernel::KPrintf("MemoryLower 0x%x, MemoryUpper 0x%x\n", bootInfo->SystemMemoryMap.MemoryLower, bootInfo->SystemMemoryMap.MemoryUpper);
		for (uint32_t i = 0; i < bootInfo->SystemMemoryMap.RegionCount; i++)
		{
			const auto& regionInfo = bootInfo->SystemMemoryMap.Regions[i];
			Kernel::KPrintf("Region: {Base= 0x%x, ", regionInfo.BaseAddress);
			Kernel::KPrintf("Length= 0x%x, ", regionInfo.Length);
			Kernel::KPrintf("Type= 0x%x}\n", regionInfo.Type);
		}
		KPrintf("----------System Memory Map----------\n\n");
	}

	void DumpBootInfo(const Boot::BootInfo* bootInfo)
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
		KPrintf("----------BootInfo----------\n\n");
	}

	bool InitPhysicalMemoryManager(const Boot::BootInfo* bootInfo)
	{
		// Init Physical Memory Manager
		g_KernelPMM = &_KernelPMM;
		if (!g_KernelPMM->Init(bootInfo))
		{
			KPrintf("Failed to Initialize g_KernelPMM\n");
			return false;
		}
		
		// Reserve Bootloader Pages
		uint32_t bootloaderPageCount = (bootInfo->BootloaderSize / 4096) + 1;
		g_KernelPMM->ReservePages(bootInfo->BootloaderPhysicalStartAddress, bootloaderPageCount);

		// Reserve Kernel Pages
		uint32_t kernelPageCount = bootInfo->KernelSize / 4096;
		g_KernelPMM->ReservePages(bootInfo->KernelPhysicalStartAddress, kernelPageCount);

		// Reserve first 16MB for ISA16 DMA
		uint32_t dma16PageCount = 0x1000000 / 4096;
		g_KernelPMM->ReservePages(0x0, dma16PageCount);

		// Reserve Address 0x0
		g_KernelPMM->ReservePage(0x0);
		return true;
	}

	bool InitVirtualMemoryManager(const Boot::BootInfo* bootInfo)
	{
		g_KernelVMM = &_KernelVMM;
		if (!g_KernelVMM->Init(bootInfo))
		{
			KPrintf("Failed to Initialize g_KernelVMM\n");
			return false;
		}

		// Identity Map first 4MB (Bootloader/DMA)
		uint32_t bootloaderPageCount = 0x400000 / PAGE_SIZE;
		if (!g_KernelVMM->MapRange(0x0, 0x0, true, bootloaderPageCount))
		{
			KPrintf("InitVirtualMemoryManager->PageManager Failed to Identity Map first 4mb!\n");
			return false;
		}

		// ReSetup Higher Half Kernel
		uint32_t kernelPhysicalStartAddressPageNumber = (bootInfo->KernelPhysicalStartAddress / PAGE_SIZE);
		uint32_t kernelPhysicalEndAddressPageNumber = (bootInfo->KernelPhysicalEndAddress / PAGE_SIZE);
		uint32_t kernelPageCount = (kernelPhysicalEndAddressPageNumber - kernelPhysicalStartAddressPageNumber); // 44 (0xC002C000)
		if (!g_KernelVMM->MapRange(bootInfo->KernelVirtualStartAddress, bootInfo->KernelPhysicalStartAddress, true, kernelPageCount))
		{
			KPrintf("InitVirtualMemoryManager->PageManager Failed to Map Kernel to Higher Half!\n");
			return false;
		}

		// Map Rest of Higher Half Table
		uint32_t pageTablePageCount = 0x400000 / PAGE_SIZE;
		if(kernelPageCount < pageTablePageCount)
		{
			if (!g_KernelVMM->MapRange(bootInfo->KernelVirtualStartAddress + (kernelPageCount * PAGE_SIZE), bootInfo->KernelPhysicalStartAddress + (kernelPageCount * PAGE_SIZE), true, (pageTablePageCount - kernelPageCount)))
			{
				KPrintf("InitVirtualMemoryManager->PageManager Failed to Map Kernel to Rest of the Higher Half!\n");
				return false;
			}
		}

		g_KernelVMM->ReloadPageDirectory();
		return true;
	}

	bool InitVirtualMemoryAllocator(const Boot::BootInfo* bootInfo)
	{	
		uint32_t kernelVMAStartAddress = bootInfo->KernelVirtualStartAddress + (4096 * 1024 * 4); // 0xC1000000
		uint32_t kernelVMASize = 0xFFFFFFFF - kernelVMAStartAddress;

		g_KernelVMA = &_KernelVMA;
		if (!g_KernelVMA->Init(kernelVMAStartAddress, kernelVMASize))
		{
			KPrintf("Failed to Initialize g_KernelVMA\n");
			return false;
		}

		return true;
	}

	bool InitKernelHeap(const Boot::BootInfo* bootInfo)
	{
		uint32_t kernelHeapSize = (100 * 1048576); // 100mb

		g_KernelHeap = &_KernelHeap;
		if (!g_KernelHeap->Init(kernelHeapSize))
		{
			KPrintf("Failed to Initialize g_KernelHeap\n");
			return false;
		}
		
		return true;
	}

	bool InitKMalloc(const Boot::BootInfo* bootInfo)
	{
		if (!Memory::KMallocInit())
		{
			KPrintf("Failed to Initialize KMallocInit\n");
			return false;
		}
		
		return true;
	}
	
	bool InitMemoryManager(const Boot::BootInfo* bootInfo)
	{
		if(!Memory::MemoryUtil::Init(bootInfo))
		{
			KPrintf("Failed to Initialize MemoryUtil\n");
			return false;
		}

		if (!InitPhysicalMemoryManager(bootInfo))
		{
			KPrintf("Failed to Initialize PhysicalMemoryManager\n");
			return false;
		}

		if (!InitVirtualMemoryManager(bootInfo))
		{
			KPrintf("Failed to Initialize VirtualMemoryManager\n");
			return false;
		}

		if (!InitVirtualMemoryAllocator(bootInfo))
		{
			KPrintf("Failed to Initialize VirtualMemoryAllocator\n");
			return false;
		}

		if (!InitKernelHeap(bootInfo))
		{
			KPrintf("Failed to Initialize KernelHeap\n");
			return false;
		}

		if (!InitKMalloc(bootInfo))
		{
			KPrintf("Failed to Initialize KMalloc\n");
			return false;
		}

		return true;
	}

	void InitKernel(const Boot::BootInfo* bootInfo)
	{
		Graphics::VGA::Init();
		Graphics::VGA::ClearScreen();

		//DumpBootInfo(bootInfo);
		//DumpSystemMemoryMap(bootInfo);

		if (x86::Processor::Init() != 0)
			KPanic("Failed to Initialize x86 Processor!\n");

		if (!InitMemoryManager(bootInfo))
			KPanic("Failed to Initialize Memory Manager\n");

		//Storage::ATA ata;
		//ata.Init(Storage::ATABus::Primary, Storage::ATADrive::Master);

		KPrintf("\nEagle Operating System\n");
	}
}

extern "C" uint32_t _bootloader_physical_start;
extern "C" uint32_t _bootloader_physical_end;
extern "C" uint32_t _kernel_physical_start;
extern "C" uint32_t _kernel_physical_end;
extern "C" uint32_t _kernel_virtual_start;
extern "C" uint32_t _kernel_virtual_end;

extern "C" void KMain(uint32_t multibootMagic, Boot::MultibootInfo* multibootInfo)
{
	if(multibootMagic != MULTIBOOT_BOOTLOADER_MAGIC)
		return;

	// Setup Boot Info
	Boot::BootInfo bootInfo;
	bootInfo.Signature = EAGLEOS_BOOT_SIGNATURE;

	// Bootloader
	bootInfo.BootloaderPhysicalStartAddress = (uint32_t)&_bootloader_physical_start;
	bootInfo.BootloaderPhysicalEndAddress = (uint32_t)&_bootloader_physical_end;
	bootInfo.BootloaderSize = ((uint32_t)&_bootloader_physical_end - (uint32_t)&_bootloader_physical_start);

	// Kernel
	bootInfo.KernelPhysicalStartAddress = (uint32_t)&_kernel_physical_start;
	bootInfo.KernelPhysicalEndAddress = (uint32_t)&_kernel_physical_end;
	bootInfo.KernelVirtualStartAddress = (uint32_t)&_kernel_virtual_start;
	bootInfo.KernelVirtualEndAddress = (uint32_t)&_kernel_virtual_end;
	bootInfo.KernelSize = ((uint32_t)&_kernel_virtual_end - (uint32_t)&_kernel_virtual_start);

	// Memory Map
	bootInfo.SystemMemoryMap.MemoryLower = (multibootInfo->MemoryLower * 1024);
	bootInfo.SystemMemoryMap.MemoryUpper = (multibootInfo->MemoryUpper * 1024);
	bootInfo.SystemMemoryMap.RegionCount = (multibootInfo->MemoryMapLength / (sizeof(Boot::MultibootMemoryMap)));
	bootInfo.SystemMemoryMap.Regions = (Boot::MemoryRegion*)multibootInfo->MemoryMapAddress;

	Kernel::InitKernel(&bootInfo);
		
	for(;;);
}
