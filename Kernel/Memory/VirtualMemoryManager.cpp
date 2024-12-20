#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Kern/KPrintf.h>
#include <Kernel/ESTD/CString.h>
#include <Kernel/ESTD/Algorithm.h>

namespace Kernel
{
	Memory::VirtualMemoryManager* g_KernelVMM = nullptr;

	namespace Memory
	{
		extern "C" void x86_EnablePaging();
		extern "C" void x86_DisablePaging();
		extern "C" void x86_LoadPageDirectory(void* dir);

		bool VirtualMemoryManager::Init(const Boot::BootInfo* bootInfo)
		{
			KPrintf("Initializing VirtualMemoryManager...\n");
			m_VirtualStartAddress = bootInfo->KernelVirtualStartAddress;
			ESTD::Memset(&m_PageDirectory, 0, sizeof(m_PageDirectory));
			ESTD::Memset(&m_PageTable0, 0, sizeof(m_PageTable0));
			ESTD::Memset(&m_PageTables768, 0, sizeof(m_PageTables768));
			return true;
		}

		bool VirtualMemoryManager::Map(VirtualAddress virtualAddress, PhysicalAddress physicalAddress, bool writable)
		{
			if (!ESTD::IsAligned(physicalAddress, PAGE_SIZE) || !ESTD::IsAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("VirtualMemoryManager->Map(0x%x -> 0x%x) Failed, Address Not Aligned!\n", virtualAddress, physicalAddress);
				return false;
			}

			uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
			if(pageDirectoryIndex != 0 && (pageDirectoryIndex < 768 || pageDirectoryIndex > 1023))
			{
				KPrintf("VirtualMemoryManager->Map(0x%x) No Page Table Allocated for VirtualAddress!\n", virtualAddress);
				return false;
			}
			
			const auto& directoryEntry = m_PageDirectory.GetEntry(virtualAddress);
			if(!directoryEntry->IsPresent())
			{
				if (pageDirectoryIndex == 0)
				{
					directoryEntry->SetAddress(MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageTable0));
				}
				else if (pageDirectoryIndex >= 768 && pageDirectoryIndex <= 1023)
				{
					uint32_t tablesIndex = (virtualAddress - m_VirtualStartAddress) / 0x400000;
					directoryEntry->SetAddress(MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageTables768[tablesIndex]));
				}
				else
				{
					KPrintf("VirtualMemoryManager->Map(0x%x) Invalid Page Directory Entry Index: %d!\n", virtualAddress, pageDirectoryIndex);
					return false;
				}

				directoryEntry->SetWritable(writable);
				directoryEntry->SetPresent(true);
			}

			PageTable* table = m_PageDirectory.GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("VirtualMemoryManager->Map(0x%x) Table Not Present For Page Directory of VirtualAddress!\n", virtualAddress);
				return false;
			}

			const auto& tableEntry = table->GetEntry(virtualAddress);
			if (tableEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->Map(0x%x -> 0x%x) Failed, Table Entry Already Present!\n", virtualAddress, physicalAddress);
				return false;
			}

			tableEntry->SetWritable(writable);
			tableEntry->SetAddress(physicalAddress);
			tableEntry->SetPresent(true);
			return true;
		}

		bool VirtualMemoryManager::MapRange(VirtualAddress virtualAddress, PhysicalAddress physicalAddress, bool writable, size_t count)
		{
			for(size_t i = 0; i < count; i++)
			{
				if (!Map(virtualAddress, physicalAddress, writable))
				{
					KPrintf("VirtualMemoryManager->Map(0x%x -> 0x%x) Failed to MemoryMap!\n", virtualAddress, physicalAddress);
					return false;
				}
				virtualAddress += PAGE_SIZE;
				physicalAddress += PAGE_SIZE;
			}
			return true;
		}

		bool VirtualMemoryManager::Unmap(VirtualAddress virtualAddress)
		{
			if (!ESTD::IsAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("VirtualMemoryManager->Unmap(0x%x) Failed, Address Not Aligned!\n", virtualAddress);
				return false;
			}

			uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
			if(pageDirectoryIndex != 0 && (pageDirectoryIndex < 768 || pageDirectoryIndex > 1023))
			{
				KPrintf("VirtualMemoryManager->Unmap(0x%x) No Page Table Allocated for VirtualAddress!\n", virtualAddress);
				return false;
			}
			
			const auto& directoryEntry = m_PageDirectory.GetEntry(virtualAddress);
			if(!directoryEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->Unmap(0x%x) No Page Directory Entry Present for VirtualAddress!\n", virtualAddress);
				return false;
			}

			PageTable* table = m_PageDirectory.GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("VirtualMemoryManager->Unmap(0x%x) Table Not Present For Page Directory of VirtualAddress!\n", virtualAddress);
				return false;
			}

			const auto& tableEntry = table->GetEntry(virtualAddress);
			if (!tableEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->Unmap(0x%x) Failed, No Page Table Entry Present for VirtualAddress!\n", virtualAddress);
				return false;
			}

			tableEntry->Clear();
			return true;
		}

		bool VirtualMemoryManager::UnmapRange(VirtualAddress virtualAddress, size_t count)
		{
			for(size_t i = 0; i < count; i++)
			{
				if (!Unmap(virtualAddress))
				{
					KPrintf("VirtualMemoryManager->Unmap(0x%x) Failed to MemoryUnmap!\n", virtualAddress);
					return false;
				}
				FlushTLB(virtualAddress);
				virtualAddress += PAGE_SIZE;
			}
			return true;
		}

		void VirtualMemoryManager::FlushTLB(VirtualAddress virtualAddress)
		{
			asm volatile("invlpg (%0)" :: "r" (virtualAddress) : "memory");
		}

		void VirtualMemoryManager::FlushTLBRange(VirtualAddress virtualAddress, size_t count)
		{
			for(size_t i = 0; i < count; i++)
				FlushTLB(virtualAddress + (i * PAGE_SIZE));
		}

		void VirtualMemoryManager::EnablePaging()
		{
			x86_EnablePaging();
		}

		void VirtualMemoryManager::DisablePaging()
		{
			x86_DisablePaging();
		}

		void VirtualMemoryManager::ReloadPageDirectory()
		{
			x86_LoadPageDirectory((void*)MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageDirectory));
		}

		PhysicalAddress VirtualMemoryManager::GetPhysicalAddress(VirtualAddress virtualAddress)
		{
			if (!ESTD::IsAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("VirtualMemoryManager->GetPhysicalAddress(0x%x) Failed, Address Not Aligned!\n", virtualAddress);
				return 0;
			}

			uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
			if(pageDirectoryIndex != 0 && (pageDirectoryIndex < 768 || pageDirectoryIndex > 1023))
			{
				KPrintf("VirtualMemoryManager->GetPhysicalAddress(0x%x) No Page Table Allocated for VirtualAddress!\n", virtualAddress);
				return 0;
			}
			
			const auto& directoryEntry = m_PageDirectory.GetEntry(virtualAddress);
			if(!directoryEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->GetPhysicalAddress(0x%x) No Page Directory Entry Present for VirtualAddress!\n", virtualAddress);
				return 0;
			}

			PageTable* table = m_PageDirectory.GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("VirtualMemoryManager->GetPhysicalAddress(0x%x) Table Not Present For Page Directory of VirtualAddress!\n", virtualAddress);
				return 0;
			}

			const auto& tableEntry = table->GetEntry(virtualAddress);
			if (!tableEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->GetPhysicalAddress(0x%x) Failed, No Page Table Entry Present for VirtualAddress!\n", virtualAddress);
				return 0;
			}

			return tableEntry->GetAddress();
		}
	}
}
