#include <Axe/ELF/ELF.h>
#include <Axe/Memory/Malloc.h>

namespace Axe
{
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

			return true;
		}

		bool ELF::LoadProgramHeadersToPhysicalMemory() const
		{
			for(uint32_t i = 0; i < m_Header->ProgramHeaderCount; i++)
			{
				const auto& programHeader = m_ProgramHeaders[i];
				void* segment = ATA::ATAReadLBAOffset(m_Drive, programHeader->Offset, programHeader->MemorySize);
				Memcpy((void*)programHeader->PhysicalAddress, segment, programHeader->MemorySize);
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

		bool ELF::IsValidELF()
		{
			return m_Header->Ident.Magic == ELF_MAGIC_NUMBER;
		}

		bool ELF::ParseProgramHeaders()
		{
			m_ProgramHeaders = (ELF32ProgramHeader**)Malloc(m_Header->ProgramHeaderCount * sizeof(ELF32ProgramHeader*));
			for (uint32_t i = 0; i < m_Header->ProgramHeaderCount; i++)
			{
				ELF32ProgramHeader* programHeader = (ELF32ProgramHeader*)ATA::ATAReadLBAOffset(m_Drive, ((m_Header->ProgramHeaderOffset) + (m_Header->ProgramHeaderEntrySize * i)), sizeof(ELF32ProgramHeader));
				*(m_ProgramHeaders + i) = programHeader;
			}
			return true;
		}

		bool ELF::ParseSectionHeaders()
		{
			m_SectionHeaders = (ELF32SectionHeader**)Malloc(m_Header->SectionHeaderCount * sizeof(ELF32SectionHeader*));
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
	}
}
