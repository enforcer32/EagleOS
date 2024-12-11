#pragma once

#include <Handshake/BootInfo.h>
#include <Kernel/Memory/PageDirectory.h>

namespace Kernel
{
	namespace Memory
	{
		class VirtualMemoryManager
		{
		public:
			bool Init(const Handshake::BootInfo* bootInfo);
			bool Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable);
			bool MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count);
			void EnablePaging();
			void DisablePaging();
			void ReloadPageDirectory();

		private:
			uint32_t m_VirtualStartAddress;
			PageDirectory m_PageDirectory __attribute__((aligned(4096)));
			PageTable m_PageTable0 __attribute__((aligned(4096)));
			PageTable m_PageTables768[256] __attribute__((aligned(4096))); // 1GB
		};
	}

	extern Memory::VirtualMemoryManager* g_KernelVMM;
}
