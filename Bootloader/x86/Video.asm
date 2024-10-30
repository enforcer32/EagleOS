section .text

global PrintChar

%include "Bootloader/x86/Boot86Top.asm"

PrintChar:
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

%include "Bootloader/x86/Boot86Bot.asm"
