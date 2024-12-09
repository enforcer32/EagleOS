section .text

global x86_EnablePaging
global x86_DisablePaging
global x86_LoadPageDirectory

x86_EnablePaging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	pop ebp
	ret

x86_DisablePaging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	and eax, 0x7FFFFFFF
	mov cr0, eax
	pop ebp
	ret

x86_LoadPageDirectory:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	pop ebp
	ret
