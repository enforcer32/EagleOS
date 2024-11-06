#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Arch/x86/Memory/Paging.h>
#include <Kernel/NXN/KPrintf.h>
#include <Kernel/NXN/CString.h>

namespace Kernel
{
	Memory::VirtualMemoryManager* g_KernelVMM = nullptr;

	namespace Memory
	{
		int32_t VirtualMemoryManager::Init()
		{
			KPrintf("Initializing VirtualMemoryManager...\n");

			m_CurrentPageDirectory = (PageDirectory*)g_KernelPMM->AllocatePage();
			NXN::Memset(m_CurrentPageDirectory, 0, 4096);

			if (!InitNullPage())
			{
				KPrintf("Failed to InitNullPage!\n");
				return -1;
			}
			
			if (!IdentityMapKernel())
			{
				KPrintf("Failed to InitNullPage!\n");
				return -1;
			}

			x86::LoadPageDirectory(m_CurrentPageDirectory);
			x86::EnablePaging();
			
			return 0;
		}

		int32_t VirtualMemoryManager::Map(VirtualAddress virtualAddress, PhysicalAddress physicalAddress)
		{
			if (!virtualAddress || !physicalAddress)
			{
				KPrintf("VirtualMemoryManager->Map Error Address Cannot be 0x0: VADDR=0x%x | PADDR=0x%x\n", virtualAddress, physicalAddress);
				return -1;
			}

			if (!IsAddressAligned(physicalAddress, PAGE_SIZE) || !IsAddressAligned(virtualAddress, PAGE_SIZE))
			{
				KPrintf("VirtualMemoryManager->Map Error Address Not Aligned: VADDR=0x%x | PADDR=0x%x\n", virtualAddress, physicalAddress);
				return -1;
			}

			const auto& directoryEntry = m_CurrentPageDirectory->GetEntry(virtualAddress);
			if (!directoryEntry)
			{
				KPrintf("VirtualMemoryManager->Map Error m_CurrentPageDirectory->GetEntry!\n");
				return -1;
			}

			if (!directoryEntry->IsPresent())
			{
				PageTable* table = (PageTable*)g_KernelPMM->AllocatePage();
				directoryEntry->SetAddress((PhysicalAddress)table);
				directoryEntry->SetPresent(1);
				directoryEntry->SetWritable(1);
			}

			PageTable* table = m_CurrentPageDirectory->GetTable(virtualAddress);
			if (!table)
			{
				KPrintf("VirtualMemoryManager->Map Error m_CurrentPageDirectory->GetTable!\n");
				return -1;
			}

			const auto& tableEntry = table->GetEntry(virtualAddress);
			if (!tableEntry)
			{
				KPrintf("VirtualMemoryManager->Map Error table->GetEntry!\n");
				return -1;
			}

			if(tableEntry->IsPresent())
			{
				KPrintf("VirtualMemoryManager->Map Error Cannot Map {Virtual=0x%x TO Physical=0x%x} Table Entry Already Present!\n", virtualAddress, physicalAddress);
				return -1;
			}

			tableEntry->SetPresent(true);
			tableEntry->SetWritable(true);
			tableEntry->SetAddress(physicalAddress);
			return 0;
		}

		bool VirtualMemoryManager::InitNullPage()
		{
			PageTableEntry pte0;
			pte0.SetPresent(true);
			pte0.SetWritable(false);
			pte0.SetAddress(0x0);

			PageTable* pt0 = (PageTable*)g_KernelPMM->AllocatePage();
			pt0->SetEntry(0, pte0);

			PageDirectoryEntry pd0;
			pd0.SetPresent(true);
			pd0.SetWritable(true);
			pd0.SetAddress((PhysicalAddress)pt0);

			m_CurrentPageDirectory->SetEntry(0, pd0);
			return true;
		}
		
		bool VirtualMemoryManager::IdentityMapKernel()
		{
			// Identity Map first 10 MB of Kernel
			uint32_t offset = 4096;
			for (size_t i = 1; i < 2560; i++)
			{
				if (Map(offset, offset) != 0)
				{
					KPrintf("Failed to Map: 0x%x to 0x%x\n", offset, offset);
					return false;
				}
				offset += 4096;
			}
			return true;
		}

		bool VirtualMemoryManager::IsAddressAligned(uintptr_t address, size_t alignment)
		{
			return (address % alignment == 0);
		}
	}
}
