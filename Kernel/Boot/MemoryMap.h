#pragma once

#include <ESTD/Types.h>

namespace Boot
{
	enum class MemoryRegionType : uint32_t
	{
		Available = 0x1,
		Reserved = 0x2,
		ACPIReclaimable = 0x3,
		NVS = 0x4,
		BadRAM = 0x5,
	};

	struct MemoryRegion
	{
		uint32_t _Reserved;
		uint64_t BaseAddress;
		uint64_t Length;
		MemoryRegionType Type;
	} __attribute__((packed));

	struct MemoryMap
	{
		uint32_t MemoryLower;
		uint32_t MemoryUpper;
		uint32_t RegionCount;
		MemoryRegion* Regions;
	};
}
