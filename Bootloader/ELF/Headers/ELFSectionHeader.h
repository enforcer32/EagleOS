#pragma once

#include <stdint.h>

namespace Bootloader
{
	namespace ELF
	{
		struct ELFSymbolTable
		{
			uint32_t Name;
			uint32_t Value; // 64bit also
			uint32_t Size;
			uint8_t Info;
			uint8_t Other;
			uint16_t SectionHeaderIndex;
		} __attribute__((packed));

		enum class ELFSectionHeaderType : uint32_t
		{
			Null = 0x0,
			PROGBITS = 0x1,
			SymbolTable = 0x2,
			StringTable = 0x3,
			RelocationA = 0x4,
			Hash = 0x5,
			Dynamic = 0x6,
			Note = 0x7,
			NOBITS = 0x8,
			Relocation = 0x9,
			SharedLib = 0x10,
			DynamicSymbolTable = 0x11,
		};

		enum class ELFSectionHeaderFlags : uint32_t
		{
			Write = 0x1,
			Alloc = 0x2,
			ExecInstruction = 0x4,
			Merge = 0x10,
			Strings = 0x20,
			InfoLink = 0x40,
			LinkOrder = 0x80,
			OSNonConfirming = 0x100,
			Group = 0x200,
			MaskOS = 0x0ff00000,
			Ordered = 0x40000000,
			Exclude = 0x80000000,
			MaskProc = 0xf0000000,
		};

		struct ELF32SectionHeader
		{
			uint32_t Name; // Offset for SectionHeaderStringTableIndex
			ELFSectionHeaderType Type;
			ELFSectionHeaderFlags Flags;
			uint32_t Address;
			uint32_t Offset;
			uint32_t Size;
			uint32_t Link;
			uint32_t Info;
			uint32_t AddressAlignment;
			uint32_t EntrySize;
		} __attribute__((packed));
	}
}
