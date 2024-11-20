#include <Handshake/CPU/PageManager.h>
#include <Handshake/Stdio/Print.h>
#include <Handshake/HS/Handshake.h>
#include <ESTD/CString.h>

namespace Handshake
{
	namespace CPU
	{
		extern "C" void CPU_EnablePaging();
		extern "C" void CPU_DisablePaging();
		extern "C" void CPU_LoadPageDirectory(void* dir);

		int32_t PageManager::Init()
		{
			m_MallocAddress = HANDSHAKE_MEMORY_KERNEL_PAGEDIRECTORY_START_ADDRESS;
			m_PageDirectory = (uint32_t*)m_MallocAddress;
			m_MallocAddress += (sizeof(uint32_t) * PAGE_DIRECTORY_TABLE_COUNT);
			if (!m_PageDirectory)
			{
				Printf("PageManager->Init Failed to Malloc for s_PageDirectory!\n");
				return -1;
			}
			ESTD::Memset(m_PageDirectory, 0, sizeof(uint32_t) * PAGE_DIRECTORY_TABLE_COUNT);
			return 0;
		}

		int32_t PageManager::Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable)
		{
			if (!IsAddressAligned(physicalAddress, PAGE_SIZE) || !IsAddressAligned(virtualAddress, PAGE_SIZE))
			{
				Printf("PageManager->Map Failed Address Not Aligned!\n");
				return -1;
			}

			auto& directoryEntry = ((uint32_t*)m_PageDirectory)[PAGE_DIRECTORY_TABLE_INDEX(virtualAddress)];
			if(!(directoryEntry & 0x1))
			{	
				// BAD IDEA OVERWRITES MEMORY
				uint32_t* table = (uint32_t*)m_MallocAddress;
				m_MallocAddress += (sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);
				if (!table)
				{
					Printf("PageManager->Map Failed to Malloc for Page Table!\n");
					return -1;
				}
				ESTD::Memset(table, 0, sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);

				directoryEntry = ((directoryEntry & ~0x7FFFF000) | (uint32_t)table);
				directoryEntry |= 0x1;
				directoryEntry |= 0x2;
			}

			uint32_t* table = (uint32_t*)PAGE_TABLE_PHYSICAL_ADDRESS(&directoryEntry);
			if (!table)
				return -1;

			auto& tableEntry = ((uint32_t*)table)[PAGE_TABLE_ENTRY_INDEX(virtualAddress)];
			if (tableEntry & 0x1)
			{
				Printf("PageManager->Map Failed Table Entry Already Present: 0x%x -> P:0x%x\n", virtualAddress, physicalAddress);
				return -1;
			}

			tableEntry |= 0x1;
			if (writable)
				tableEntry |= 0x2;

			tableEntry = ((tableEntry & ~0x7FFFF000) | physicalAddress);
			return 0;
		}

		int32_t PageManager::MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count)
		{
			for(uint32_t i = 0; i < count; i++)
			{
				//Printf("Mapping: 0x%x -> 0x%x\n", virtualAddress, physicalAddress);
				if (Map(virtualAddress, physicalAddress, writable) != 0)
				{
					Printf("PageManager->Map Failed to MemoryMap: 0x%x -> 0x%x\n", virtualAddress, physicalAddress);
					return -1;
				}

				virtualAddress += PAGE_SIZE;
				physicalAddress += PAGE_SIZE;
			}

			return 0;
		}

		void PageManager::EnablePaging() const
		{
			CPU_EnablePaging();
		}
		
		void PageManager::DisablePaging() const
		{
			CPU_DisablePaging();
		}

		void PageManager::LoadPageDirectory() const
		{
			CPU_LoadPageDirectory(m_PageDirectory);
		}

		void* PageManager::GetPageDirectory() const
		{
			return m_PageDirectory;
		}

		bool PageManager::IsAddressAligned(uint32_t address, uint32_t alignment) const
		{
			return (address % alignment == 0);
		}
	}
}
