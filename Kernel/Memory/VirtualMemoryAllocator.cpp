#include <Kernel/Memory/VirtualMemoryAllocator.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Memory/VirtualMemoryManager.h>
#include <Kernel/Kern/KPrintf.h>

namespace Kernel
{
	Memory::VirtualMemoryAllocator* g_KernelVMA = nullptr;

	namespace Memory
	{
		bool VirtualMemoryAllocator::Init(VirtualAddress startAddress, uint32_t size)
		{
			KPrintf("Initializing VirtualMemoryAllocator...\n");
			m_PageSize = 4096;
			m_StartAddress = startAddress;
			m_EndAddress = startAddress + size;
			m_MemorySizeBytes = m_EndAddress - m_StartAddress;
			m_PageCount = m_MemorySizeBytes / m_PageSize;
			m_Bitmap.ResetAll();
			KPrintf("VirtualMemoryAllocator{AddressSpacePageSize = %d, AddressSpacePageCount = %d, AddressSpaceSize = %dMB, StartAddress = 0x%x, EndAddress = 0x%x}\n", 4096, m_PageCount, (m_MemorySizeBytes / 1048576), m_StartAddress, m_EndAddress);
			return true;
		}

		VirtualAddress VirtualMemoryAllocator::AllocatePage()
		{
			return AllocatePages(1);
		}
		
		VirtualAddress VirtualMemoryAllocator::AllocatePages(uint32_t pageCount)
		{
			if (!pageCount)
				return 0;

			int64_t firstPage = m_Bitmap.FindUnsetBits(pageCount);
			if(firstPage < 0)
				return 0;

			PhysicalAddress paddr = g_KernelPMM->AllocatePages(pageCount);
			if (!paddr)
			{
				KPrintf("VirtualMemoryAllocator->AllocatePages(%d) Failed to Allocate Physical Memory!\n", pageCount);
				return 0;
			}
			
			m_Bitmap.SetBits(firstPage, pageCount);
			VirtualAddress vaddr = PageNumberToAddress(firstPage);

			if (!g_KernelVMM->MapRange(vaddr, paddr, true, pageCount))
			{
				KPrintf("VirtualMemoryAllocator->AllocatePages(%d) Failed to Map(0x%x -> 0x%x)!\n", pageCount, paddr, vaddr);
				// m_Bitmap.ClearBits(firstPage, pageCount); // Don't Execute This For Safety?
				return false;
			}

			KPrintf("VirtualMemoryAllocator->Allocated(0x%x -> 0x%x)!\n", vaddr, paddr);
			return vaddr;
		}

		bool VirtualMemoryAllocator::FreePage(VirtualAddress address)
		{
			return FreePages(address, 1);
		}

		bool VirtualMemoryAllocator::FreePages(VirtualAddress address, uint32_t pageCount)
		{
			if (!address)
				return true;

			PhysicalAddress physicalAddress = g_KernelVMM->GetPhysicalAddress(address);
			if (!address)
			{
				KPrintf("VirtualMemoryAllocator->FreePages(%d) Failed to Get Physical Address for Virtual Address(0x%x)!\n", pageCount, address);
				return false;
			}

			if (!g_KernelVMM->UnmapRange(address, pageCount))
			{
				KPrintf("VirtualMemoryAllocator->FreePages(%d) Failed to Unmap(0x%x)!\n", pageCount, address);
				return false;
			}

			g_KernelPMM->FreePages(physicalAddress, pageCount);
			m_Bitmap.ClearBits((AddressToPageNumber(address)), pageCount);
			return true;
		}

		void VirtualMemoryAllocator::ReservePage(VirtualAddress address)
		{
			ReservePages(address, 1);
		}

		void VirtualMemoryAllocator::ReservePages(VirtualAddress address, uint32_t pageCount)
		{
			m_Bitmap.SetBits(AddressToPageNumber(address), pageCount);
		}

		uint32_t VirtualMemoryAllocator::GetPageSize() const
		{
			return m_PageSize;
		}

		void VirtualMemoryAllocator::SetRegionState(VirtualAddress address, uint32_t sizeBytes, VirtualPageState state)
		{
			uint32_t pageNumber = AddressToPageNumber(address);
			uint32_t pageCount = sizeBytes / m_PageSize;
			if (state == VirtualPageState::Free)
				m_Bitmap.ClearBits(pageNumber, pageCount);
			else if (state == VirtualPageState::Reserved)
				m_Bitmap.SetBits(pageNumber, pageCount);
		}

		uint32_t VirtualMemoryAllocator::AddressToPageNumber(VirtualAddress address) const
		{
			return (address - m_StartAddress) / m_PageSize;
		}

		VirtualAddress VirtualMemoryAllocator::PageNumberToAddress(uint32_t pageNumber) const
		{
			return (m_StartAddress + (pageNumber * m_PageSize));
		}
	}
}
