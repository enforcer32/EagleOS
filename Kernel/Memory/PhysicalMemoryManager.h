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
			PhysicalAddress AllocatePages(uint32_t pageCount);
			void FreePage(PhysicalAddress address);
			void FreePages(PhysicalAddress address, uint32_t pageCount);
			void ReservePage(PhysicalAddress address);
			void ReservePages(PhysicalAddress address, uint32_t pageCount);
			uint32_t GetPageSize() const;

		private:
			bool InitMemory(const Boot::BootInfo* bootInfo);
			bool InitBitmap(const Boot::BootInfo* bootInfo);
			bool InitFreeMemory(const Boot::BootInfo* bootInfo);
			void SetRegionState(PhysicalAddress address, uint32_t sizeBytes, PageState state);
			uint32_t AddressToPageNumber(PhysicalAddress address) const;
			PhysicalAddress PageNumberToAddress(uint32_t pageNumber) const;

		private:
			uint32_t m_PageSize;
			uint32_t m_MemorySizeBytes;
			uint32_t m_PageCount;
			PhysicalAddress m_StartAddress;
			PhysicalAddress m_EndAddress;
			ESTD::Bitmap<131072> m_Bitmap;
		};
	}

	extern Memory::PhysicalMemoryManager* g_KernelPMM;
}
