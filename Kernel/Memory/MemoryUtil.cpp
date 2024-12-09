#include <Kernel/Memory/MemoryUtil.h>
#include <Kernel/Kern/KPrintf.h>

namespace Kernel
{
	namespace Memory
	{
		uint32_t MemoryUtil::s_KernelVirtualStartAddress = 0;
		uint32_t MemoryUtil::s_KernelPhysicalStartAddress = 0;

		bool MemoryUtil::Init(const Handshake::BootInfo* bootInfo)
		{
			s_KernelVirtualStartAddress = bootInfo->KernelVirtualStartAddress;
			s_KernelPhysicalStartAddress = bootInfo->KernelPhysicalStartAddress;
			return true;
		}

		PhysicalAddress MemoryUtil::KernelVirtualToPhysicalAddress(VirtualAddress virtualAddress)
		{
			return ((virtualAddress - s_KernelVirtualStartAddress) + s_KernelPhysicalStartAddress);
		}

		VirtualAddress MemoryUtil::KernelPhysicalToVirtualAddress(PhysicalAddress physicalAddress)
		{
			return ((physicalAddress + s_KernelVirtualStartAddress) - s_KernelPhysicalStartAddress);
		}
	}
}
