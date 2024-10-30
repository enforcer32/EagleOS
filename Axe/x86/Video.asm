section .text

global PrintChar

%include "Axe/x86/Boot86Top.asm"

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

%include "Axe/x86/Boot86Bot.asm"
