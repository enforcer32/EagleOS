#pragma once

#include <Kernel/ESTD/String.h>
#include <Kernel/ESTD/StringUtil.h>

namespace Kernel
{
	namespace Storage
	{
		enum class ATADeviceCapabilities : uint16_t
		{
			DMA   = 1 << 0,
			SMART = 1 << 1,
			LBA   = 1 << 2,
			ATAPI = 1 << 3
		};

		enum class ATADeviceCommandSet : uint16_t
		{
			ATA = 1 << 0,
			ATAPI = 1 << 1,
			SMART = 1 << 2
		};

		struct ATADeviceGeometry
		{
			uint16_t Cylinders[2];
			uint16_t Heads[2];
			uint16_t Sectors[2];

			ESTD::String GetGeometryString() const
			{
				ESTD::String geometry = ESTD::String("Cylinders: ") + ESTD::StringUtil::ToString(Cylinders[0]) + ", " +
					"Heads: " + ESTD::StringUtil::ToString(Heads[0]) + ", " +
					"Sectors: " + ESTD::StringUtil::ToString(Sectors[0]);
				return geometry;
			}
		} __attribute__((packed));

		struct ATADeviceCapacity
		{
			uint32_t TotalSectors;
			uint16_t ExtendedCapacity;

			ESTD::String GetCapacityString() const
			{
				ESTD::String capacity = ESTD::String("TotalSectors: ") + ESTD::StringUtil::ToString(TotalSectors) + ", " +
					"ExtendedCapacity: " + ESTD::StringUtil::ToString(ExtendedCapacity);
				return capacity;
			}
		} __attribute__((packed));

		struct ATADeviceTransferModes
		{
			uint16_t DMAModes[2];
			uint16_t RecommendedPIO[2];

			ESTD::String GetTransferModesString() const
			{
				return ESTD::String("DMA Modes: ") + ESTD::StringUtil::ToString(DMAModes[0]) + ", " +
					"PIO Modes: " + ESTD::StringUtil::ToString(RecommendedPIO[0]);
			}
		} __attribute__((packed));

		struct ATADeviceLBAInfo
		{
			uint16_t LBASupport[2];
			uint16_t LBA48Support;

			ESTD::String GetLBAInfoString() const
			{
				return ESTD::String("LBA Support: ") + ESTD::String(LBASupport[0]) +
					", LBA48 Support: " + ESTD::String(LBA48Support);
			}
		} __attribute__((packed));

		struct ATADeviceInfo
		{
			uint16_t _Reserved[5];
			ATADeviceCapabilities Capabilities;
			uint16_t _ReservedFlags;
			uint16_t ModelNumber[20];
			uint8_t FirmwareRevision[16];
			uint16_t SerialNumber[10];
			ATADeviceGeometry Geometry;
			ATADeviceCapacity Capacity;
			ATADeviceTransferModes TransferModes;
			ATADeviceLBAInfo LBAInfo;
			ATADeviceCommandSet CommandSets;
			uint32_t SectorSize[2];

			ESTD::String GetCapabilitiesString() const
			{
				ESTD::String capabilities;
				if (static_cast<int>(Capabilities) & static_cast<int>(ATADeviceCapabilities::DMA))
					capabilities += "DMA ";
				if (static_cast<int>(Capabilities) & static_cast<int>(ATADeviceCapabilities::SMART))
					capabilities += "ATAPI ";
				if (static_cast<int>(Capabilities) & static_cast<int>(ATADeviceCapabilities::LBA))
					capabilities += "SMART ";
				if (static_cast<int>(Capabilities) & static_cast<int>(ATADeviceCapabilities::ATAPI))
					capabilities += "ATAPI ";
				return capabilities;
			}

			ESTD::String GetModelNumberString() const
			{ 
				ESTD::String modelStr;
				for (size_t i = 0; i < 20; i++)
				{
					char c1 = ModelNumber[i] & 0xFF;
					char c2 = (ModelNumber[i] >> 8) & 0xFF;
					modelStr += c1 ? c1 : c2;
					if (!c1 && !c2)
						break;
				}
				return modelStr;
			}

			ESTD::String GetFirmwareRevisionString() const
			{
				ESTD::String firmwareRevisionStr;
				for (size_t i = 0; i < 16; i++)
					firmwareRevisionStr += FirmwareRevision[i];
				return firmwareRevisionStr;
			}

			ESTD::String GetSerialNumberString() const 
			{ 
				ESTD::String serialNumberStr;
				for (size_t i = 0; i < 10; i++)
				{
					char c1 = SerialNumber[i] & 0xFF;
					char c2 = (SerialNumber[i] >> 8) & 0xFF;
					serialNumberStr += c1 ? c1 : c2;
					if (!c1 && !c2) break;
				}
				return serialNumberStr;
			}

			ESTD::String GetGeometryString() const 
			{ 
				return Geometry.GetGeometryString();
			}

			ESTD::String GetCapacityString() const 
			{ 
				return Capacity.GetCapacityString();
			}

			ESTD::String GetTransferModesString() const 
			{ 
				return TransferModes.GetTransferModesString(); 
			}

			ESTD::String GetLBAInfoString() const 
			{ 
				return LBAInfo.GetLBAInfoString(); 
			}

			ESTD::String GetCommandSetsString() const 
			{ 
				ESTD::String sets;
				if (static_cast<int>(CommandSets) & static_cast<int>(ATADeviceCommandSet::ATA))
					sets += "ATA ";
				if (static_cast<int>(CommandSets) & static_cast<int>(ATADeviceCommandSet::ATAPI))
					sets += "ATAPI ";
				if (static_cast<int>(CommandSets) & static_cast<int>(ATADeviceCommandSet::SMART))
					sets += "SMART ";
				return sets;
			}

			uint32_t GetSectorSizeString() const
			{
				return (static_cast<uint32_t>(SectorSize[1]) << 16) | SectorSize[0];
			}
		} __attribute__((packed));
	}
}
