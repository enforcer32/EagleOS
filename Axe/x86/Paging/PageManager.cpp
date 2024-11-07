#include <Axe/x86/Paging/PageManager.h>
#include <Axe/Memory/Malloc.h>

namespace Axe
{
	namespace x86
	{
		extern "C" void x86_EnablePaging();
		extern "C" void x86_DisablePaging();
		extern "C" void x86_LoadPageDirectory(void* dir);

		int32_t PageManager::Init()
		{
			m_KernelDirectory = (uint32_t*)Malloc(sizeof(uint32_t) * x86_PAGE_DIRECTORY_TABLE_COUNT);
			if (!m_KernelDirectory)
				return -1;
			Memset(m_KernelDirectory, 0, sizeof(uint32_t) * x86_PAGE_DIRECTORY_TABLE_COUNT);
			return 0;
		}

		int32_t PageManager::Map(uintptr_t virtualAddress, uintptr_t physicalAddress, bool writable)
		{
			if (!IsAddressAligned(physicalAddress, x86_PAGE_SIZE) || !IsAddressAligned(virtualAddress, x86_PAGE_SIZE))
				return -1;

			auto& directoryEntry = ((uint32_t*)m_KernelDirectory)[x86_PAGE_DIRECTORY_TABLE_INDEX(virtualAddress)];
			if(!(directoryEntry & 0x1))
			{
				uint32_t* table = (uint32_t*)Malloc(sizeof(uint32_t) * x86_PAGE_TABLE_ENTRY_COUNT);
				if (!table)
					return -1;

				directoryEntry = ((directoryEntry & ~0x7FFFF000) | (uintptr_t)table);
				directoryEntry |= 0x1;
				directoryEntry |= 0x2;
			}

			uint32_t* table = (uint32_t*)((directoryEntry & ~0xFFF));
			if (!table)
				return -1;

			auto& tableEntry = ((uint32_t*)table)[x86_PAGE_TABLE_ENTRY_INDEX(virtualAddress)];
			if(tableEntry & 0x1)
				return -1;

			tableEntry |= 0x1;
			if (writable)
				tableEntry |= 0x2;
			tableEntry = ((tableEntry & ~0x7FFFF000) | physicalAddress);
			return 0;
		}

		void PageManager::LoadKernelDirectory() const
		{
			x86_LoadPageDirectory(m_KernelDirectory);
		}

		void PageManager::EnablePaging() const
		{
			x86_EnablePaging();
		}
		
		void PageManager::DisablePaging() const
		{
			x86_DisablePaging();
		}

		bool PageManager::IsAddressAligned(uintptr_t address, uint32_t alignment) const
		{
			return (address % alignment == 0);
		}
	}
}
