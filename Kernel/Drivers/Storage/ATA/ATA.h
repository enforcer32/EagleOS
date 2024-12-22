#pragma once

#include <Kernel/Drivers/Storage/ATA/ATADevice.h>

namespace Kernel
{
	namespace Storage
	{
		enum class ATABus : uint8_t
		{
			Primary = 0x0,
			Secondary = 0x1,
		};

		enum class ATADrive : uint8_t
		{
			Master = 0x0,
			Slave = 0x1,
		};

		class ATA
		{
		public:
			bool Init(ATABus bus, ATADrive drive);
			size_t GetSectorSize() const;
			int32_t Read28(void* buf, size_t sectorCount, size_t lba);
			int32_t Write28(const void* buf, size_t sectorCount, size_t lba);

		private:
			bool InitBusDrive();
			void Delay400NS();
			bool Poll();
			bool CacheFlush();
			int32_t ReadSector28(void* buf, size_t lba);
			int32_t WriteSector28(const void* buf, size_t lba);

		private:
			size_t m_SectorSize;
			ATABus m_Bus;
			ATADrive m_Drive;
			uint16_t m_BusPort;
			uint8_t m_DriveCode;
			ATADeviceInfo* m_DeviceInfo;
		};
	}
}
