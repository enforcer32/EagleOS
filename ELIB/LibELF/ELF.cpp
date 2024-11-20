#include <ELIB/LibELF/ELF.h>
#include <ESTD/CString.h>
#include <ESTD/EMalloc.h>
#include <ESTD/Algorithm.h>

#include <Handshake/Stdio/Print.h>

namespace ELF
{
	bool ELF::Parse(ATA::ATADrive drive)
	{
		m_Drive = drive;
		m_Header = (ELF32Header*)ATA::ATAReadLBAOffset(drive, 0, sizeof(ELF32Header));

		if (!IsValidELF())
			return false;

		if (!ParseProgramHeaders())
			return false;
		
		if (!ParseSectionHeaders())
			return false;

		if (!ParseSectionHeaderStringTable())
			return false;

		if (!ParseProgramHeaderVirtualAddress())
			return false;

		return true;
	}

	bool ELF::LoadProgramHeadersToPhysicalMemory() const
	{
		for(uint32_t i = 0; i < m_Header->ProgramHeaderCount; i++)
		{
			const auto& programHeader = m_ProgramHeaders[i];
			void* segment = ATA::ATAReadLBAOffset(m_Drive, programHeader->Offset, programHeader->MemorySize);
			ESTD::Memcpy((void*)programHeader->VirtualAddress, segment, programHeader->MemorySize);
		}
		return true;
	}

	uint32_t ELF::GetEntryPointAddress() const
	{
		return m_Header->EntryPointAddress;
	}

	ELF32ProgramHeader** ELF::GetProgramHeaders() const
	{
		return m_ProgramHeaders;
	}

	uint32_t ELF::GetProgramHeaderCount() const
	{
		return m_Header->ProgramHeaderCount;
	}

	uint32_t ELF::GetVirtualAddressLow() const
	{
		return m_VirtualAddressLow;
	}
	
	uint32_t ELF::GetVirtualAddressHigh() const
	{
		return m_VirtualAddressHigh;
	}

	bool ELF::IsValidELF()
	{
		return m_Header->Ident.Magic == ELF_MAGIC_NUMBER;
	}

	bool ELF::ParseProgramHeaders()
	{
		m_ProgramHeaders = (ELF32ProgramHeader**)EMalloc(m_Header->ProgramHeaderCount * sizeof(ELF32ProgramHeader*));
		for (uint32_t i = 0; i < m_Header->ProgramHeaderCount; i++)
		{
			ELF32ProgramHeader* programHeader = (ELF32ProgramHeader*)ATA::ATAReadLBAOffset(m_Drive, ((m_Header->ProgramHeaderOffset) + (m_Header->ProgramHeaderEntrySize * i)), sizeof(ELF32ProgramHeader));
			*(m_ProgramHeaders + i) = programHeader;
		}
		return true;
	}

	bool ELF::ParseSectionHeaders()
	{
		m_SectionHeaders = (ELF32SectionHeader**)EMalloc(m_Header->SectionHeaderCount * sizeof(ELF32SectionHeader*));
		for (uint32_t i = 0; i < m_Header->SectionHeaderCount; i++)
		{
			ELF32SectionHeader* sectionHeader = (ELF32SectionHeader*)ATA::ATAReadLBAOffset(m_Drive, ((m_Header->SectionHeaderOffset) + (m_Header->SectionHeaderEntrySize * i)), sizeof(ELF32SectionHeader));
			*(m_SectionHeaders + i) = sectionHeader;
		}
		return true;
	}

	bool ELF::ParseSectionHeaderStringTable()
	{
		const auto& stringTableHeader = m_SectionHeaders[m_Header->SectionHeaderStringTableIndex];
		m_SectionHeaderStringTableRaw = (char*)ATA::ATAReadLBAOffset(m_Drive, stringTableHeader->Offset, stringTableHeader->Size);
		return true;
	}

	bool ELF::ParseProgramHeaderVirtualAddress()
	{
		uint32_t lowAddress = 0xFFFFFFFF;
		uint32_t highAddress = 0x0;
		uint32_t segmentStart, segmentEnd;

		for(size_t i = 0; i < m_Header->ProgramHeaderCount; i++)
		{
			ELF32ProgramHeader* phdr = m_ProgramHeaders[i];
			if (phdr->Type == ELFProgramHeaderType::Load)
			{
				segmentStart = phdr->VirtualAddress;
				segmentEnd = segmentStart + phdr->MemorySize;

				lowAddress = ESTD::Min(lowAddress, segmentStart);
				highAddress = ESTD::Max(highAddress, segmentEnd);
			}
		}

		m_VirtualAddressLow = lowAddress;
		m_VirtualAddressHigh = highAddress;
		return true;
	}
}
