#pragma once

#include <Kernel/Boot/BootInfo.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/ESTD/Bitmap.h>

namespace Kernel
{
	namespace Memory
	{
		enum class PageState : uint8_t
		{
			Free = 0x0,
			Reserved = 0x1
		};

		class PhysicalMemoryManager
		{
		public:
			bool Init(const Boot::BootInfo* bootInfo);
			PhysicalAddress AllocatePage();
			PhysicalAddress AllocatePages(size_t pageCount);
			void FreePage(PhysicalAddress address);
			void FreePages(PhysicalAddress address, size_t pageCount);
			void ReservePage(PhysicalAddress address);
			void ReservePages(PhysicalAddress address, size_t pageCount);
			size_t GetPageSize() const;

		private:
			bool InitMemory(const Boot::BootInfo* bootInfo);
			bool InitBitmap(const Boot::BootInfo* bootInfo);
			bool InitFreeMemory(const Boot::BootInfo* bootInfo);
			void SetRegionState(PhysicalAddress address, size_t sizeBytes, PageState state);
			size_t AddressToPageNumber(PhysicalAddress address) const;
			PhysicalAddress PageNumberToAddress(size_t pageNumber) const;

		private:
			size_t m_PageSize;
			size_t m_MemorySizeBytes;
			size_t m_PageCount;
			PhysicalAddress m_StartAddress;
			PhysicalAddress m_EndAddress;
			ESTD::Bitmap<131072> m_Bitmap;
		};
	}

	extern Memory::PhysicalMemoryManager* g_KernelPMM;
}
