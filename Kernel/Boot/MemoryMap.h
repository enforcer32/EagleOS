#pragma once

#include <Kernel/ESTD/Types.h>

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
		uintptr_t MemoryAddressLow;
		uintptr_t MemoryAddressHigh;
		size_t RegionCount;
		MemoryRegion* Regions;
	};
}
