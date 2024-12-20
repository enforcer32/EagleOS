#pragma once

#include <Kernel/ESTD/Types.h>

namespace Kernel
{
	namespace Storage
	{
		enum class ATABus
		{
			Primary = 0x0,
			Secondary = 0x1,
		};

		enum class ATADrive
		{
			Master = 0x0,
			Slave = 0x0,
		};

		class ATA
		{
		public:
			bool Init(ATABus bus, ATADrive drive);

		private:
			bool InitBusDrive();
			bool BusHasDrives(ATABus bus);

		private:
			uint32_t m_SectorSize;
			ATABus m_Bus;
			ATADrive m_Drive;
			uint16_t m_BusPort;
		};
	}
}
