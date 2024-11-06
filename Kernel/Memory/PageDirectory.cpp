#include <Kernel/Memory/PageDirectory.h>
#include <Kernel/NXN/Bitwise.h>

namespace Kernel
{
	namespace Memory
	{
		bool PageDirectoryEntry::IsPresent() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::Present);
		}

		void PageDirectoryEntry::SetPresent(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::Present);
		}

		bool PageDirectoryEntry::IsWritable() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::ReadWrite);
		}

		void PageDirectoryEntry::SetWritable(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::ReadWrite);
		}

		bool PageDirectoryEntry::IsUser() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::User);
		}

		void PageDirectoryEntry::SetUser(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::User);
		}

		bool PageDirectoryEntry::IsWriteThrough() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::WriteThrough);
		}

		void PageDirectoryEntry::SetWriteThrough(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::WriteThrough);
		}

		bool PageDirectoryEntry::IsCacheDisable() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::CacheDisable);
		}

		void PageDirectoryEntry::SetCacheDisable(bool value)
		{
			NXN::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::CacheDisable);
		}

		bool PageDirectoryEntry::IsAccessed() const
		{
			return NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::Accessed);
		}

		size_t PageDirectoryEntry::GetPageSize() const
		{
			return (NXN::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::PageSize) ? 4194304 : 4096);	
		}

		PhysicalAddress PageDirectoryEntry::GetAddress() const
		{
			return (m_Entry & ~0xFFF);
		}
		
		void PageDirectoryEntry::SetAddress(PhysicalAddress address)
		{
			m_Entry = ((m_Entry & ~0x7FFFF000) | address);
		}

		PageDirectoryEntry* PageDirectory::GetEntry(VirtualAddress virtualAddress)
		{
			if (!virtualAddress)
				return nullptr;
			return &m_Tables[PAGE_DIRECTORY_TABLE_INDEX(virtualAddress)];
		}

		void PageDirectory::SetEntry(uint32_t index, PageDirectoryEntry entry)
		{
			m_Tables[index] = entry;
		}

		PageTable* PageDirectory::GetTable(VirtualAddress virtualAddress)
		{
			const auto& entry = GetEntry(virtualAddress);
			return reinterpret_cast<PageTable*>(entry->GetAddress());
		}
	}
}
