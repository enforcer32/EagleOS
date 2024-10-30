#include <Axe/ATA/ATA.h>
#include <Axe/Memory/Malloc.h>

namespace Axe
{
	namespace ATA
	{
		void* ATAReadLBAOffset(ATADrive drive, uint32_t offset, uint32_t size)
		{
			uint32_t lba = (offset / ATA_SECTOR_SIZE);
			uint32_t lbaOffset = (offset % ATA_SECTOR_SIZE);
			uint32_t sectorcount = (size / ATA_SECTOR_SIZE) + 1;
			uint8_t* data = (uint8_t*)Malloc(sectorcount * ATA_SECTOR_SIZE);
			ATAReadLBA(data, ATA::ATADrive::Slave, sectorcount, lba);
			data += lbaOffset;
			return data;
		}
	}
}
