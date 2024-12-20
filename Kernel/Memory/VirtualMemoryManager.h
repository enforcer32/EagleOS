#pragma once

#include <Kernel/Boot/BootInfo.h>
#include <Kernel/Memory/PageDirectory.h>

namespace Kernel
{
	namespace Memory
	{
		class VirtualMemoryManager
		{
		public:
			bool Init(const Boot::BootInfo* bootInfo);
			bool Map(VirtualAddress virtualAddress, PhysicalAddress physicalAddress, bool writable);
			bool MapRange(VirtualAddress virtualAddress, PhysicalAddress physicalAddress, bool writable, size_t count);
			bool Unmap(VirtualAddress virtualAddress);
			bool UnmapRange(VirtualAddress virtualAddress, size_t count);
			void FlushTLB(VirtualAddress virtualAddress);
			void FlushTLBRange(VirtualAddress virtualAddress, size_t count);
			void EnablePaging();
			void DisablePaging();
			void ReloadPageDirectory();
			PhysicalAddress GetPhysicalAddress(VirtualAddress virtualAddress);

		private:
			VirtualAddress m_VirtualStartAddress;
			PageDirectory m_PageDirectory __attribute__((aligned(4096)));
			PageTable m_PageTable0 __attribute__((aligned(4096)));
			PageTable m_PageTables768[256] __attribute__((aligned(4096))); // 1GB
		};
	}

	extern Memory::VirtualMemoryManager* g_KernelVMM;
}
