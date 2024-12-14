#pragma once

#include <ESTD/Types.h>

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, size_t size);
void operator delete[](void* ptr, size_t size);

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
