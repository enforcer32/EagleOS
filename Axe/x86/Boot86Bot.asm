%ifndef Boot86GDT
%define Boot86GDT

gdt16_start:
	; offset 0x00
	gdt16_null:
		dd 0
		dd 0

	; offset 0x08
	gdt16_code:
		dw 0xffff ;16-bits of limit
		dw 0 ; 16-bits of base
		db 0 ; 8-bits of base
		db 10011010b ; 4-bit privilege + 4-bit type flags
		db 00001111b ; 4-bit other flags + 4 bits of limit
		db 0 ; 8 bits of base

	; offset 0x10
	gdt16_data:
		dw 0xffff
		dw 0
		db 0
		db 10010010b
		db 00001111b
		db 0
gdt16_end:

; GDT Pointer to base and limit
gdt16_descriptor:
	dw gdt16_end - gdt16_start - 1 ; size
	dd gdt16_start ; base

bios_idt:
	dw 0x3FF
	dd 0

kernel_gdt_backup: dd 0

%endif
