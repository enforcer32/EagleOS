#pragma once

#include <Axe/ELF/Headers/ELFIdent.h>

#define ELF_MAGIC_NUMBER 0x464C457F

namespace Axe
{
	namespace ELF
	{
		enum class ELFHeaderFileType : uint16_t
		{
			None = 0x0,
			Relocatable = 0x1,
			Executable = 0x2,
			SharedObject = 0x3,
			Core = 0x4,
		};

		enum class ELFHeaderMachineType : uint16_t
		{
			X86 = 0x03,
			MIPS = 0x08,
			ARM = 0x28,
			AMD64 = 0x3E,
			ARMV8 = 0xB7,
			RISCV = 0xF3,
		};

		struct ELF32Header
		{
			ELFIdent Ident;
			ELFHeaderFileType Type;
			ELFHeaderMachineType Machine;
			uint32_t Version;
			uint32_t EntryPointAddress;
			uint32_t ProgramHeaderOffset;
			uint32_t SectionHeaderOffset;
			uint32_t Flags;
			uint16_t ELFHeaderSize;
			uint16_t ProgramHeaderEntrySize;
			uint16_t ProgramHeaderCount;
			uint16_t SectionHeaderEntrySize;
			uint16_t SectionHeaderCount;
			uint16_t SectionHeaderStringTableIndex;
		} __attribute__((packed));
	}
}
