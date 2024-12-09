rm ./Build/Handshake/EagleOS/Bootloader.bin.lock
rm ./Build/Kernel/EagleOS/Kernel.elf.lock
bochs -f .bochsrc
