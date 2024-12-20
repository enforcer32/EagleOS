#pragma once

#include <Kernel/ESTD/Types.h>

void* operator new(size_t size);
void* operator new[](size_t size);
void operator delete(void* ptr);
void operator delete[](void* ptr);
void operator delete(void* ptr, size_t size);
void operator delete[](void* ptr, size_t size);

namespace ESTD
{
	void* Malloc(size_t size);
	void Free(void* ptr);
}
