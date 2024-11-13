#pragma once

#include <Axe/BootInfo.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <ESTD/Bitset.h>

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
			int32_t Init(const Axe::BootInfo* bootInfo, size_t pageSize);

			PhysicalAddress AllocatePage();
			PhysicalAddress AllocatePages(size_t pageCount);
			void FreePage(PhysicalAddress address);
			void FreePages(PhysicalAddress address, size_t pageCount);
			void ReservePage(PhysicalAddress address);
			void ReservePages(PhysicalAddress address, size_t pageCount);

			size_t GetPageSize() const;

		private:
			bool InitMemory(const Axe::SystemMemoryInfo* memoryInfo);
			bool InitBitmap(const Axe::BootInfo* bootInfo);
			bool InitFreeMemory(const Axe::SystemMemoryInfo* memoryInfo);

			void SetRegionState(PhysicalAddress address, size_t sizeBytes, PageState state);
			void SetPages(size_t page, size_t pageCount, PageState state);
			PageState GetPageState(size_t page) const;
			void SetPageState(size_t page, PageState state);
			int64_t FindFreePages(size_t pageCount) const;

			size_t AddressToPage(PhysicalAddress address) const;
			PhysicalAddress PageToAddress(size_t page) const;

		private:
			size_t m_PageSize;
			size_t m_MemorySizeBytes;
			size_t m_PageCount;
			PhysicalAddress m_StartAddress;
			PhysicalAddress m_EndAddress;
			uint8_t* m_Bitmap;
			PhysicalAddress m_BitmapPhysicalAddress;
		};
	}

	extern Memory::PhysicalMemoryManager* g_KernelPMM;
}
