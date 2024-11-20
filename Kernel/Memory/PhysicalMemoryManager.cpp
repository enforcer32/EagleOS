#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/CString.h>
#include <ESTD/Errno.h>

namespace Kernel
{
	Memory::PhysicalMemoryManager* g_KernelPMM = nullptr;

	namespace Memory
	{
		//int32_t PhysicalMemoryManager::Init(const Axe::BootInfo* bootInfo, size_t pageSize)
		//{
		//	KPrintf("Initializing PhysicalMemoryManager...\n");
		//	m_PageSize = pageSize;

		//	if (!InitMemory(bootInfo->MemoryInfo))
		//	{
		//		KPrintf("PhysicalMemoryManager Failed to InitMemory\n");
		//		return -1;
		//	}

		//	if (!InitBitmap(bootInfo))
		//	{
		//		KPrintf("PhysicalMemoryManager Failed to InitBitmap\n");
		//		return -1;
		//	}

		//	if (!InitFreeMemory(bootInfo->MemoryInfo))
		//	{
		//		KPrintf("PhysicalMemoryManager Failed to InitFreeMemory\n");
		//		return -1;
		//	}
	
		//	KPrintf("PhysicalMemoryManager{PageSize = %d, PageCount = %d, MemorySize = %dMB, StartAddress = 0x%x, EndAddress = 0x%x, BitmapAddress = 0x%x}\n", m_PageSize, m_PageCount, (m_MemorySizeBytes / 1048576), m_StartAddress, m_EndAddress, m_Bitmap);
		//	return 0;
		//}

		PhysicalAddress PhysicalMemoryManager::AllocatePage()
		{
			return AllocatePages(1);
		}
		
		PhysicalAddress PhysicalMemoryManager::AllocatePages(size_t pageCount)
		{
			if (!pageCount)
				return -1;

			int64_t firstPage = FindFreePages(pageCount);
			if(firstPage < 0)
				return -1;
			
			SetPages(firstPage, pageCount, PageState::Reserved);
			return PageToAddress(firstPage);
		}

		void PhysicalMemoryManager::FreePage(PhysicalAddress address)
		{
			FreePages(address, 1);
		}

		void PhysicalMemoryManager::FreePages(PhysicalAddress address, size_t pageCount)
		{
			SetPages(AddressToPage(address), pageCount, PageState::Free);
		}

		void PhysicalMemoryManager::ReservePage(PhysicalAddress address)
		{
			ReservePages(address, 1);
		}

		void PhysicalMemoryManager::ReservePages(PhysicalAddress address, size_t pageCount)
		{
			SetPages(AddressToPage(address), pageCount, PageState::Reserved);
		}

		size_t PhysicalMemoryManager::GetPageSize() const
		{
			return m_PageSize;
		}

		//bool PhysicalMemoryManager::InitMemory(const Axe::SystemMemoryInfo* memoryInfo)
		//{
		//	uint64_t memoryStart = -1;
		//	uint64_t memoryEnd = 0;

		//	for(size_t i = 0; i < memoryInfo->RegionCount; i++)
		//	{
		//		if(memoryInfo->Regions[i].BaseAddress < memoryStart)
		//			memoryStart = memoryInfo->Regions[i].BaseAddress;
		//		if(memoryInfo->Regions[i].BaseAddress + memoryInfo->Regions[i].Length > memoryEnd)
		//			memoryEnd = memoryInfo->Regions[i].BaseAddress + memoryInfo->Regions[i].Length;					
		//	}

		//	m_StartAddress = memoryStart;
		//	m_EndAddress = memoryEnd;
		//	m_MemorySizeBytes = m_EndAddress - m_StartAddress;
		//	m_PageCount = m_MemorySizeBytes / m_PageSize;
		//	return true;
		//}

		//bool PhysicalMemoryManager::InitBitmap(const Axe::BootInfo* bootInfo)
		//{
		//	m_Bitmap = (uint8_t*)bootInfo->KernelVirtualStartAddress + (100 * m_PageSize);
		//	m_BitmapPhysicalAddress = bootInfo->KernelPhysicalStartAddress + (100 * m_PageSize);
		//	ESTD::Memset(m_Bitmap, static_cast<uint8_t>(PageState::Reserved), m_PageCount);
		//	return true;
		//}
		
		//bool PhysicalMemoryManager::InitFreeMemory(const Axe::SystemMemoryInfo* memoryInfo)
		//{
		//	for(size_t i = 0; i < memoryInfo->RegionCount; i++)
		//	{
		//		const auto& region = memoryInfo->Regions[i];
		//		if(region.Type == Axe::SystemMemoryRegionType::Usable && region.Length > 0)
		//		{
		//			SetRegionState(region.BaseAddress, region.Length, PageState::Free);
		//		}
		//	}
		//	SetRegionState(m_BitmapPhysicalAddress, m_PageCount, PageState::Reserved);
		//	return true;
		//}

		void PhysicalMemoryManager::SetRegionState(PhysicalAddress address, size_t sizeBytes, PageState state)
		{
			size_t page = AddressToPage(address);
			size_t pageCount = sizeBytes / m_PageSize;
			SetPages(page, pageCount, state);
		}

		void PhysicalMemoryManager::SetPages(size_t page, size_t pageCount, PageState state)
		{
			for(size_t i = page; i < (page + pageCount); i++)
				SetPageState(i, state);
		}

		PageState PhysicalMemoryManager::GetPageState(size_t page) const
		{
			if (page > m_PageCount)
				return PageState::Reserved;
			size_t byteIndex = page / 8;
			size_t bitIndex = page % 8;
			return static_cast<PageState>(((m_Bitmap[byteIndex] & (1 << bitIndex)) != 0));
		}

		void PhysicalMemoryManager::SetPageState(size_t page, PageState state)
		{
			if (page > m_PageCount)
				return;
			size_t byteIndex = page / 8;
			size_t bitIndex = page % 8;

			if(state == PageState::Free)
				m_Bitmap[byteIndex] &= ~(1 << bitIndex);
			else if(state == PageState::Reserved)
				m_Bitmap[byteIndex] |= (1 << bitIndex);
		}

		int64_t PhysicalMemoryManager::FindFreePages(size_t pageCount) const
		{
			int64_t firstPage = -1;
			size_t currentPage = 0;

			for(size_t i = 0; i < m_PageCount; i++)
			{
				if(GetPageState(i) == PageState::Reserved)
				{
					firstPage = -1;
					currentPage = 0;
					continue;
				}

				if(firstPage == -1)
					firstPage = i;
				
				currentPage++;
				if(currentPage == pageCount)
					break;
			}

			if(firstPage == -1)
				return -ENOMEM;

			return firstPage;
		}

		size_t PhysicalMemoryManager::AddressToPage(PhysicalAddress address) const
		{
			return (address - m_StartAddress) / m_PageSize;
		}

		PhysicalAddress PhysicalMemoryManager::PageToAddress(size_t page) const
		{
			return (m_StartAddress + (page * m_PageSize));
		}
	}
}
