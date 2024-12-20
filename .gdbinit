set architecture i386
set auto-load safe-path .
set disassembly-flavor intel

add-symbol-file Build/Kernel/EagleOS/Kernel.elf 0x00100000
#break Kernel/Arch/x86/Boot.asm:_kernelstart
break *0x00104000
target remote | qemu-system-i386 -m 1024M -d int,cpu_reset -no-reboot -no-shutdown -kernel Build/Kernel/EagleOS/Kernel.elf -S -gdb stdio
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
