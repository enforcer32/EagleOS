#pragma once

#include <Handshake/BootInfo.h>
#include <Kernel/Memory/PhysicalAddress.h>
#include <Kernel/Memory/VirtualAddress.h>

namespace Kernel
{
	namespace Memory
	{
		class PhysicalMemoryManager;

		enum class MemoryZoneType : uint8_t
		{
			DMA = 0x0,
			Normal = 0x1,
			High = 0x2,
			_MAX_SIZE
		};

		enum class PageFrameState : uint8_t
		{
			Free = 0x0,
			Used = 0x1,
		};

		struct PageFrame
		{
			PageFrameState State;
			MemoryZoneType ZoneType;
		};

		struct MemoryZone
		{
			MemoryZoneType ZoneType;
			uint32_t PageFrameCount;
			uint32_t FreePageFrameCount;
			uint32_t FirstPageFrameNumber;
			PageFrame* PageFrameMap;
		};

		class ZoneMemoryManager
		{
		public:
			bool Init(const PhysicalMemoryManager* pmm, const Handshake::BootInfo* bootInfo);

		private:
			bool InitMemoryZone(MemoryZoneType zoneType, PageFrame* pageFrameMap, PhysicalAddress startAddress, PhysicalAddress endAddress);
			bool InitNormalZone(const PhysicalMemoryManager* pmm, const Handshake::BootInfo* bootInfo);

		private:
			MemoryZone m_Zones[(uint8_t)MemoryZoneType::_MAX_SIZE];
			uint32_t m_PageFrameSize;

			PhysicalAddress m_NormalZonePhysicalStartAddress;
			PhysicalAddress m_NormalZonePhysicalEndAddress;
			uint32_t m_NormalZoneSize;

			PhysicalAddress m_HighZonePhysicalStartAddress;
			PhysicalAddress m_HighZonePhysicalEndAddress;
			uint32_t m_HighZoneSize;
		};
	}
}
