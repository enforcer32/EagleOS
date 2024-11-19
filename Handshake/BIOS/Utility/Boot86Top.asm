%ifndef Boot86Top
%define Boot86Top

%macro EnterRealMode 0
	; Disable Interrupts
	cli

	; Disable Paging

	; Save GDT
	sgdt[gdt_backup]

	; Load BIOS IDT
	lidt[bios_idt]

	; Load 16-bit GDT
	lgdt[gdt16_descriptor]

	; Jump to 16-bit Code Segment
	jmp 0x08:.rminit

.rminit:
	[BITS 16]

	; Load data segment selectors with DATA16_SEG
	mov ax, 0x10
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

	; Load Saved GDT
	lgdt[gdt_backup]

	; set PE (PROTECTION ENABLED) bit in cr0
	mov eax, cr0
	or al, 1
	mov cr0, eax

	; Jump to Protected Mode
	jmp 0x08:.pm

.pm:
	[BITS 32]
	; Load data segment selectors DATA32_SEG
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	; Enable Interrupts
	;sti
%endmacro

%endif
