#include <Kernel/Memory/PageTable.h>
#include <Kernel/NXN/Bitwise.h>

namespace Kernel
{
	namespace Memory
	{
		bool PageTableEntry::IsPresent() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Present);
		}

		void PageTableEntry::SetPresent(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::Present);
		}

		bool PageTableEntry::IsWritable() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::ReadWrite);
		}

		void PageTableEntry::SetWritable(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::ReadWrite);
		}

		bool PageTableEntry::IsUser() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::User);
		}

		void PageTableEntry::SetUser(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::User);
		}

		bool PageTableEntry::IsWriteThrough() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::WriteThrough);
		}

		void PageTableEntry::SetWriteThrough(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::WriteThrough);
		}

		bool PageTableEntry::IsCacheDisable() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::CacheDisable);
		}

		void PageTableEntry::SetCacheDisable(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::CacheDisable);
		}

		bool PageTableEntry::IsAccessed() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Accessed);
		}

		bool PageTableEntry::IsDirty() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Dirty);
		}

		bool PageTableEntry::IsGlobal() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Global);
		}

		void PageTableEntry::SetGlobal(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::Global);
		}

		PhysicalAddress PageTableEntry::GetAddress() const
		{
			return (m_Entry & ~0xFFF);
		}
		
		void PageTableEntry::SetAddress(PhysicalAddress address)
		{
			m_Entry = ((m_Entry & ~0x7FFFF000) | address);
		}

		PageTableEntry* PageTable::GetEntry(VirtualAddress virtualAddress)
		{
			if (!virtualAddress)
				return nullptr;
			return &m_Entries[PAGE_TABLE_ENTRY_INDEX(virtualAddress)];
		}

		void PageTable::SetEntry(uint32_t index, PageTableEntry entry)
		{
			m_Entries[index] = entry;
		}
	}
}
