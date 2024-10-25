#include <Bootloader/Memory/Malloc.h>
#include <Bootloader/Memory/MemoryInfo.h>

namespace Bootloader
{
	uint8_t* s_AllocAddress = (uint8_t*)BOOTLOADER_MEMORY_FREE_ADDRESS;

	void* Malloc(uint32_t size)
	{
		void* ptr = s_AllocAddress;
		s_AllocAddress += size;
		return ptr;
	}

	void* Memset(void* dst, int32_t ch, uint32_t num)
	{
		uint8_t* p1 = (uint8_t*)dst;
		while (num--)
			*p1++ = ch;
		return dst;
	}
}
