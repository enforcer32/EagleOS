#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Kern/KPrintf.h>

namespace Kernel
{
	Memory::PhysicalMemoryManager* g_KernelPMM = nullptr;

	namespace Memory
	{
		bool PhysicalMemoryManager::Init(const Boot::BootInfo* bootInfo)
		{
			KPrintf("Initializing PhysicalMemoryManager...\n");
			m_PageSize = 4096;

			if (!InitMemory(bootInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitMemory\n");
				return false;
			}

			if (!InitBitmap(bootInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitBitmap\n");
				return false;
			}

			if (!InitFreeMemory(bootInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to InitFreeMemory\n");
				return false;
			}
	
			KPrintf("PhysicalMemoryManager{PageSize = %d, PageCount = %d, MemorySize = %dMB, StartAddress = 0x%x, EndAddress = 0x%x}\n", m_PageSize, m_PageCount, (m_MemorySizeBytes / 1048576), m_StartAddress, m_EndAddress);
			return true;
		}

		PhysicalAddress PhysicalMemoryManager::AllocatePage()
		{
			return AllocatePages(1);
		}
		
		PhysicalAddress PhysicalMemoryManager::AllocatePages(uint32_t pageCount)
		{
			if (!pageCount)
				return 0;

			int64_t firstPage = m_Bitmap.FindUnsetBits(pageCount);
			if(firstPage < 0)
				return 0;
			
			m_Bitmap.SetBits(firstPage, pageCount);
			return PageNumberToAddress(firstPage);
		}

		void PhysicalMemoryManager::FreePage(PhysicalAddress address)
		{
			FreePages(address, 1);
		}

		void PhysicalMemoryManager::FreePages(PhysicalAddress address, uint32_t pageCount)
		{
			if (!address)
				return;
			m_Bitmap.ClearBits((AddressToPageNumber(address)), pageCount);
		}

		void PhysicalMemoryManager::ReservePage(PhysicalAddress address)
		{
			ReservePages(address, 1);
		}

		void PhysicalMemoryManager::ReservePages(PhysicalAddress address, uint32_t pageCount)
		{
			m_Bitmap.SetBits(AddressToPageNumber(address), pageCount);
		}

		uint32_t PhysicalMemoryManager::GetPageSize() const
		{
			return m_PageSize;
		}

		bool PhysicalMemoryManager::InitMemory(const Boot::BootInfo* bootInfo)
		{
			uint64_t memoryStart = -1;
			uint64_t memoryEnd = 0;

			for (uint32_t i = 0; i < bootInfo->SystemMemoryMap.RegionCount; i++)
			{
				if (bootInfo->SystemMemoryMap.Regions[i].BaseAddress < memoryStart)
					memoryStart = bootInfo->SystemMemoryMap.Regions[i].BaseAddress;

				if ((bootInfo->SystemMemoryMap.Regions[i].BaseAddress <= bootInfo->SystemMemoryMap.MemoryUpper) && (bootInfo->SystemMemoryMap.Regions[i].BaseAddress + bootInfo->SystemMemoryMap.Regions[i].Length > memoryEnd))
					memoryEnd = bootInfo->SystemMemoryMap.Regions[i].BaseAddress + bootInfo->SystemMemoryMap.Regions[i].Length;
			}

			m_StartAddress = memoryStart;
			m_EndAddress = memoryEnd;
			m_MemorySizeBytes = m_EndAddress - m_StartAddress;
			m_PageCount = m_MemorySizeBytes / m_PageSize;
			return true;
		}

		bool PhysicalMemoryManager::InitBitmap(const Boot::BootInfo* bootInfo)
		{
			m_Bitmap.SetAll();
			return true;
		}
		
		bool PhysicalMemoryManager::InitFreeMemory(const Boot::BootInfo* bootInfo)
		{
			for(uint32_t i = 0; i < bootInfo->SystemMemoryMap.RegionCount; i++)
			{
				const auto& region = bootInfo->SystemMemoryMap.Regions[i];
				if(region.Type == Boot::MemoryRegionType::Available && region.Length > 0)
				{
					SetRegionState(region.BaseAddress, region.Length, PageState::Free);
				}
			}
			return true;
		}

		void PhysicalMemoryManager::SetRegionState(PhysicalAddress address, uint32_t sizeBytes, PageState state)
		{
			uint32_t pageNumber = AddressToPageNumber(address);
			uint32_t pageCount = sizeBytes / m_PageSize;
			if (state == PageState::Free)
				m_Bitmap.ClearBits(pageNumber, pageCount);
			else if (state == PageState::Reserved)
				m_Bitmap.SetBits(pageNumber, pageCount);
		}

		uint32_t PhysicalMemoryManager::AddressToPageNumber(PhysicalAddress address) const
		{
			return (address - m_StartAddress) / m_PageSize;
		}

		PhysicalAddress PhysicalMemoryManager::PageNumberToAddress(uint32_t pageNumber) const
		{
			return (m_StartAddress + (pageNumber * m_PageSize));
		}
	}
}
