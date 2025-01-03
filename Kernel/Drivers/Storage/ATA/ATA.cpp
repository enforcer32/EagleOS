#include <Kernel/Drivers/Storage/ATA/ATA.h>
#include <Kernel/Arch/x86/IO.h>
#include <Kernel/ESTD/StdLib.h>
#include <Kernel/Kern/KPrintf.h>

/* ATA Bus Ports */
#define ATA_PRIMARY_BUS_IOPORT 0x1F0 // 0x1F0 - 0x1F7
#define ATA_SECONDARY_BUS_IOPORT 0x170 // 0x170 - 0x177

/* ATA Drives */
#define ATA_MASTER_DRIVE 0xA0
#define ATA_SLAVE_DRIVE 0xB0

/* ATA Registers - Offset from Bus Ports */
#define ATA_DATA_REG 0x0 // PIO data bytes (R/W)
#define ATA_ERROR_REG 0x1 // Retrieve Error Generated by Last Command (R)
#define ATA_FEATURES_REG 0x1 // Control Command Specific Features (W)
#define ATA_SECTOR_COUNT_REG 0x2 // Number of Sectors to Read/Write (0 Special) (R/W)
#define ATA_LBA_LOW_REG 0x3  // Partial Disk Sector Address (R/W)
#define ATA_LBA_MID_REG 0x4 // Partial Disk Sector Address (R/W)
#define ATA_LBA_HIGH_REG 0x5 // Partial Disk Sector Address (R/W)
#define ATA_DRIVEHEAD_SELECT_REG 0x6 // Select Drive/Head (R/W)
#define ATA_STATUS_REG 0x7 // Read Current Status (R)
#define ATA_COMMAND_REG 0x7 // Send ATA Commands to Device (W)

#define ATA_ALTSTATUS_REG 0xC // Alternate Status Register (Doesn't Affect Interrupts) (R)
#define ATA_DEVICE_CONTROL_REG 0xC // Reset Bus or Enable/Disable INterrupts (W)
#define ATA_DRIVE_ADDRESS_REG 0xD // Provide Drive/Head Select Info (R)

/* ATA Error Register */
#define ATA_ERROR_REG_AMNF 0x01 // Address Mark Not Found
#define ATA_ERROR_REG_TKZNF 0x02 // Track zero not found
#define ATA_ERROR_REG_ABRT 0x04 // Aborted command
#define ATA_ERROR_REG_MCR 0x08 // Media change request
#define ATA_ERROR_REG_IDNF 0x10 // ID not found
#define ATA_ERROR_REG_MC 0x20 // Media changed
#define ATA_ERROR_REG_UNC 0x40 // Uncorrectable data error
#define ATA_ERROR_REG_BBK 0x80 // Bad Block detected

/* ATA Drive/Head Select Register */
#define ATA_DRIVEHEAD_SELECT_REG_DRV 0x10 // Selects Drive Number
#define ATA_DRIVEHEAD_SELECT_REG_LBA 0x40 // If Set uses LBA, Otherwise CHS

/* ATA Status Register */
#define ATA_STATUS_REG_ERR 0x01 // Error Occursed, Send New Command to Clear it/(OR Software Reset)
#define ATA_STATUS_REG_IDX 0x02 // Index, Always 0
#define ATA_STATUS_REG_CORR 0x04 // Corrected data, Always 0
#define ATA_STATUS_REG_DRQ 0x08 // Set when the drive has PIO data to transfer, or is ready to accept PIO data
#define ATA_STATUS_REG_SRV 0x10 // Overlapped Mode Service Request
#define ATA_STATUS_REG_DF 0x20 // Drive Fault Error (does not set ERR)
#define ATA_STATUS_REG_DRDY 0x40 // Bit is clear when drive is spun down, or after an error. Set otherwise
#define ATA_STATUS_REG_BSY 0x80 // Indicates Drive is Preparing to send/receive data (wait for it to clear). In case of 'hang' (it never clears), do a software reset

/* ATA Device Control Register */
#define ATA_DEVICE_CONTROL_REG_nIEN 0x02 // Set this to stop Current Device from sending interrupts
#define ATA_DEVICE_CONTROL_REG_SRST 0x04 // Set, then clear (after 5us), this to do a "Software Reset" on all ATA drives on a bus, if one is misbehaving
#define ATA_DEVICE_CONTROL_REG_HOB 0x80 // Set this to read back the High Order Byte of the last LBA48 value sent to an IO port

/* ATA Device Address Register */
#define ATA_DRIVE_ADDRESS_REG_DS0 0x1 // Drive 0 select. Clears when drive 0 selected
#define ATA_DRIVE_ADDRESS_REG_DS1 0x2 // Drive 0 select. Clears when drive 0 selected
#define ATA_DRIVE_ADDRESS_REG_HS0 (0x3C >> 2) // One's compliment representation of the currently selected head
#define ATA_DRIVE_ADDRESS_REG_WTG 0x40 // Write gate; goes low while writing to the drive is in progress

/* ATA Commands */
#define ATA_COMMAND_IDENTIFY 0xEC
#define ATA_COMMAND_READ_PIO 0x20
#define ATA_COMMAND_WRITE_PIO 0x30
#define ATA_COMMAND_CACHE_FLUSH 0xE7

/* General ATA INFO */
#define ATA_SECTOR_SIZE 0x200

namespace Kernel
{
	namespace Storage
	{
		static const char* ATABusStr[]
		{
			"Primary",
			"Secondary",
		};

		static const char* ATADriveStr[]
		{
			"Master",
			"Slave",
		};

		bool ATA::Init(ATABus bus, ATADrive drive)
		{
			KPrintf("Initializing ATA Interface for Bus: %s, Drive: %s\n", ((bus == ATABus::Primary) ? "Primary" : "Secondary"), ((drive == ATADrive::Master) ? "Master" : "Slave"));
			m_SectorSize = ATA_SECTOR_SIZE;
			m_Bus = bus;
			m_Drive = drive;
			m_BusPort = (m_Bus == ATABus::Primary ? 0x1F0 : 0x170);
			m_DriveCode = (m_Drive == ATADrive::Master ? 0xA0 : 0xB0);

			if (!InitBusDrive())
			{
				KPrintf("ATA->Failed to Initialize Bus: %s, Drive: %s!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
				return false;
			}

			return true;
		}

		size_t ATA::GetSectorSize() const
		{
			return m_SectorSize;
		}

		int32_t ATA::Read28(void* buf, size_t sectorCount, size_t lba)
		{
			uint8_t* ptr = (uint8_t*)buf;
			for (size_t i = 0; i < sectorCount; i++)
			{
				if (ReadSector28(ptr, lba + i) != 0)
					return -1;
				ptr += m_SectorSize;
			}

			return sectorCount * m_SectorSize;
		}
		
		int32_t ATA::Write28(const void* buf, size_t sectorCount, size_t lba)
		{
			uint8_t* ptr = (uint8_t*)buf;
			for (size_t i = 0; i < sectorCount; i++)
			{
				if (WriteSector28(ptr, lba + i) != 0)
					return -1;
				ptr += m_SectorSize;
			}

			return sectorCount * m_SectorSize;
		}

		bool ATA::InitBusDrive()
		{
			// Select and Identify Drive
			x86::IO::OUTB(m_BusPort + ATA_DRIVEHEAD_SELECT_REG, m_DriveCode);
			x86::IO::OUTB(m_BusPort + ATA_SECTOR_COUNT_REG, 0);
			x86::IO::OUTB(m_BusPort + ATA_LBA_LOW_REG, 0);
			x86::IO::OUTB(m_BusPort + ATA_LBA_MID_REG, 0);
			x86::IO::OUTB(m_BusPort + ATA_LBA_HIGH_REG, 0);
			x86::IO::OUTB(m_BusPort + ATA_COMMAND_REG, ATA_COMMAND_IDENTIFY);

			Delay400NS();

			uint8_t status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
			if (!status)
			{
				KPrintf("ATA->InitBusDrive Failed, Bus: %s, Drive: %s Doesn't Exist!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
				return false;
			}

			while ((x86::IO::INSB(m_BusPort + ATA_STATUS_REG) & ATA_STATUS_REG_BSY) != 0)
			{
				if (x86::IO::INSB(m_BusPort + ATA_LBA_MID_REG) != 0 || x86::IO::INSB(m_BusPort + ATA_LBA_HIGH_REG) != 0)
				{
					KPrintf("ATA->InitBusDrive Failed, Bus: %s, Drive: %s is not ATA!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
					return false;
				}
			}

		retry:
			status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
			if (status & ATA_STATUS_REG_ERR)
			{
				KPrintf("ATA->InitBusDrive Failed, Bus: %s, Drive: %s Has Error!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
				return false;
			}

			while (!(status & ATA_STATUS_REG_DRQ))
				goto retry;

			// READ IDENTIFY
			uint16_t* buf = new uint16_t[256];
			for (size_t i = 0; i < 256; i++)
			{
				*buf = x86::IO::INSW(m_BusPort + ATA_DATA_REG);
				buf++;
			}
			m_DeviceInfo = (ATADeviceInfo*)buf;
			return true;
		}

		void ATA::Delay400NS()
		{
			for (size_t i = 0; i < 14; i++)
				x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
		}

		bool ATA::Poll()
		{
			Delay400NS();

		pool:
			uint8_t status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
			if (status & ATA_STATUS_REG_BSY)
				goto pool;

		retry:
			status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
			if (status & ATA_STATUS_REG_ERR)
			{
				KPrintf("ATA->Poll Failed, Bus: %s, Drive: %s Has Error!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
				return false;
			}

			if (!(status & ATA_STATUS_REG_DRQ))
				goto retry;
			
			return true;
		}

		bool ATA::CacheFlush()
		{
			x86::IO::OUTB(m_BusPort + ATA_COMMAND_REG, ATA_COMMAND_CACHE_FLUSH);

			uint8_t status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);
			while ((status & ATA_STATUS_REG_BSY) && !(status & ATA_STATUS_REG_DRQ))
				status = x86::IO::INSB(m_BusPort + ATA_STATUS_REG);

			if (status & ATA_STATUS_REG_ERR)
			{
				KPrintf("ATA->CacheFlush Failed, Bus: %s, Drive: %s Has Error!\n", ATABusStr[(uint8_t)m_Bus], ATADriveStr[(uint8_t)m_Drive]);
				return false;
			}

			return true;
		}

		int32_t ATA::ReadSector28(void* buf, size_t lba)
		{
			uint8_t seldrive = (m_Drive == ATADrive::Master ? 0xE0 : 0xF0);
			x86::IO::OUTB(m_BusPort + ATA_DRIVEHEAD_SELECT_REG, (seldrive | (uint8_t)(lba >> 24 & 0x0F)));
			x86::IO::OUTB(m_BusPort + 1, 0x0);
			x86::IO::OUTB(m_BusPort + ATA_SECTOR_COUNT_REG, 1);
			x86::IO::OUTB(m_BusPort + ATA_LBA_LOW_REG, (uint8_t)(lba));
			x86::IO::OUTB(m_BusPort + ATA_LBA_MID_REG, (uint8_t)(lba >> 8));
			x86::IO::OUTB(m_BusPort + ATA_LBA_HIGH_REG, (uint8_t)(lba >> 16));
			x86::IO::OUTB(m_BusPort + ATA_COMMAND_REG, ATA_COMMAND_READ_PIO);

			// Wait for Device to be Ready
			if (!Poll())
				return -1;

			uint16_t* ptr = (uint16_t*)buf;
			for (size_t i = 0; i < 256; i++)
			{
				*ptr = x86::IO::INSW(m_BusPort + ATA_DATA_REG);
				ptr++;
			}

			Delay400NS();
			return 0;
		}

		int32_t ATA::WriteSector28(const void* buf, size_t lba)
		{
			uint8_t seldrive = (m_Drive == ATADrive::Master ? 0xE0 : 0xF0);
			x86::IO::OUTB(m_BusPort + ATA_DRIVEHEAD_SELECT_REG, (seldrive | (uint8_t)(lba >> 24 & 0x0F)));
			x86::IO::OUTB(m_BusPort + 1, 0x0);
			x86::IO::OUTB(m_BusPort + ATA_SECTOR_COUNT_REG, 1);
			x86::IO::OUTB(m_BusPort + ATA_LBA_LOW_REG, (uint8_t)(lba));
			x86::IO::OUTB(m_BusPort + ATA_LBA_MID_REG, (uint8_t)(lba >> 8));
			x86::IO::OUTB(m_BusPort + ATA_LBA_HIGH_REG, (uint8_t)(lba >> 16));
			x86::IO::OUTB(m_BusPort + ATA_COMMAND_REG, ATA_COMMAND_WRITE_PIO);

			// Wait for Device to be Ready
			if (!Poll())
				return -1;

			uint16_t* ptr = (uint16_t*)buf;
			for (size_t i = 0; i < 256; i++)
			{
				x86::IO::OUTW(m_BusPort + ATA_DATA_REG, *ptr);
				ptr++;
			}

			// Cache Flush after Write
			if (!CacheFlush())
				return -1;
			Delay400NS();
			return 0;
		}
	}
}
