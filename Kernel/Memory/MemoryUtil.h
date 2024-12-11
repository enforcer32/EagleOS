#pragma once

#include <Handshake/BootInfo.h>
#include <Kernel/Memory/VirtualAddress.h>
#include <Kernel/Memory/PhysicalAddress.h>

namespace Kernel
{
	namespace Memory
	{
		class MemoryUtil
		{
		public:
			static bool Init(const Handshake::BootInfo* bootInfo);
			static PhysicalAddress KernelVirtualToPhysicalAddress(VirtualAddress virtualAddress);
			static VirtualAddress KernelPhysicalToVirtualAddress(PhysicalAddress physicalAddress);

		private:
			static VirtualAddress s_KernelVirtualStartAddress;
			static PhysicalAddress s_KernelPhysicalStartAddress;
		};
	}
}
