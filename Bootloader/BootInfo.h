#pragma once

#include <stdint.h>

struct BootInfo
{
	uint32_t Signature;
	uint32_t* KernelAddress;
};
