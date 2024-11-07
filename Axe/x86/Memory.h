#pragma once

#include <Axe/NXN/Types.h>

namespace Axe
{
	namespace x86
	{
		namespace BIOS
		{
			enum class E820MemoryMapType : uint32_t
			{
				Usable = 0x1,
				Reserved = 0x2,
				ACPIReclaim = 0x3,
				ACPINVS = 0x4,
				BadMemory = 0x5,
			};

			struct E820MemoryMap
			{
				uint64_t BaseAddress;
				uint64_t Length;
				E820MemoryMapType Type;
				uint32_t ExtendedAttributes;
			};

			extern "C" int32_t E820QuerySystemAddressMap(E820MemoryMap* memoryMap, uint32_t* continueID);
		}
	}
}
