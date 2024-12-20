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
		uintptr_t BootloaderPhysicalStartAddress;
		uintptr_t BootloaderPhysicalEndAddress;
		size_t BootloaderSize;

		// Kernel
		uintptr_t KernelPhysicalStartAddress;
		uintptr_t KernelPhysicalEndAddress;
		uintptr_t KernelVirtualStartAddress;
		uintptr_t KernelVirtualEndAddress;
		size_t KernelSize;
	
		// Memory Map
		MemoryMap SystemMemoryMap;
	};
}
