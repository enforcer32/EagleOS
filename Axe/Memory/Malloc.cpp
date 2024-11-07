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
}
