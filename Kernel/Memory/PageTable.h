#pragma once

#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PageInfo.h>

namespace Kernel
{
	namespace Memory
	{
		enum class PageTableFlags : uint32_t
		{
			Present = 0x1,
			ReadWrite = 0x2,
			User = 0x4,
			WriteThrough = 0x8,
			CacheDisable = 0x10,
			Accessed = 0x20,
			Dirty = 0x40,
			PAT = 0x80,
			Global = 0x100,
		};

		class PageTableEntry
		{
		public:
			inline operator uint32_t() const { return m_Entry; }
			
			void Clear();
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
			bool IsDirty() const;
			bool IsGlobal() const;
			void SetGlobal(bool value);			
			PhysicalAddress GetAddress() const;
			void SetAddress(PhysicalAddress address);

		private:
			uint32_t m_Entry;
		};
		
		class PageTable
		{
		public:
			PageTableEntry* GetEntry(VirtualAddress virtualAddress);
			void SetEntry(uint32_t index, PageTableEntry entry);
			void DumpPageTableEntries(uint32_t table) const;
			void DumpPageTableEntry(VirtualAddress virtualAddress) const;

		private:
			PageTableEntry m_Entries[PAGE_TABLE_ENTRY_COUNT]{};
		};
	}
}
