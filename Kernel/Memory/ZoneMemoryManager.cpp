#include <Kernel/Memory/ZoneMemoryManager.h>
#include <Kernel/Memory/PhysicalMemoryManager.h>
#include <Kernel/Kern/KPrintf.h>
#include <ESTD/Algorithm.h>
#include <ESTD/CString.h>

namespace Kernel
{
	namespace Memory
	{
		bool ZoneMemoryManager::Init(const PhysicalMemoryManager* pmm, const Handshake::BootInfo* bootInfo)
		{
			ESTD::Memset(m_Zones, 0, sizeof(m_Zones));
			m_PageFrameSize = pmm->m_PageFrameSize;

			m_NormalZonePhysicalStartAddress = ESTD::AlignUp(pmm->m_LowMemoryPhysicalStartAddress, m_PageFrameSize);
			m_NormalZonePhysicalEndAddress = ESTD::AlignDown(bootInfo->LowMemoryPhysicalEndAddress, m_PageFrameSize);
			m_NormalZoneSize = ESTD::AlignDown((m_NormalZonePhysicalEndAddress - m_NormalZonePhysicalStartAddress), m_PageFrameSize);
			if (!InitMemoryZone(MemoryZoneType::Normal, pmm->m_PageFrameMap, m_NormalZonePhysicalStartAddress, m_NormalZonePhysicalEndAddress))
			{
				KPrintf("ZoneMemoryManager: Failed to InitZone->Normal\n");
				return false;
			}
			
			m_HighZonePhysicalStartAddress = ESTD::AlignUp(bootInfo->HighMemoryPhysicalStartAddress, m_PageFrameSize);
			m_HighZonePhysicalEndAddress = ESTD::AlignDown(bootInfo->HighMemoryPhysicalEndAddress, m_PageFrameSize);
			m_HighZoneSize = ESTD::AlignDown((m_HighZonePhysicalEndAddress - m_HighZonePhysicalStartAddress), m_PageFrameSize);
			if (!InitMemoryZone(MemoryZoneType::High, pmm->m_PageFrameMap, m_HighZonePhysicalStartAddress, m_HighZonePhysicalEndAddress))
			{
				KPrintf("ZoneMemoryManager: Failed to InitZone->High\n");
				return false;
			}

			KPrintf("ZoneMemoryManager->Normal{ZoneType = %d, PageFrameCount = %d, FreePageFrameCount = %d, FirstPageFrameNumber = %d, PageFrameMap = 0x%x}\n", m_Zones[(uint8_t)MemoryZoneType::Normal].ZoneType, m_Zones[(uint8_t)MemoryZoneType::Normal].PageFrameCount, m_Zones[(uint8_t)MemoryZoneType::Normal].FreePageFrameCount, m_Zones[(uint8_t)MemoryZoneType::Normal].FirstPageFrameNumber, m_Zones[(uint8_t)MemoryZoneType::Normal].PageFrameMap);
			KPrintf("ZoneMemoryManager->High{ZoneType = %d, PageFrameCount = %d, FreePageFrameCount = %d, FirstPageFrameNumber = %d, PageFrameMap = 0x%x}\n", m_Zones[(uint8_t)MemoryZoneType::High].ZoneType, m_Zones[(uint8_t)MemoryZoneType::High].PageFrameCount, m_Zones[(uint8_t)MemoryZoneType::High].FreePageFrameCount, m_Zones[(uint8_t)MemoryZoneType::High].FirstPageFrameNumber, m_Zones[(uint8_t)MemoryZoneType::High].PageFrameMap);
			return true;
		}

		bool ZoneMemoryManager::InitMemoryZone(MemoryZoneType zoneType, PageFrame* pageFrameMap, PhysicalAddress startAddress, PhysicalAddress endAddress)
		{
			KPrintf("Initializing Memory Zone: 0x%x -> 0x%x\n", startAddress, endAddress);

			if (startAddress >= endAddress)
			{
				KPrintf("ZoneMemoryManager->InitMemoryZone(%d): Invalid Addresses\n", (uint8_t)zoneType);
				return false;
			}

			if (!ESTD::IsAligned(startAddress, m_PageFrameSize) || !ESTD::IsAligned(endAddress, m_PageFrameSize))
			{
				KPrintf("ZoneMemoryManager->InitMemoryZone(%d): Addresses not Aligned\n", (uint8_t)zoneType);
				return false;
			}

			uint32_t zonePageFrameCount = (endAddress - startAddress) / m_PageFrameSize;
			uint32_t zoneFirstPageFrame = (startAddress / m_PageFrameSize);

			MemoryZone* zone = &m_Zones[(uint8_t)zoneType];
			zone->ZoneType = zoneType;
			zone->PageFrameCount = zonePageFrameCount;
			zone->FreePageFrameCount = zonePageFrameCount;
			zone->FirstPageFrameNumber = zoneFirstPageFrame;
			zone->PageFrameMap = pageFrameMap + zoneFirstPageFrame;

			ESTD::Memset(zone->PageFrameMap, 0, zone->PageFrameCount * sizeof(PageFrame));
			return true;
		}
	}
}
