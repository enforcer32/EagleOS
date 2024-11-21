#pragma once

#include <Kernel/Memory/ZoneMemoryManager.h>

namespace Kernel
{
	namespace Memory
	{
		class PhysicalMemoryManager
		{
		public:
			bool Init(const Handshake::BootInfo* bootInfo);

		private:
			bool InitLowMemory();
			bool InitMemoryZones();

		private:
			friend class ZoneMemoryManager;

			uint32_t m_PageFrameSize;
			PhysicalAddress m_LowMemoryPhysicalStartAddress;
			VirtualAddress m_LowMemoryVirtualStartAddress;
			uint32_t m_MemorySizeBytes;
			uint32_t m_PageFrameCount;
			PageFrame* m_PageFrameMap;

			ZoneMemoryManager m_ZoneMM;
		};
	}

	extern Memory::PhysicalMemoryManager* g_KernelPMM;
}
