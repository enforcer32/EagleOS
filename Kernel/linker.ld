ENTRY(_kernelstart)
OUTPUT_FORMAT(elf32-i386)

SECTIONS 
{
	. = 0xC0000000;
	__kernel_virtual_start = .;
	.text ALIGN(4096) : AT ((ADDR (.text) - (0xC0000000)))
	{
		*(.text)
		*(.text.*)
	}
 
	.rodata ALIGN(4096) : AT ((ADDR (.rodata) - (0xC0000000)))
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data ALIGN(4096) : AT ((ADDR (.data) - (0xC0000000)))
	{
		*(.data)
		*(.data.*)
	}

	.bss ALIGN(4096) : AT ((ADDR (.bss) - (0xC0000000)))
	{
		__kernel_bss_start = .;
		*(COMMON)
		*(.bss)
		*(.bss.*)
		__kernel_bss_end = .;
	}
	__kernel_virtual_end = .;
}
