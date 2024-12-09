#pragma once

#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PageTable.h>

namespace Kernel
{
	namespace Memory
	{
		enum class PageDirectoryFlags : uint32_t
		{
			Present = 0x1,
			ReadWrite = 0x2,
			User = 0x4,
			WriteThrough = 0x8,
			CacheDisable = 0x10,
			Accessed = 0x20,
			PageSize = 0x80,
		};

		class PageDirectoryEntry
		{
		public:
			inline operator uint32_t() const { return m_Entry; }

			bool IsPresent() const;
			void SetPresent(bool value);
			bool IsWritable() const;
			void SetWritable(bool value);
			bool IsUser() const;
			void SetUser(bool value);
			bool IsWriteThrough() const;
			void SetWriteThrough(bool value);
			bool IsCacheDisable() const;
			void SetCacheDisable(bool value);
			bool IsAccessed() const;
			uint32_t GetPageSize() const;
			PhysicalAddress GetAddress() const;
			void SetAddress(PhysicalAddress address);

		private:
			uint32_t m_Entry;
		};

		class PageDirectory
		{
		public:
			PageDirectoryEntry* GetEntry(VirtualAddress virtualAddress);
			void SetEntry(uint32_t index, PageDirectoryEntry entry);
			PageTable* GetTable(VirtualAddress virtualAddress);
			void DumpPageDirectoryEntries() const;
			void DumpPageDirectoryEntry(VirtualAddress virtualAddress) const;

		private:
			PageDirectoryEntry m_Entries[PAGE_DIRECTORY_TABLE_COUNT]{};
		};
	}
}
