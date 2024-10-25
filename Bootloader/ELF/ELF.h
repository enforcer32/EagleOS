#pragma once

#include <Bootloader/ELF/Headers/ELFHeader.h>
#include <Bootloader/ELF/Headers/ELFProgramHeader.h>
#include <Bootloader/ELF/Headers/ELFSectionHeader.h>
#include <Bootloader/ATA/ATA.h>

namespace Bootloader
{
	namespace ELF
	{
		class ELF
		{
		public:
			bool Parse(ATA::ATADrive drive);

		private:
			bool IsValidELF();
			bool ParseProgramHeaders();
			bool ParseSectionHeaders();
			bool ParseSectionHeaderStringTable();

		private:
			ATA::ATADrive m_Drive;
			ELF32Header* m_Header;
			ELF32ProgramHeader** m_ProgramHeaders;
			ELF32SectionHeader** m_SectionHeaders;
			char* m_SectionHeaderStringTableRaw;
		};
	}
}
