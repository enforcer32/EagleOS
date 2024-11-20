#pragma once

//#include <Axe/BootInfo.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PageDirectory.h>

namespace Kernel
{
	namespace Memory
	{
		class VirtualMemoryManager
		{
		public:
			//int32_t Init(const Axe::BootInfo* bootInfo);
			int32_t Map(VirtualAddress virtualAddress, PhysicalAddress physicalAddress);

		private:
			bool IsAddressAligned(uintptr_t address, size_t alignment);

		private:
			PageDirectory* m_CurrentPageDirectory;
		};
	}

	extern Memory::VirtualMemoryManager* g_KernelVMM;
}
