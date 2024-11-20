#include <Handshake/System/MemoryMap.h>
#include <Handshake/BIOS/Memory/E820Memory.h>
#include <Handshake/Memory/Malloc.h>

namespace Handshake
{
	namespace System
	{
		MemoryMap* DetectMemoryMap()
		{
			MemoryMap* systemMemoryMap = (MemoryMap*)Malloc(sizeof(MemoryMap));
			MemoryRegion* regions = (MemoryRegion*)Malloc(256 * sizeof(MemoryRegion));
			uint32_t regionIdx = 0;

			BIOS::E820MemoryMap e820MemoryMap;
			uint32_t continueID = 0;
			int result = BIOS::E820QueryMemoryMap(&e820MemoryMap, &continueID);
			while(result > 0 && continueID != 0)
			{
				regions[regionIdx].BaseAddress = e820MemoryMap.BaseAddress;
				regions[regionIdx].Length = e820MemoryMap.Length;
				regions[regionIdx].Type = static_cast<MemoryRegionType>(e820MemoryMap.Type);
				regions[regionIdx].ExtendedAttributes = e820MemoryMap.ExtendedAttributes;
				regionIdx++;
				result = BIOS::E820QueryMemoryMap(&e820MemoryMap, &continueID);
			}

			systemMemoryMap->RegionCount = regionIdx;
			systemMemoryMap->Regions = regions;
			return systemMemoryMap;
		}
	}
}
