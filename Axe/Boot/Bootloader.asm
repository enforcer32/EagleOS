section .bootloaderentrypoint

[BITS 32]

global _bootloaderstart

extern BLMain

CODE_SEG equ 0x08 ; Selector 0x08 will be our code segment offset
DATA_SEG equ 0x10 ; Selector 0x10 will be our data segment offset

_bootloaderstart:
	; set data segment
	mov eax, DATA_SEG
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	mov ebp, 0x00009000
	mov esp, ebp

	; Enable A20 Line to access all memory
	in al, 0x92
	or al, 2
	out 0x92, al

	; Kernel
	call BLMain

	; Halt
	cli
.hang:
	hlt
	jmp .hang

times 512-($-$$) db 0
