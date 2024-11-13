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
		int32_t VirtualMemoryManager::Init(const Axe::BootInfo* bootInfo)
		{
			KPrintf("Initializing VirtualMemoryManager...\n");
			m_CurrentPageDirectory = (PageDirectory*)bootInfo->KernelPageDirectory;
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
				KPrintf("SHOULD NOT EXECUTED\n");
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

		bool VirtualMemoryManager::IsAddressAligned(uintptr_t address, size_t alignment)
		{
			return (address % alignment == 0);
		}
	}
}
