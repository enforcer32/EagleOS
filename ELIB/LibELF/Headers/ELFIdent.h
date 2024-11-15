#pragma once

#include <ESTD/Types.h>

namespace ELF
{
	// ELF Architecture
	enum class ELFClassType : uint8_t
	{
		None = 0x0, // Invalid
		ELF32 = 0x1, // 32-Bit
		ELF64 = 0x2, // 64-Bit
	};

	// ELF Endianness
	enum class ELFDataType : uint8_t
	{
		None = 0x0, // Invalid
		LSB = 0x1, // Little Endian
		MSB = 0x2, // Big Endian
	};

	// ELF OS ABI
	enum class ELFOSABIType : uint8_t
	{
		SystemV = 0x0,
		HPUX = 0x1,
		NetBSD = 0x2,
		Linux = 0x3,
	};

	// ELF Ident
	struct ELFIdent
	{
		uint32_t Magic; // Magic Number: 0x7F E L F (0x7F454C46)
		ELFClassType Class; // Architecture
		ELFDataType Data; // Endianness
		uint8_t Version; // ELF Version: 1
		ELFOSABIType OSABI;
		uint8_t ABIVersion;
		uint8_t _PADDING[7];
	} __attribute__((packed));;
}
