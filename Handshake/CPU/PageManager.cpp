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
			ESTD::Memset(m_PageDirectory, 0, sizeof(uint32_t) * PAGE_DIRECTORY_TABLE_COUNT);
			ESTD::Memset(m_PageTable0, 0, sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);
			ESTD::Memset(m_PageTable768, 0, sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);
			return 0;
		}

		int32_t PageManager::Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable)
		{
			if (!IsAddressAligned(physicalAddress, PAGE_SIZE) || !IsAddressAligned(virtualAddress, PAGE_SIZE))
			{
				Printf("PageManager->Map Failed Address Not Aligned!\n");
				return -1;
			}

			if((virtualAddress > (1024 * 1024)) && (virtualAddress < 0xC0000000 && virtualAddress > (0xc0000000+(1024*4096))))
			{
				Printf("PageManager->No Page Table Allocated for VirtualAddress: 0x%x\n", virtualAddress);
				return -1;
			}

			uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
			auto& directoryEntry = ((uint32_t*)m_PageDirectory)[pageDirectoryIndex];
			if(!(directoryEntry & 0x1))
			{	
				if(pageDirectoryIndex == 0)
				{
					directoryEntry = ((directoryEntry & ~0x7FFFF000) | (uint32_t)m_PageTable0);
				}
				else if(pageDirectoryIndex == 768)
				{
					directoryEntry = ((directoryEntry & ~0x7FFFF000) | (uint32_t)m_PageTable768);
				}
				else
				{
					Printf("PageManager->Invalid Page Directory Entry Index: %d\n", pageDirectoryIndex);
					return -1;
				}
				directoryEntry |= 0x1;
				directoryEntry |= 0x2;
			}

			uint32_t* table = (uint32_t*)PAGE_TABLE_PHYSICAL_ADDRESS(&directoryEntry);
			if (!table)
			{
				Printf("PageManager->Table Not Present For Page Directory of Addresses: 0x%x -> P:0x%x\n", virtualAddress, physicalAddress);
				return -1;
			}

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
			CPU_LoadPageDirectory((void*)m_PageDirectory);
		}

		void* PageManager::GetPageDirectory() const
		{
			return (void*)m_PageDirectory;
		}

		bool PageManager::IsAddressAligned(uint32_t address, uint32_t alignment) const
		{
			return (address % alignment == 0);
		}
	}
}
