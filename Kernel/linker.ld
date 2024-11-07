ENTRY(_kernelstart)
OUTPUT_FORMAT(elf32-i386)

SECTIONS 
{
	. = 1M; /* ORIGIN STARTS AT 1MB; Load Kernel at 1MB Memory (Address 1024 * 1024 = 0x00100000 ) */

	__kernel_physical_start = .;

	. = 0xC0000000;
	__kernel_virtual_start = .;
	.text ALIGN(4096) : AT ((ADDR (.text) - (0xC0000000)) + __kernel_physical_start)
	{
		*(.text)
		*(.text.*)
	}
 
	.rodata ALIGN(4096) : AT ((ADDR (.rodata) - (0xC0000000)) + __kernel_physical_start)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data ALIGN(4096) : AT ((ADDR (.data) - (0xC0000000)) + __kernel_physical_start)
	{
		*(.data)
		*(.data.*)
	}

	.bss ALIGN(4096) : AT ((ADDR (.bss) - (0xC0000000)) + __kernel_physical_start)
	{
		__kernel_bss_start = .;
		*(COMMON)
		*(.bss)
		*(.bss.*)
		__kernel_stack_start = .;
		. += 4096;
		__kernel_stack_end = .;
		__kernel_bss_end = .;
	}
	__kernel_virtual_end = .;
	__kernel_physical_end = __kernel_physical_start + (. - 0xC0000000);
}
