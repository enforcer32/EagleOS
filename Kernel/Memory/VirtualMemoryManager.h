#pragma once

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
			int32_t Init();
			int32_t Map(VirtualAddress virtualAddress, PhysicalAddress physicalAddress);

		private:
			bool InitNullPage();
			bool IdentityMapKernel();
			bool IsAddressAligned(uintptr_t address, size_t alignment);

		private:
			PageDirectory* m_CurrentPageDirectory;
		};
	}

	extern Memory::VirtualMemoryManager* g_KernelVMM;
}
