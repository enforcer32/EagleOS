#pragma once

#include <ESTD/Types.h>

namespace Handshake
{
	namespace System
	{
		enum class MemoryRegionType : uint32_t
		{
			Usable = 0x1,
			Reserved = 0x2,
			ACPIReclaim = 0x3,
			ACPINVS = 0x4,
			BadMemory = 0x5,
		};

		struct MemoryRegion
		{
			uint64_t BaseAddress;
			uint64_t Length;
			MemoryRegionType Type;
			uint32_t ExtendedAttributes;
		};

		struct MemoryMap
		{
			uint32_t RegionCount;
			MemoryRegion* Regions;
		};

		MemoryMap* DetectMemoryMap();
	}
}
