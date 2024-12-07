#pragma once

#include <ESTD/Types.h>
#include <Handshake/System/MemoryMap.h>

#define HANDSHAKE_BOOT_SIGNATURE 0xDEADD432

namespace Handshake
{
	struct BootInfo
	{
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
	
		// System Memory Map
		System::MemoryMap* MemoryMap;
	};
}
