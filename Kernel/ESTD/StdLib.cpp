#include <Kernel/ESTD/StdLib.h>
#include <Kernel/Memory/KMalloc.h>

void* operator new(size_t size)
{
	return Kernel::Memory::KMalloc(size);
}

void* operator new[](size_t size)
{
	return Kernel::Memory::KMalloc(size);
}

void operator delete(void* ptr)
{
	Kernel::Memory::KFree(ptr);
}

void operator delete[](void* ptr)
{
	Kernel::Memory::KFree(ptr);
}

void operator delete(void* ptr, size_t size)
{
	Kernel::Memory::KFree(ptr);
}

void operator delete[](void* ptr, size_t size)
{
	Kernel::Memory::KFree(ptr);
}

namespace ESTD
{
	void* Malloc(size_t size)
	{
		return Kernel::Memory::KMalloc(size);
	}

	void Free(void* ptr)
	{
		Kernel::Memory::KFree(ptr);
	}
}
