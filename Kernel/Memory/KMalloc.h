#pragma once

#include <Kernel/ESTD/Types.h>

namespace Kernel
{
	namespace Memory
	{
		bool KMallocInit();
		void* KMalloc(size_t size);
		void* KZAlloc(size_t size);
		void KFree(void* ptr);
	}
}
