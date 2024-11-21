#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Kern/KPrintf.h>

namespace Kernel
{
	Memory::PhysicalMemoryManager* g_KernelPMM = nullptr;

	namespace Memory
	{
		bool PhysicalMemoryManager::Init(const Handshake::BootInfo* bootInfo)
		{
			m_PageFrameSize = 4096;
			m_LowMemoryPhysicalStartAddress = bootInfo->LowMemoryPhysicalStartAddress;
			m_LowMemoryVirtualStartAddress = bootInfo->LowMemoryVirtualStartAddress;
			m_MemorySizeBytes = bootInfo->HighMemoryPhysicalEndAddress;
			m_PageFrameCount = m_MemorySizeBytes / m_PageFrameSize;

			if (!InitLowMemory())
			{
				KPrintf("PhysicalMemoryManager Failed to InitLowMemory\n");
				return false;
			}

			if(!m_ZoneMM.Init(this, bootInfo))
			{
				KPrintf("PhysicalMemoryManager Failed to Init ZoneMemoryManager\n");
				return false;
			}

			KPrintf("PhysicalMemoryManager{PageFrameSize = %d, PageFrameCount = %d, MemorySize = %dMB}\n", m_PageFrameSize, m_PageFrameCount, (m_MemorySizeBytes / 1048576));
			return true;
		}

		bool PhysicalMemoryManager::InitLowMemory()
		{
			// Initialize Memory for Page Structures
			m_PageFrameMap = (PageFrame*)m_LowMemoryVirtualStartAddress;
			for(uint32_t i = 0; i < m_PageFrameCount; i++)
				m_PageFrameMap[i].State = PageFrameState::Free;

			m_LowMemoryPhysicalStartAddress += (sizeof(PageFrame) * m_PageFrameCount);
			m_LowMemoryVirtualStartAddress += (sizeof(PageFrame) * m_PageFrameCount);

			return true;
		}
	}
}
