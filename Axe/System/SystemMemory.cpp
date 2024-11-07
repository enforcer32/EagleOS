#include <Axe/System/SystemMemory.h>
#include <Axe/Memory/BMalloc.h>
#include <Axe/x86/Memory.h>

namespace Axe
{
	SystemMemoryInfo* DetectSystemMemory()
	{
		SystemMemoryInfo* memoryInfo = (SystemMemoryInfo*)BMalloc(sizeof(SystemMemoryInfo));
		SystemMemoryRegion* regions = (SystemMemoryRegion*)BMalloc(256 * sizeof(SystemMemoryRegion));
		uint32_t regionIdx = 0;

		x86::BIOS::E820MemoryMap memoryMap;
		uint32_t continueID = 0;
		int result = x86::BIOS::E820QuerySystemAddressMap(&memoryMap, &continueID);
		while(result > 0 && continueID != 0)
		{
			regions[regionIdx].BaseAddress = memoryMap.BaseAddress;
			regions[regionIdx].Length = memoryMap.Length;
			regions[regionIdx].Type = static_cast<SystemMemoryRegionType>(memoryMap.Type);
			regions[regionIdx].ExtendedAttributes = memoryMap.ExtendedAttributes;
			regionIdx++;
			result = x86::BIOS::E820QuerySystemAddressMap(&memoryMap, &continueID);
		}

		memoryInfo->RegionCount = regionIdx;
		memoryInfo->Regions = regions;
		return memoryInfo;
	}
}
