#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/CString.h>
#include <ESTD/Algorithm.h>

namespace Kernel
{
	Memory::VirtualMemoryManager* g_KernelVMM = nullptr;

	namespace Memory
	{
		extern "C" void x86_EnablePaging();
		extern "C" void x86_DisablePaging();
		extern "C" void x86_LoadPageDirectory(void* dir);

		bool VirtualMemoryManager::Init(const Handshake::BootInfo* bootInfo)
		{
			m_VirtualStartAddress = bootInfo->KernelVirtualStartAddress;
			ESTD::Memset(&m_PageDirectory, 0, sizeof(uint32_t) * PAGE_DIRECTORY_TABLE_COUNT);
			ESTD::Memset(&m_PageTable0, 0, sizeof(m_PageTable0));
			ESTD::Memset(&m_PageTables768, 0, sizeof(m_PageTables768));
			return true;
		}

		bool VirtualMemoryManager::Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable)
		{
			if (!ESTD::IsAligned(physicalAddress, PAGE_SIZE) || !ESTD::IsAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("VirtualMemoryManager->Map(0x%x -> 0x%x) Failed, Address Not Aligned!\n", virtualAddress, physicalAddress);
				return false;
			}

			//if ((virtualAddress > (1024 * 1024)) && ((virtualAddress < 0xC0000000) && (virtualAddress > (0xc0000000 + ((1024*4096) * 1)))))
			uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
			if(pageDirectoryIndex != 0 && (pageDirectoryIndex < 768 || pageDirectoryIndex > 1023))
			{
				KPrintf("VirtualMemoryManager->No Page Table Allocated for VirtualAddress: 0x%x\n", virtualAddress);
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
					KPrintf("VirtualMemoryManager->Invalid Page Directory Entry Index: %d\n", pageDirectoryIndex);
					return false;
				}

				directoryEntry->SetWritable(writable);
				directoryEntry->SetPresent(true);
			}

			PageTable* table = m_PageDirectory.GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("VirtualMemoryManager->Table Not Present For Page Directory of VirtualAddress: 0x%x!\n", virtualAddress);
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

		bool VirtualMemoryManager::MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count)
		{
			for(uint32_t i = 0; i < count; i++)
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
	}
}
