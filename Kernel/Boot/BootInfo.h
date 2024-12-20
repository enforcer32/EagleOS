#pragma once

#include <Kernel/Boot/MemoryMap.h>

#define EAGLEOS_BOOT_SIGNATURE 0xDEADD432

namespace Boot
{
	struct BootInfo
	{
		// Boot Signature
		uint32_t Signature;

		// Bootloader
		uint32_t BootloaderPhysicalStartAddress;
		uint32_t BootloaderPhysicalEndAddress;
		uint32_t BootloaderSize;

		// Kernel
		uint32_t KernelPhysicalStartAddress;
		uint32_t KernelPhysicalEndAddress;
		uint32_t KernelVirtualStartAddress;
		uint32_t KernelVirtualEndAddress;
		uint32_t KernelSize;
	
		// Memory Map
		MemoryMap SystemMemoryMap;
	};
}
