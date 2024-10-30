#pragma once

#include <Bootloader/System/SystemMemory.h>

struct BootInfo
{
	uint32_t Signature;
	uint32_t* KernelAddress;
	Bootloader::SystemMemoryInfo* SystemMemoryInfo;
};
