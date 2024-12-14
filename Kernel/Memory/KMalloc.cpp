#include <Kernel/Memory/KMalloc.h>
#include <Kernel/Memory/Heap.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/Kern/KPanic.h>
#include <ESTD/CString.h>

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

namespace Kernel
{
	namespace Memory
	{
		bool KMallocInit()
		{
			KPrintf("Initializing KMalloc...\n");
			return true;
		}

		void* KMalloc(size_t size)
		{
			return g_KernelHeap->Allocate(size);
		}

		void* KZAlloc(size_t size)
		{
			void* ptr = g_KernelHeap->Allocate(size);
			if (!ptr)
				return 0;
			ESTD::Memset(ptr, 0x0, size);
			return ptr;
		}

		void KFree(void* ptr)
		{
			g_KernelHeap->Free(ptr);
		}
	}
}
