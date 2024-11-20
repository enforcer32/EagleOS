#pragma once

#include <ELIB/LibELF/Headers/ELFHeader.h>
#include <ELIB/LibELF/Headers/ELFProgramHeader.h>
#include <ELIB/LibELF/Headers/ELFSectionHeader.h>
#include <ELIB/LibATA/ATA.h>

namespace ELF
{
	class ELF
	{
	public:
		bool Parse(ATA::ATADrive drive);
		bool LoadProgramHeadersToVirtualMemory() const;
		uint32_t GetEntryPointAddress() const;
		ELF32ProgramHeader** GetProgramHeaders() const;
		uint32_t GetProgramHeaderCount() const;

		uint32_t GetVirtualAddressLow() const;
		uint32_t GetVirtualAddressHigh() const;

	private:
		bool IsValidELF();
		bool ParseProgramHeaders();
		bool ParseSectionHeaders();
		bool ParseSectionHeaderStringTable();
		bool ParseProgramHeaderVirtualAddress();

	private:
		ATA::ATADrive m_Drive;
		ELF32Header* m_Header;
		ELF32ProgramHeader** m_ProgramHeaders;
		ELF32SectionHeader** m_SectionHeaders;
		char* m_SectionHeaderStringTableRaw;
		uint32_t m_VirtualAddressLow, m_VirtualAddressHigh;
	};
}
