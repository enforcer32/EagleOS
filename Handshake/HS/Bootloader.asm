section .text

[BITS 32]

global _handshakestart

extern HSMain

CODE_SEG equ 0x08 ; Selector 0x08 will be our code segment offset
DATA_SEG equ 0x10 ; Selector 0x10 will be our data segment offset
HANDSHAKE_MEMORY_STACK_ADDRESS equ 0x0000B000

_handshakestart:
	; set data segment
	mov eax, DATA_SEG
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax
	mov ss, eax
	mov ebp, HANDSHAKE_MEMORY_STACK_ADDRESS
	mov esp, ebp

	; Enable A20 Line to access all memory
	in al, 0x92
	or al, 2
	out 0x92, al

	; Handshake Main
	call HSMain

	; Halt
	cli
.hang:
	hlt
	jmp .hang

times 512-($-$$) db 0
