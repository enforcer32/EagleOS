#include <Kernel/Memory/PageDirectory.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/Bitwise.h>

namespace Kernel
{
	namespace Memory
	{
		void PageDirectoryEntry::Clear()
		{
			m_Entry = 0;
		}

		bool PageDirectoryEntry::IsPresent() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::Present);
		}

		void PageDirectoryEntry::SetPresent(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::Present);
		}

		bool PageDirectoryEntry::IsWritable() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::ReadWrite);
		}

		void PageDirectoryEntry::SetWritable(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::ReadWrite);
		}

		bool PageDirectoryEntry::IsUser() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::User);
		}

		void PageDirectoryEntry::SetUser(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::User);
		}

		bool PageDirectoryEntry::IsWriteThrough() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::WriteThrough);
		}

		void PageDirectoryEntry::SetWriteThrough(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::WriteThrough);
		}

		bool PageDirectoryEntry::IsCacheDisable() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::CacheDisable);
		}

		void PageDirectoryEntry::SetCacheDisable(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageDirectoryFlags::CacheDisable);
		}

		bool PageDirectoryEntry::IsAccessed() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::Accessed);
		}

		uint32_t PageDirectoryEntry::GetPageSize() const
		{
			return (ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageDirectoryFlags::PageSize) ? 4194304 : 4096);	
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
			return &m_Entries[PAGE_DIRECTORY_TABLE_INDEX(virtualAddress)];
		}

		void PageDirectory::SetEntry(uint32_t index, PageDirectoryEntry entry)
		{
			m_Entries[index] = entry;
		}

		PageTable* PageDirectory::GetTable(VirtualAddress virtualAddress)
		{
			const auto& entry = GetEntry(virtualAddress);
			return reinterpret_cast<PageTable*>(MemoryUtil::KernelPhysicalToVirtualAddress(entry->GetAddress()));
		}

		void PageDirectory::DumpPageDirectoryEntries() const
		{
			KPrintf("\n-------------------------PageDirectoryEntries---------------------\n");
			KPrintf("[P][W][U][WT][CD][A][VIRT][PHYS][IDX]\n");
			for(uint32_t i = 0; i < PAGE_DIRECTORY_TABLE_COUNT; i++)
			{
				const auto& entry = &m_Entries[i];
				uint32_t virtualAddress = (0x400000 * i);
				KPrintf("[%d][%d][%d][ %d][ %d][%d][0x%x][0x%x][%d]\n",
							entry->IsPresent(), entry->IsWritable(), entry->IsUser(), entry->IsWriteThrough(),
							entry->IsCacheDisable(), entry->IsAccessed(), virtualAddress, entry->GetAddress(), i);
			}
			KPrintf("-------------------------PageDirectoryEntries---------------------\n");
		}

		void PageDirectory::DumpPageDirectoryEntry(VirtualAddress virtualAddress) const
		{
			KPrintf("\n-------------------------PageDirectoryEntry---------------------\n");
			KPrintf("[P][W][U][WT][CD][A][VIRT][PHYS][IDX]\n");
			const auto& entry = &m_Entries[PAGE_DIRECTORY_TABLE_INDEX(virtualAddress)];
			KPrintf("[%d][%d][%d][ %d][ %d][%d][0x%x][0x%x][%d]\n",
						entry->IsPresent(), entry->IsWritable(), entry->IsUser(), entry->IsWriteThrough(),
						entry->IsCacheDisable(), entry->IsAccessed(), virtualAddress, entry->GetAddress(), (virtualAddress / 0x400000));
			KPrintf("-------------------------PageDirectoryEntry---------------------\n");
		}
	}
}
