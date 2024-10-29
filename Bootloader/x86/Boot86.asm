section .text

global BIOSPrintChar

CODE16_SEG equ gdt16_code - gdt16_start ; Selector 0x08 will be our 16-bit code segment offset
DATA16_SEG equ gdt16_data - gdt16_start ; Selector 0x10 will be our 16-bit data segment offset
CODE32_SEG equ 0x08 ; Selector 0x08 will be our code 32-bit segment offset
DATA32_SEG equ 0x10 ; Selector 0x10 will be our data 32-bit segment offset

%macro EnterRealMode 0
	; Disable Interrupts
	cli

	; Disable Paging

	; Save Kernel GDT
	sgdt[kernel_gdt_backup]

	; Load BIOS IDT
	lidt[bios_idt]

	; Load 16-bit GDT
	lgdt[gdt16_descriptor]

	; Jump to 16-bit Code Segment
	jmp CODE16_SEG:.rminit

.rminit:
	[BITS 16]

	; Load data segment selectors with DATA16_SEG
	mov ax, DATA16_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; LOAD Real Mode IDT

	; Disable Protected Mode
	mov eax, cr0
	and al, ~1
	mov cr0, eax

	; Jump to Real Mode
	jmp 0x00:.rm

.rm:
	; Load data segment selectors 0
	mov ax, 0x00
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Setup Stack
	mov sp, 0x5400

	; Enable Interrupts
	sti
%endmacro

%macro EnterProtectedMode 0
	; Disable Interrupts
	cli

	; Enable Paging

	; Load Saved Kernel GDT
	lgdt[kernel_gdt_backup]

	; set PE (PROTECTION ENABLED) bit in cr0
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; Jump to Protected Mode
	jmp CODE32_SEG:.pm

.pm:
	[BITS 32]
	; Load data segment selectors DATA32_SEG
	mov ax, DATA32_SEG
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Enable Interrupts
	;sti
%endmacro

BIOSPrintChar:
	push ebp
	mov ebp, esp

	EnterRealMode

	mov ah, 0xE
	mov al, [bp+8]
	mov bx, 0x0
	int 0x10

	EnterProtectedMode
	
	mov esp, ebp
	pop ebp
	ret

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
