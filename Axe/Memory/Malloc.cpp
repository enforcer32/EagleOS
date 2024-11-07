#include <Axe/Memory/Malloc.h>
#include <Axe/Boot/MemoryInfo.h>

namespace Axe
{
	uint8_t* s_AllocAddress = (uint8_t*)BOOTLOADER_MEMORY_FREE_ADDRESS;

	void* Malloc(uint32_t size)
	{
		uint32_t alignedSize = (size % 4096 ? (size - (size % 4096)) + 4096 : size);
		void* ptr = s_AllocAddress;
		s_AllocAddress += alignedSize;
		return ptr;
	}

	void* Memset(void* dst, int32_t ch, uint32_t num)
	{
		uint8_t* p1 = (uint8_t*)dst;
		while (num--)
			*p1++ = ch;
		return dst;
	}

	void* Memcpy(void* dst, const void* src, uint32_t num)
	{
		uint8_t* p1 = (uint8_t*)dst;
		const uint8_t* p2 = (uint8_t*)src;
		while (num--)
			*p1++ = *p2++;
		return dst;
	}
}
