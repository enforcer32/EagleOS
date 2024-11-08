#pragma once

#include <Axe/System/SystemMemory.h>

#define AXE_BOOT_SIGNATURE 0xDEADD432

namespace Axe
{
	struct BootInfo
	{
		uint32_t Signature;
		uintptr_t KernelPhysicalStartAddress;
		uintptr_t KernelPhysicalEndAddress;
		uintptr_t KernelVirtualStartAddress;
		uintptr_t KernelVirtualEndAddress;
		void* KernelPageDirectory;
		SystemMemoryInfo* MemoryInfo;
	};
}
