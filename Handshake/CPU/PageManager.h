#pragma once

#include <ESTD/Types.h>

#define PAGE_DIRECTORY_TABLE_COUNT 1024
#define PAGE_TABLE_ENTRY_COUNT 1024
#define PAGE_SIZE 4096

#define PAGE_DIRECTORY_TABLE_INDEX(vaddr)	((uint32_t)vaddr / (PAGE_DIRECTORY_TABLE_COUNT * PAGE_SIZE))
#define PAGE_TABLE_ENTRY_INDEX(vaddr)		((uint32_t)vaddr % (PAGE_TABLE_ENTRY_COUNT * PAGE_SIZE) / PAGE_SIZE)
#define PAGE_TABLE_PHYSICAL_ADDRESS(dirent) ((*dirent) & ~0xFFF)

namespace Handshake
{
	namespace CPU
	{
		class PageManager
		{
		public:
			int32_t Init();
			int32_t Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable);
			int32_t MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count);
			void EnablePaging() const;
			void DisablePaging() const;
			void LoadPageDirectory() const;
			void* GetPageDirectory() const;
			
		private:
			bool IsAddressAligned(uint32_t address, uint32_t alignment) const;

		private:
			uint32_t m_PageDirectory[PAGE_DIRECTORY_TABLE_COUNT];
			uint32_t m_PageTable0[PAGE_TABLE_ENTRY_COUNT];
			uint32_t m_PageTable768[PAGE_TABLE_ENTRY_COUNT];
		};
	}
}
