#pragma once

#include <Kernel/ESTD/Types.h>

#define MULTIBOOT_SEARCH 8192
#define MULTIBOOT_HEADER_ALIGN 4
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_MOD_ALIGN 0x00001000
#define MULTIBOOT_INFO_ALIGN 0x00000004
#define MULTIBOOT_PAGE_ALIGN 0x00000001
#define MULTIBOOT_MEMORY_INFO 0x00000002
#define MULTIBOOT_VIDEO_MODE 0x00000004
#define MULTIBOOT_AOUT_KLUDGE 0x00010000
#define MULTIBOOT_INFO_MEMORY 0x00000001
#define MULTIBOOT_INFO_BOOTDEV 0x00000002
#define MULTIBOOT_INFO_CMDLINE 0x00000004
#define MULTIBOOT_INFO_MODS 0x00000008
#define MULTIBOOT_INFO_AOUT_SYMS 0x00000010
#define MULTIBOOT_INFO_ELF_SHDR 0X00000020
#define MULTIBOOT_INFO_MEMORY_MAP 0x00000040
#define MULTIBOOT_INFO_DRIVE_INFO 0x00000080
#define MULTIBOOT_INFO_CONFIG_TABLE 0x00000100
#define MULTIBOOT_INFO_BOOT_LOADER_NAME 0x00000200
#define MULTIBOOT_INFO_APM_TABLE 0x00000400
#define MULTIBOOT_INFO_VBE_INFO 0x00000800
#define MULTIBOOT_INFO_FRAMEBUFFER_INFO 0x00001000

namespace Boot
{
	struct MultibootSymbolTable
	{
		uint32_t TabSize;
		uint32_t StringSize;
		uint32_t Address;
		uint32_t _Reserved;
	} __attribute__((packed));

	struct MultibootELFSectionHeaderTable
	{
		uint32_t Number;
		uint32_t Size;
		uint32_t Address;
		uint32_t Shndx;
	} __attribute__((packed));

	struct MultibootModule
	{
		uint32_t StartAddress;
		uint32_t EndAddress;
		uint32_t CommandLine;
		uint32_t _Pad;
	} __attribute__((packed));

	enum class MultibootMemoryMapType : uint32_t
	{
		Available = 0x1,
		Reserved = 0x2,
		ACPIReclaimable = 0x3,
		NVS = 0x4,
		BADRAM = 0x5,
	};

	struct MultibootMemoryMap
	{
		uint32_t Size;
		uint64_t BaseAddress;
		uint64_t Length;
		MultibootMemoryMapType Type;
	} __attribute__((packed));

	enum class MultibootFramebufferType : uint8_t
	{
		Indexed = 0x0,
		RGB = 0x1,
		EGAText = 0x2,
	};

	struct MultibootInfo
	{
		/* Multiboot info version number */
		uint32_t Flags;

		/* Available memory from BIOS */
		uint32_t MemoryLower;
		uint32_t MemoryUpper;

		 /* "root" partition */
		uint32_t BootDevice;

		/* Kernel command line */
		uint32_t CommandLine;

		/* Boot-Module list */
		uint32_t ModCount;
		uint32_t ModAddress;

		union
		{
			MultibootSymbolTable SymbolTable;
			MultibootELFSectionHeaderTable ELFHeaderTable;
		} Sym;

		/* Memory Mapping buffer */
		uint32_t MemoryMapLength;
		MultibootMemoryMap* MemoryMapAddress;

		/* Drive Info buffer */
		uint32_t DrivesLength;
		uint32_t DrivesAddress;

		/* ROM configuration table */
		uint32_t ConfigTable;

		/* Boot Loader Name */
		uint32_t BootloaderName;

		/* APM table */
		uint32_t APMTable;

		/* Video */
		uint32_t VBEControlInfo;
		uint32_t VBEModeInfo;
		uint16_t VBEMode;
		uint16_t VBEInterfaceSegment;
		uint16_t VBEInterfaceOffset;
		uint16_t VBEInterfaceLength;
		uint64_t FramebufferAddress;
		uint32_t FramebufferPitch;
		uint32_t FramebufferWidth;
		uint32_t FramebufferHeight;
		uint8_t FramebufferBPP;
		MultibootFramebufferType FramebufferType;
		union
		{
			struct
			{
				uint32_t FramebufferPaletteAddress;
				uint16_t FramebufferPaletteNumColors;
			} __attribute__((packed));
			struct
			{
				uint8_t FramebufferRedFieldPosition;
				uint8_t FramebufferRedMaskSize;
				uint8_t FramebufferGreenFieldPosition;
				uint8_t FramebufferGreenMaskSize;
				uint8_t FramebufferBlueFieldPosition;
				uint8_t FramebufferBlueMaskSize;
			} __attribute__((packed));
		};
	} __attribute__((packed));

	void MultibootDumpMemoryMap(const MultibootInfo* multibootInfo);
}
