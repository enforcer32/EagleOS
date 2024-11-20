#include <Handshake/Memory/Malloc.h>
#include <Handshake/HS/Handshake.h>

namespace Handshake
{
	uint8_t* s_AllocAddress = (uint8_t*)HANDSHAKE_MEMORY_FREE_ADDRESS;

	void* Malloc(uint32_t size)
	{
		uint32_t alignedSize = (size % 4096 ? (size - (size % 4096)) + 4096 : size);
		void* ptr = s_AllocAddress;
		s_AllocAddress += alignedSize;
		return ptr;
	}
}
