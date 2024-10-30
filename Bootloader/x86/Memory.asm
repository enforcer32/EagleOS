section .text

global E820QuerySystemAddressMap

%include "Bootloader/x86/Boot86Top.asm"

; Int 15/AX=E820h
; EAX = 0000E820h
; EDX = 534D4150h ('SMAP') SIGNATURE
; EBX = continuation value or 00000000h to start at beginning of map
; ECX = size of buffer for result, in bytes (should be >= 20 bytes)
; ES:DI -> buffer for result
E820QuerySystemAddressMap:
	push ebp
	mov ebp, esp

	EnterRealMode
	push ebx
    push ecx
    push edx
    push esi
    push edi
    push ds
    push es

	; Load E820MemoryMap Address into [ES:DI]
	mov edi, [bp+8]
	shr edi, 4
	mov es, di
	mov edi, [bp+8]
	and edi, 0xf

	; Load continueID into [DS:SI]
	mov esi, [bp+12]
	shr esi, 4
	mov ds, si
	mov esi, [bp+12]
	and esi, 0xf
	mov ebx, ds:[si]

	; Call int 15
	mov eax, 0xE820 ; Function
	mov edx, 0x534D4150 ; Signature
	mov ecx, 0x18 ; Size 24bytes
	int 0x15

	; Validate Results
	cmp eax, 0x534D4150
	jne .Error

	mov eax, ecx
	mov ds:[si], ebx
	jmp .Done

.Error:
	mov eax, -1

.Done:
	pop es
    pop ds
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx

	push eax
	EnterProtectedMode
	pop eax

	mov esp, ebp
	pop ebp
	ret

%include "Bootloader/x86/Boot86Bot.asm"
