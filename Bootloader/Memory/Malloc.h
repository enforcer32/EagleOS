#pragma once

#include <stdint.h>

namespace Bootloader
{
	void* Malloc(uint32_t size);
	void* Memset(void* dst, int32_t ch, uint32_t num);
	void* Memcpy(void* dst, const void* src, uint32_t num);
}
