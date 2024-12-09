#include <Kernel/Memory/PageTable.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/Bitwise.h>

namespace Kernel
{
	namespace Memory
	{
		bool PageTableEntry::IsPresent() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Present);
		}

		void PageTableEntry::SetPresent(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::Present);
		}

		bool PageTableEntry::IsWritable() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::ReadWrite);
		}

		void PageTableEntry::SetWritable(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::ReadWrite);
		}

		bool PageTableEntry::IsUser() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::User);
		}

		void PageTableEntry::SetUser(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::User);
		}

		bool PageTableEntry::IsWriteThrough() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::WriteThrough);
		}

		void PageTableEntry::SetWriteThrough(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::WriteThrough);
		}

		bool PageTableEntry::IsCacheDisable() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::CacheDisable);
		}

		void PageTableEntry::SetCacheDisable(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::CacheDisable);
		}

		bool PageTableEntry::IsAccessed() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Accessed);
		}

		bool PageTableEntry::IsDirty() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Dirty);
		}

		bool PageTableEntry::IsGlobal() const
		{
			return ESTD::Bitwise::BitmaskTestOne(m_Entry, (uint32_t)PageTableFlags::Global);
		}

		void PageTableEntry::SetGlobal(bool value)
		{
			ESTD::Bitwise::BitmaskSet(m_Entry, (uint32_t)PageTableFlags::Global);
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
			return &m_Entries[PAGE_TABLE_ENTRY_INDEX(virtualAddress)];
		}

		void PageTable::SetEntry(uint32_t index, PageTableEntry entry)
		{
			m_Entries[index] = entry;
		}

		void PageTable::DumpPageTableEntries(uint32_t table) const
		{
			KPrintf("\n-------------------------DumpPageTableEntry---------------------\n");
			KPrintf("[P][W][U][WT][CD][A][D][G][VIRT][PHYS][IDX]\n");
			for(uint32_t i = 0; i < 21; i++)
			{
				const auto& entry = &m_Entries[i];
				uint32_t virtualAddress = ((0x400000 * table) + (0x1000 * i));
				KPrintf("[%d][%d][%d][ %d][ %d][%d][%d][%d}[0x%x][0x%x][%d]\n",
							entry->IsPresent(), entry->IsWritable(), entry->IsUser(), entry->IsWriteThrough(),
							entry->IsCacheDisable(), entry->IsAccessed(), entry->IsDirty(), entry->IsGlobal(), 
							virtualAddress, entry->GetAddress(), i);
			}
			KPrintf("-------------------------DumpPageTableEntry---------------------\n");
		}

		void PageTable::DumpPageTableEntry(VirtualAddress virtualAddress) const
		{
			const auto& entry = &m_Entries[PAGE_TABLE_ENTRY_INDEX(virtualAddress)];
			KPrintf("\n-------------------------DumpPageTableEntry---------------------\n");
			KPrintf("VirtualAddress: 0x%x, PageTableIndex: %d\n", virtualAddress, PAGE_TABLE_ENTRY_INDEX(virtualAddress));
			KPrintf("IsPresent: %s\n", (entry->IsPresent() ? "true" : "false"));
			KPrintf("IsWritable: %s\n", (entry->IsWritable() ? "true" : "false"));
			KPrintf("IsUser: %s\n", (entry->IsUser() ? "true" : "false"));
			KPrintf("IsWriteThrough: %s\n", (entry->IsWriteThrough() ? "true" : "false"));
			KPrintf("IsCacheDisable: %s\n", (entry->IsCacheDisable() ? "true" : "false"));
			KPrintf("IsAccessed: %s\n", (entry->IsAccessed() ? "true" : "false"));
			KPrintf("IsDirty: %s\n", (entry->IsDirty() ? "true" : "false"));
			KPrintf("IsGlobal: %s\n", (entry->IsGlobal() ? "true" : "false"));
			KPrintf("Mapped PhysicalAddress: 0x%x\n", entry->GetAddress());
			KPrintf("-------------------------DumpPageTableEntry---------------------\n");
		}
	}
}
