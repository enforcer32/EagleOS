#pragma once

#include <stdint.h>

namespace Bootloader
{
	namespace ELF
	{
		enum class ELFProgramHeaderType : uint32_t
		{
			Null = 0x0,
			Load = 0x1,	
			Dynamic = 0x2,
			Interpreter = 0x3,
			Note = 0x4,
			SharedLib = 0x5,
			ProgramHeader = 0x6,
			TLS = 0x7,
		};

		enum class ELFProgramHeaderFlags : uint32_t
		{
			None = 0x0, // Access Denied
			Execute = 0x1, // PF_X
			Write = 0x2, // PF_W
			WriteExecute = 0x3, // PF_W+PF_X
			Read = 0x4, // PF_R
			ReadExecute = 0x5, // PF_R+PF_X	
			ReadWrite = 0x6, // PF_R+PF_W
			ReadWriteExecute = 0x7, // PF_R+PF_W+PF_X
		};

		struct ELF32ProgramHeader
		{
			ELFProgramHeaderType Type;
			uint32_t Offset;
			uint32_t VirtualAddress;
			uint32_t PhysicalAddress;
			uint32_t FileSize;
			uint32_t MemorySize;
			ELFProgramHeaderFlags Flags; // PF_X Executable, PF_W Writable, PF_R Readable
			uint32_t Alignment;
		} __attribute__((packed));
	}
}
