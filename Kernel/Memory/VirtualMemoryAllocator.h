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
			bool Init(VirtualAddress startAddress, uint32_t size);
			VirtualAddress AllocatePage();
			VirtualAddress AllocatePages(uint32_t pageCount);
			bool FreePage(VirtualAddress address);
			bool FreePages(VirtualAddress address, uint32_t pageCount);
			void ReservePage(VirtualAddress address);
			void ReservePages(VirtualAddress address, uint32_t pageCount);
			uint32_t GetPageSize() const;
			
		private:
			void SetRegionState(VirtualAddress address, uint32_t sizeBytes, VirtualPageState state);
			uint32_t AddressToPageNumber(VirtualAddress address) const;
			VirtualAddress PageNumberToAddress(uint32_t pageNumber) const;

		private:
			uint32_t m_PageSize;
			uint32_t m_MemorySizeBytes;
			uint32_t m_PageCount;
			VirtualAddress m_StartAddress;
			VirtualAddress m_EndAddress;
			ESTD::Bitmap<131072> m_Bitmap;
		};
	}

	extern Memory::VirtualMemoryAllocator* g_KernelVMA;
}
