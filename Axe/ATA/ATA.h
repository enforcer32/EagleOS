#pragma once

#include <Axe/NXN/Types.h>

#define ATA_SECTOR_SIZE 512

namespace Axe
{
	namespace ATA
	{
		enum class ATADrive : uint8_t
		{
			Master = 0xE0,
			Slave = 0xF0
		};

		extern "C" int32_t ATAReadLBA(void* dst, ATADrive drive, uint32_t sectorcount, uint32_t lba);
		void* ATAReadLBAOffset(ATADrive drive, uint32_t offset, uint32_t size);
	}
}
