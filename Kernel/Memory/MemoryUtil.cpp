#include <Kernel/Memory/MemoryUtil.h>

namespace Kernel
{
	namespace Memory
	{
		uint32_t MemoryUtil::s_KernelVirtualStartAddress = 0;

		bool MemoryUtil::Init(const Handshake::BootInfo* bootInfo)
		{
			s_KernelVirtualStartAddress = bootInfo->KernelVirtualStartAddress;
			return true;
		}

		PhysicalAddress MemoryUtil::KernelVirtualToPhysicalAddress(VirtualAddress virtualAddress)
		{
			return (virtualAddress - s_KernelVirtualStartAddress);
		}

		VirtualAddress MemoryUtil::KernelPhysicalToVirtualAddress(PhysicalAddress physicalAddress)
		{
			return (physicalAddress + s_KernelVirtualStartAddress);
		}
	}
}
