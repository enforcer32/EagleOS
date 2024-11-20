section .text

global CPU_EnablePaging
global CPU_DisablePaging
global CPU_LoadPageDirectory

CPU_EnablePaging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	pop ebp
	ret

CPU_DisablePaging:
	push ebp
	mov ebp, esp
	mov eax, cr0
	and eax, 0x7FFFFFFF
	mov cr0, eax
	pop ebp
	ret

CPU_LoadPageDirectory:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	pop ebp
	ret
