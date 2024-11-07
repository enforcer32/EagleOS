#pragma once

#include <Axe/NXN/Types.h>

#define x86_PAGE_DIRECTORY_TABLE_COUNT 1024
#define x86_PAGE_TABLE_ENTRY_COUNT 1024
#define x86_PAGE_SIZE 4096

#define x86_PAGE_DIRECTORY_TABLE_INDEX(vaddr)	((uint32_t)vaddr / (x86_PAGE_DIRECTORY_TABLE_COUNT * x86_PAGE_SIZE))
#define x86_PAGE_TABLE_ENTRY_INDEX(vaddr)		((uint32_t)vaddr % (x86_PAGE_TABLE_ENTRY_COUNT * x86_PAGE_SIZE) / x86_PAGE_SIZE)
#define x86_PAGE_TABLE_PHYSICAL_ADDRESS(dirent) ((*dirent) & ~0xFFF)

namespace Axe
{
	namespace x86
	{
		class PageManager
		{
		public:
			int32_t Init();
			int32_t Map(uintptr_t virtualAddress, uintptr_t physicalAddress, bool writable);
			void LoadKernelDirectory() const;
			void EnablePaging() const;
			void DisablePaging() const;

		private:
			bool IsAddressAligned(uintptr_t address, uint32_t alignment) const;

		private:
			uint32_t* m_KernelDirectory;
		};
	}
}
