#include <Bootloader/System/SystemMemory.h>
#include <Bootloader/Memory/Malloc.h>
#include <Bootloader/x86/Memory.h>

namespace Bootloader
{
	SystemMemoryInfo* DetectSystemMemory()
	{
		SystemMemoryInfo* memoryInfo = (SystemMemoryInfo*)Malloc(sizeof(SystemMemoryInfo));
		SystemMemoryRegion* regions = (SystemMemoryRegion*)Malloc(256 * sizeof(SystemMemoryRegion));
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
