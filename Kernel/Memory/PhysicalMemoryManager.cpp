#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/NXN/KPrintf.h>
#include <Kernel/NXN/CString.h>
#include <Kernel/NXN/Errno.h>

namespace Kernel
{
	namespace Memory
	{
		int32_t PhysicalMemoryManager::Init(const Axe::SystemMemoryInfo* memoryInfo, size_t pageSize)
		{
			KPrintf("Initializing PhysicalMemoryManager...\n");
			m_PageSize = pageSize;

			if (!InitMemory(memoryInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitMemory\n");
				return -1;
			}

			if (!InitBitmap(memoryInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitBitmap\n");
				return -1;
			}

			if (!InitFreeMemory(memoryInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitFreeMemory\n");
				return -1;
			}
	
			KPrintf("PhysicalMemoryManager{PageSize = %d, PageCount = %d, MemorySize = %dMB, StartAddress = 0x%x, EndAddress = 0x%x, BitmapAddress = 0x%x}\n", m_PageSize, m_PageCount, (m_MemorySizeBytes / 1048576), m_StartAddress, m_EndAddress, m_Bitmap);
			return 0;
		}
		
		void* PhysicalMemoryManager::AllocatePages(size_t pageCount)
		{
			if (!pageCount)
				return nullptr;

			int64_t firstPage = FindFreePages(pageCount);
			if(firstPage < 0)
				return nullptr;
			
			SetPages(firstPage, pageCount, PageState::Reserved);
			return PageToAddress(firstPage);
		}

		void PhysicalMemoryManager::FreePages(void* address, size_t pageCount)
		{
			SetPages(AddressToPage(address), pageCount, PageState::Free);
		}

		void PhysicalMemoryManager::ReservePages(void* address, size_t pageCount)
		{
			SetPages(AddressToPage(address), pageCount, PageState::Reserved);
		}

		bool PhysicalMemoryManager::InitMemory(const Axe::SystemMemoryInfo* memoryInfo)
		{
			uint64_t memoryStart = -1;
			uint64_t memoryEnd = 0;

			for(size_t i = 0; i < memoryInfo->RegionCount; i++)
			{
				if(memoryInfo->Regions[i].BaseAddress < memoryStart)
					memoryStart = memoryInfo->Regions[i].BaseAddress;
				if(memoryInfo->Regions[i].BaseAddress + memoryInfo->Regions[i].Length > memoryEnd)
					memoryEnd = memoryInfo->Regions[i].BaseAddress + memoryInfo->Regions[i].Length;					
			}

			m_StartAddress = (void*)memoryStart;
			m_EndAddress = (void*)memoryEnd;
			m_MemorySizeBytes = (uint8_t*)m_EndAddress - (uint8_t*)m_StartAddress;
			m_PageCount = m_MemorySizeBytes / m_PageSize;
			return true;
		}

		bool PhysicalMemoryManager::InitBitmap(const Axe::SystemMemoryInfo* memoryInfo)
		{
			size_t i;
			for(i = 0; i < memoryInfo->RegionCount; i++)
				if (memoryInfo->Regions[i].Type == Axe::SystemMemoryRegionType::Usable && memoryInfo->Regions[i].BaseAddress > 0 && memoryInfo->Regions[i].Length > 0)
					break;

			m_Bitmap = (uint8_t*)memoryInfo->Regions[i].BaseAddress + (100 * m_PageSize);
			NXN::Memset(m_Bitmap, static_cast<uint8_t>(PageState::Reserved), m_PageCount);
			return true;
		}
		
		bool PhysicalMemoryManager::InitFreeMemory(const Axe::SystemMemoryInfo* memoryInfo)
		{
			for(size_t i = 0; i < memoryInfo->RegionCount; i++)
			{
				const auto& region = memoryInfo->Regions[i];
				if(region.Type == Axe::SystemMemoryRegionType::Usable && region.Length > 0)
				{
					SetRegionState((void*)region.BaseAddress, region.Length, PageState::Free);
				}
			}
			SetRegionState(m_Bitmap, m_PageCount, PageState::Reserved);
			return true;
		}

		void PhysicalMemoryManager::SetRegionState(void* address, size_t sizeBytes, PageState state)
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

		size_t PhysicalMemoryManager::AddressToPage(void* address)
		{
			return ((uint8_t*)address - (uint8_t*)m_StartAddress) / m_PageSize;
		}

		void* PhysicalMemoryManager::PageToAddress(size_t page)
		{
			return ((uint8_t*)m_StartAddress + (page * m_PageSize));
		}
	}
}
