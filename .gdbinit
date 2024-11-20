set architecture i386
set auto-load safe-path .
set disassembly-flavor intel

add-symbol-file ./Build/Handshake/EagleOS/RelocatableBootloader.o 0x0000B000
add-symbol-file ./Build/Kernel/EagleOS/RelocatableKernel.o 0x00100000
#break Kernel/Kernel.cpp:KMain
break Handshake/HS/Handshake.cpp:HSMain
target remote | qemu-system-i386 -hda ./Build/Handshake/EagleOS/Bootloader.bin -hdb ./Build/Kernel/EagleOS/Kernel.elf -S -gdb stdio

define hook-stop
print/x $eax
print/x $ebx
print/x $ecx
print/x $edx
print $eflags
x/xw $esp

disassemble $eip, +10

end
