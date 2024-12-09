#pragma once

#include <Handshake/BootInfo.h>
#include <Kernel/Memory/PageDirectory.h>

namespace Kernel
{
	namespace Memory
	{
		class PageManager
		{
		public:
			bool Init(const Handshake::BootInfo* bootInfo);
			bool Map(uint32_t virtualAddress, uint32_t physicalAddress, bool writable);
			bool MapRange(uint32_t virtualAddress, uint32_t physicalAddress, bool writable, uint32_t count);
			void EnablePaging();
			void DisablePaging();
			void ReloadPageDirectory();

		private:
			PageDirectory m_PageDirectory __attribute__((aligned(4096)));
			PageTable m_PageTable0 __attribute__((aligned(4096))); // DMA 4mb
			PageTable m_PageTable768 __attribute__((aligned(4096))); // Kernel 4mb
		};
	}

	extern Memory::PageManager* g_KernelPageManager;
}
