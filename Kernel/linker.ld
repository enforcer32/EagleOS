ENTRY(_kernelstart)
OUTPUT_FORMAT(elf32-i386)

SECTIONS 
{
	. = 0x100000;
	_bootloader_physical_start = .;
	.boot.data ALIGN(4096) :
	{
		*(.multiboot)
		*(.boot.data)
	}

	.boot.text ALIGN(4096) :
	{
		*(.boot.text)
	}

	.boot.bss ALIGN(4096) :
	{
		*(.boot.bss)
	}
	_bootloader_physical_end = .;

	. = 0xC0000000;
	_kernel_virtual_start = .;
	_kernel_physical_start = ((_kernel_virtual_start - 0xC0000000) + 0x200000);
	.text ALIGN(4096) : AT ((ADDR (.text) - (_kernel_virtual_start) + 0x200000))
	{
		*(.text)
		*(.text.*)
	}
 
	.rodata ALIGN(4096) : AT ((ADDR (.rodata) - (_kernel_virtual_start) + 0x200000))
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data ALIGN(4096) : AT ((ADDR (.data) - (_kernel_virtual_start) + 0x200000))
	{
		*(.data)
		*(.data.*)
	}

	.bss ALIGN(4096) : AT ((ADDR (.bss) - (_kernel_virtual_start) + 0x200000))
	{
		_kernel_bss_start = .;
		*(COMMON)
		*(.bss)
		*(.bss.*)
		_kernel_stack_bottom = .;
		. += 4096;
		_kernel_stack_top = .;
		_kernel_bss_end = .;
	}
	_kernel_virtual_end = .;
	_kernel_physical_end = ((_kernel_virtual_end - 0xC0000000) + 0x200000);
}
