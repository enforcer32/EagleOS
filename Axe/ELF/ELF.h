#pragma once

#include <Axe/ELF/Headers/ELFHeader.h>
#include <Axe/ELF/Headers/ELFProgramHeader.h>
#include <Axe/ELF/Headers/ELFSectionHeader.h>
#include <Axe/ATA/ATA.h>

namespace Axe
{
	namespace ELF
	{
		class ELF
		{
		public:
			bool Parse(ATA::ATADrive drive);
			bool LoadProgramHeaders();
			uint32_t GetEntryPointAddress();

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
