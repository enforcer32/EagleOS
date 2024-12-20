#include <Kernel/Memory/Heap.h>
#include <Kernel/Memory/VirtualMemoryAllocator.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/ESTD/Algorithm.h>

namespace Kernel
{
	Memory::Heap* g_KernelHeap = nullptr;

	namespace Memory
	{
		bool Heap::Init(uint32_t sizeBytes)
		{
			KPrintf("Initializing Heap...\n");
			if (!sizeBytes)
			{
				KPrintf("Heap->Failed to Initialize with Size: %d!\n", sizeBytes);
				return false;
			}

			m_MemorySizeBytes = ESTD::AlignDown(sizeBytes, g_KernelVMA->GetPageSize());
			VirtualAddress startAddress = g_KernelVMA->AllocatePages(m_MemorySizeBytes / g_KernelVMA->GetPageSize());
			if (!startAddress)
			{
				KPrintf("Heap->Failed to Allocate VMA for Size: %d\n", m_MemorySizeBytes);
				return false;
			}
			
			m_StartAddress = (void*)startAddress;
			m_EndAddress = (void*)(startAddress + m_MemorySizeBytes);
			m_CurrentAddress = (uint8_t*)m_StartAddress;
			ESTD::Memset(m_StartAddress, 0, sizeof(HeapNode));
			KPrintf("Heap{Size = %dMB, StartAddress = 0x%x, EndAddress = 0x%x}\n", (m_MemorySizeBytes / 1048576), m_StartAddress, m_EndAddress);
			return true;
		}

		void* Heap::Allocate(uint32_t sizeBytes)
		{
			if (!sizeBytes)
				return nullptr;

			HeapNode* node = (HeapNode*)m_StartAddress;
			HeapNode* prev = node->Prev;
			while((uint8_t*)node < m_CurrentAddress)
			{
				if(node->Size >= sizeBytes && node->State == HeapState::Free)
				{
					node->State = HeapState::Reserved;
					return ((uint8_t*)node + sizeof(HeapNode));
				}
				prev = node;
				node = (HeapNode*)((uint8_t*)node + (node->Size + sizeof(HeapNode)));
			}

			node->Size = sizeBytes;
			node->State = HeapState::Reserved;
			node->Prev = prev;
			node->Next = nullptr;
			if(prev)
				prev->Next = node;
			m_CurrentAddress += (node->Size + sizeof(HeapNode));
			return ((uint8_t*)node + sizeof(HeapNode));
		}
	
		void Heap::Free(void* address)
		{
			if (!address)
				return;

			HeapNode* node = (HeapNode*)((uint8_t*)address - sizeof(HeapNode));
			if (node->State == HeapState::Reserved)
				node->State = HeapState::Free;

			HeapNode* next = node->Next;
			if (next && next->State == HeapState::Free)
			{
				HeapNode* prev = node->Prev;
				next->Size += node->Size;
				next->Prev = node->Prev;
				if (prev)
					prev->Next = next;
			}

			HeapNode* prev = node->Prev;
			if (prev && prev->State == HeapState::Free)
			{
				HeapNode* next = node->Next;
				prev->Size += node->Size;
				prev->Next = node->Next;
				if (next)
					next->Prev = prev;
			}
		}

		void Heap::Dump() const
		{
			KPrintf("\n--------------------HeapNodes(Size=%d)---------------------\n", sizeof(HeapNode));
			uint32_t i = 0;
			HeapNode* node = (HeapNode*)m_StartAddress;
			while(node)
			{
				KPrintf("Node: %d, Size: %d, State: %d, NodeAddress: 0x%x, DataAddress: 0x%x, NextNodeAddress: 0x%x, PrevNodeAddress: 0x%x\n\n", i++, node->Size, node->State, node, ((uint8_t*)node + sizeof(HeapNode)), node->Next, node->Prev);
				node = node->Next;
			}
			KPrintf("--------------------HeapNodes(Size=%d)---------------------\n", sizeof(HeapNode));
		}
	}
}
