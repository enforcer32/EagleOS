#pragma once

#include <stdint.h>

namespace Axe
{
	enum class SystemMemoryRegionType : uint32_t
	{
		Usable = 0x1,
		Reserved = 0x2,
		ACPIReclaim = 0x3,
		ACPINVS = 0x4,
		BadMemory = 0x5,
	};

	struct SystemMemoryRegion
	{
		uint64_t BaseAddress;
		uint64_t Length;
		SystemMemoryRegionType Type;
		uint32_t ExtendedAttributes;
	};

	struct SystemMemoryInfo
	{
		uint32_t RegionCount;
		SystemMemoryRegion* Regions;
	};

	SystemMemoryInfo* DetectSystemMemory();
}
