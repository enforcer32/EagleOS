ENTRY(_handshakestart)
OUTPUT_FORMAT(binary)

SECTIONS 
{
	. = 0x0000B000; /* Load 2nd Stage Bootloader Address */

	__bootloader_physical_start = .;
	.text : ALIGN(4096)
	{
		*(.hsentry)
		*(.text)
		*(.text.*)
	}
 
	.rodata : ALIGN(4096)
	{
		*(.rodata)
		*(.rodata.*)
	}

	.data : ALIGN(4096)
	{
		*(.data)
		*(.data.*)
	}

	.bss : ALIGN(4096)
	{
		*(COMMON)
		*(.bss)
		*(.bss.*)
	}
	__bootloader_physical_end = .;
}
