[BITS 32]

; Multiboot Configurations
MULTIBOOT_MAGIC equ 0x1BADB002
MULTIBOOT_ALIGN equ 0x00000001
MULTIBOOT_MEMORY_INFO equ 0x00000002
MULTIBOOT_VIDEO_MODE equ 0x00000004
MULTIBOOT_FLAGS equ (MULTIBOOT_ALIGN | MULTIBOOT_MEMORY_INFO)
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; Kernel Configurations
KERNEL_PHYSICAL_LOAD_ADDRESS equ 0x200000
KERNEL_VIRTUAL_LOAD_ADDRESS equ 0xC0000000

; Globals
global _kernelstart
extern _kernel_bss_start
extern _kernel_bss_end
extern _kernel_stack_top
extern KMain

; Multiboot Headers
section .multiboot
align 4
_multiboot:
	dd MULTIBOOT_MAGIC
	dd MULTIBOOT_FLAGS
	dd MULTIBOOT_CHECKSUM

; Boot
section .boot.text
_kernelstart:
	; Clear Interrupts
	cli

	; Setup boot stack
	mov ebp, _boot_stack_top
	mov esp, ebp

	; Save Multiboot Magic
	push eax
	; Save Multiboot Info
	push ebx

	; Zero .bss section
	;mov edi, _kernel_bss_start
	;mov ecx, _kernel_bss_end
	;sub ecx, _kernel_bss_start
	;mov al, 0
	;rep stosb

	; Identity map from 0x0 to 0x400000
	mov ecx, 0
	mov ebx, 0x0 ; Virtual address starts at 0x0
	mov edx, 0x0 ; Physical address starts at 0x0
_identity_map:
	; Page table entry: physical address | flags
	mov eax, edx
	or eax, 0x003 ; Set Present (P) and Read/Write (RW) flags (0x003 is 00000011)
	mov [_boot_page_table0 + ecx * 4], eax

	; Increment the addresses for the next page
	add ebx, 0x1000 ; Increment the virtual address by 4 KB
	add edx, 0x1000 ; Increment the physical address by 4 KB

	inc ecx ; Move to the next page table entry
    cmp ecx, 1024 ; Ensure we map 256 entries
    jl _identity_map ; Loop if we haven't mapped all 256 entries

	; Memory Map 0xC0000000 -> 0x200000
    mov ecx, 0
    mov ebx, KERNEL_VIRTUAL_LOAD_ADDRESS
    mov edx, KERNEL_PHYSICAL_LOAD_ADDRESS
_map_pages:
    mov eax, edx
    or eax, 0x003
    mov [_boot_page_table768 + ecx * 4], eax

    add ebx, 0x1000
    add edx, 0x1000

	inc ecx
    cmp ecx, 1024
    jl _map_pages

	; Set up the page directory entries for page tables
	mov eax, _boot_page_table0
	or eax, 0x003 ; Set Present (P) and Read/Write (RW) flags
	mov [_boot_page_directory], eax ; Identity map is mapped at index 0 of the page directory

	mov eax, _boot_page_table768
	or eax, 0x003
	mov [_boot_page_directory + 768 * 4], eax

	; Load Page Directory
    mov eax, _boot_page_directory
    mov cr3, eax

    ; Enable paging
    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

	lea ebx, [_kernelstart_higherhalf]
	jmp ebx

section .text
_kernelstart_higherhalf:
	; Setup Kernel Stack
	mov ebp, _kernel_stack_top
	mov esp, ebp

	; Restore Multiboot Magic
	mov ebx, [_boot_stack_top-4]
	; Restore Multiboot Info
	mov edx, [_boot_stack_top-8]
	push edx
	push ebx

	; Call Higherhalf Kernel
	call KMain

	; Halt
	cli
.hang:
	hlt
	jmp .hang

section .boot.data
; Page Tables/Directory
align 4096
_boot_page_directory: times 1024 dd 0
_boot_page_table0: times 1024 dd 0
_boot_page_table768: times 1024 dd 0

section .boot.bss nobits
align 16
_boot_stack_bottom: resb 16384
_boot_stack_top:
