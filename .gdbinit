set architecture i386
set auto-load safe-path .
set disassembly-flavor intel

add-symbol-file ./Build/Axe/BootloaderObject.o 0x0000B000
add-symbol-file ./Build/Kernel/KernelObject.o 0x00100000
#break Axe/BLMain.cpp:BLMain
#break Kernel/Kernel.asm:_kernelstart
#break Kernel/Kernel.cpp:KMain
#break Axe/ELF/ELF.cpp:25
#break Kernel/Kernel.cpp:KMain:32
#break Kernel/Memory/VirtualMemoryManager.cpp:59
#break Kernel/Memory/PageDirectory.cpp:82
#break Axe/x86/Paging/PageManager.cpp:62
break Axe/ELF/ELF.cpp:ParseSectionHeaders
target remote | qemu-system-i386 -hda ./Build/Bootloader.bin -hdb ./Build/Kernel.elf -S -gdb stdio
c

define hook-stop
print/x $eax
print/x $ebx
print/x $ecx
print/x $edx
print $eflags
x/xw $esp

disassemble $eip, +10

end
