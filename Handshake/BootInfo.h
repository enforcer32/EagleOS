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

		/*
			Low Memory
			896 MB
			Memory Mapped into Kernel Address Space
			Free Memory Usable/Addressable by Kernel
			Starts After Kernel End
		*/
		uint32_t LowMemoryPhysicalStartAddress;
		uint32_t LowMemoryPhysicalEndAddress;
		uint32_t LowMemoryVirtualStartAddress;
		uint32_t LowMemoryVirtualEndAddress;
		uint32_t LowMemorySize;

		/*
			High Memory
			Not Memory Mapped
			Only Usable if Mapped
			Starts After Low Memory End
		*/
		uint32_t HighMemoryPhysicalStartAddress;
		uint32_t HighMemoryPhysicalEndAddress;

		// Stack
		uint32_t StackBaseVirtualAddress;
		uint32_t StackSize;

		// System Memory Map
		System::MemoryMap* MemoryMap;
	};
}
