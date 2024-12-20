#pragma once

#include <Kernel/ESTD/Types.h>

namespace Kernel
{
	namespace Memory
	{
		bool KMallocInit();
		void* KMalloc(uint32_t size);
		void* KZAlloc(uint32_t size);
		void KFree(void* ptr);
	}
}
