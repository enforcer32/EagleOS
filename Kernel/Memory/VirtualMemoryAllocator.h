#pragma once

#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/ESTD/Bitmap.h>

namespace Kernel
{
	namespace Memory
	{
		enum class VirtualPageState : uint8_t
		{
			Free = 0x0,
			Reserved = 0x1
		};

		class VirtualMemoryAllocator
		{
		public:
			bool Init(VirtualAddress startAddress, size_t size);
			VirtualAddress AllocatePage();
			VirtualAddress AllocatePages(size_t pageCount);
			bool FreePage(VirtualAddress address);
			bool FreePages(VirtualAddress address, size_t pageCount);
			void ReservePage(VirtualAddress address);
			void ReservePages(VirtualAddress address, size_t pageCount);
			size_t GetPageSize() const;
			
		private:
			void SetRegionState(VirtualAddress address, size_t sizeBytes, VirtualPageState state);
			size_t AddressToPageNumber(VirtualAddress address) const;
			VirtualAddress PageNumberToAddress(size_t pageNumber) const;

		private:
			size_t m_PageSize;
			size_t m_MemorySizeBytes;
			size_t m_PageCount;
			VirtualAddress m_StartAddress;
			VirtualAddress m_EndAddress;
			ESTD::Bitmap<131072> m_Bitmap;
		};
	}

	extern Memory::VirtualMemoryAllocator* g_KernelVMA;
}
