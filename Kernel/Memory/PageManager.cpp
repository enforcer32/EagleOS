#include <Kernel/Memory/PageManager.h>
#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/CString.h>
#include <ESTD/Algorithm.h>

namespace Kernel
{
	Memory::PageManager* g_KernelPageManager = nullptr;

	namespace Memory
	{
		extern "C" void x86_EnablePaging();
		extern "C" void x86_DisablePaging();
		extern "C" void x86_LoadPageDirectory(void* dir);

		bool PageManager::Init(const Handshake::BootInfo* bootInfo)
		{
			ESTD::Memset(&m_PageDirectory, 0, sizeof(uint32_t) * PAGE_DIRECTORY_TABLE_COUNT);
			ESTD::Memset(&m_PageTable0, 0, sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);
			ESTD::Memset(&m_PageTable768, 0, sizeof(uint32_t) * PAGE_TABLE_ENTRY_COUNT);
			return true;
		}

		bool PageManager::Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable)
		{
			if (!ESTD::IsAligned(physicalAddress, PAGE_SIZE) || !ESTD::IsAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("PageManager->Map(0x%x -> 0x%x) Failed, Address Not Aligned!\n", virtualAddress, physicalAddress);
				return false;
			}

			if ((virtualAddress > (1024 * 1024)) && ((virtualAddress < 0xC0000000) && (virtualAddress > (0xc0000000 + ((1024*4096) * 1)))))
			{
				KPrintf("PageManager->No Page Table Allocated for VirtualAddress: 0x%x\n", virtualAddress);
				return false;
			}
			
			const auto& directoryEntry = m_PageDirectory.GetEntry(virtualAddress);
			if(!directoryEntry->IsPresent())
			{
				uint32_t pageDirectoryIndex = PAGE_DIRECTORY_TABLE_INDEX(virtualAddress);
				if (pageDirectoryIndex == 0)
				{
					directoryEntry->SetAddress(MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageTable0));
				}
				else if (pageDirectoryIndex == 768)
				{
					directoryEntry->SetAddress(MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageTable768));
				}
				else
				{
					KPrintf("PageManager->Invalid Page Directory Entry Index: %d\n", pageDirectoryIndex);
					return false;
				}

				directoryEntry->SetWritable(writable);
				directoryEntry->SetPresent(true);
			}

			PageTable* table = m_PageDirectory.GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("PageManager->Table Not Present For Page Directory of VirtualAddress: 0x%x!\n", virtualAddress);
				return false;
			}

			const auto& tableEntry = table->GetEntry(virtualAddress);
			if (tableEntry->IsPresent())
			{
				KPrintf("PageManager->Map(0x%x -> 0x%x) Failed, Table Entry Already Present!\n", virtualAddress, physicalAddress);
				return false;
			}

			tableEntry->SetWritable(writable);
			tableEntry->SetAddress(physicalAddress);
			tableEntry->SetPresent(true);
			return true;
		}

		bool PageManager::MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count)
		{
			for(uint32_t i = 0; i < count; i++)
			{
				if (!Map(virtualAddress, physicalAddress, writable))
				{
					KPrintf("PageManager->Map(0x%x -> 0x%x) Failed to MemoryMap!\n", virtualAddress, physicalAddress);
					return false;
				}
				virtualAddress += PAGE_SIZE;
				physicalAddress += PAGE_SIZE;
			}
			return true;
		}

		void PageManager::EnablePaging()
		{
			x86_EnablePaging();
		}

		void PageManager::DisablePaging()
		{
			x86_DisablePaging();
		}

		void PageManager::ReloadPageDirectory()
		{
			x86_LoadPageDirectory((void*)MemoryUtil::KernelVirtualToPhysicalAddress((VirtualAddress)&m_PageDirectory));
		}
	}
}
