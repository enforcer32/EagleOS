#pragma once

#include <Axe/System/SystemMemory.h>

namespace Axe
{
	struct BootInfo
	{
		uint32_t Signature;
		uint32_t* KernelAddress;
		SystemMemoryInfo* MemoryInfo;
	};
}
