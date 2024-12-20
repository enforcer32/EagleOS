#pragma once

#include <Kernel/ESTD/Types.h>

namespace Kernel
{
	namespace Memory
	{
		enum class HeapState : uint8_t
		{
			Free = 0x0,
			Reserved = 0x1
		};

		struct HeapNode
		{
			size_t Size;
			HeapState State;
			HeapNode* Next;
			HeapNode* Prev;
		} __attribute__((packed));

		class Heap
		{
		public:
			bool Init(size_t sizeBytes);
			void* Allocate(size_t sizeBytes);
			void Free(void* address);
			void Dump() const;
			
		private:
			size_t m_MemorySizeBytes;
			void* m_StartAddress;
			void* m_EndAddress;
			uint8_t* m_CurrentAddress;
		};
	}

	extern Memory::Heap* g_KernelHeap;
}
