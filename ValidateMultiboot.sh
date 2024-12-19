if grub-file --is-x86-multiboot Build/Kernel/EagleOS/Kernel.elf; then
	echo "Valid Multiboot Format!"
else
	echo "Invalid Multiboot Format!"
fi
