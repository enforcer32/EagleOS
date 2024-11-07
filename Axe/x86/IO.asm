section .text

global x86_INSB
global x86_OUTB
global x86_INSW
global x86_OUTW
global x86_INSD
global x86_OUTD

x86_INSB:
	push ebp
	mov ebp, esp
	xor eax, eax
	mov edx, [ebp+8]
	in al, dx
	pop ebp
	ret

x86_OUTB:
	push ebp
	mov ebp, esp
	mov eax, [ebp+12]
	mov edx, [ebp+8]
	out dx, al
	pop ebp
	ret

x86_INSW:
	push ebp
	mov ebp, esp
	xor eax, eax
	mov edx, [ebp+8]
	in ax, dx
	pop ebp
	ret

x86_OUTW:
	push ebp
	mov ebp, esp
	mov eax, [ebp+12]
	mov edx, [ebp+8]
	out dx, ax
	pop ebp
	ret

x86_INSD:
	push ebp
	mov ebp, esp
	xor eax, eax
	mov edx, [ebp+8]
	in eax, dx
	pop ebp
	ret

x86_OUTD:
	push ebp
	mov ebp, esp
	xor eax, eax
	mov eax, [ebp+12]
	mov edx, [ebp+8]
	out dx, eax
	pop ebp
	ret
