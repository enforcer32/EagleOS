cmake_minimum_required(VERSION 3.28)

project(EagleOS
	VERSION 1.0.0
	LANGUAGES CXX
)

# DETECT TOOLS
find_program(QEMU_EXECUTABLE qemu-system-i386)
if(NOT QEMU_EXECUTABLE)
    message(FATAL_ERROR "QEMU not found. Please install QEMU and ensure it is in your PATH.")
endif()

# TOOLCHAIN
set(CMAKE_TOOLCHAIN_PATH $ENV{TOOLCHAIN_PATH})

# COMPILER CONFIGS
set(CMAKE_CXX_COMPILER ${CMAKE_TOOLCHAIN_PATH}/i686-elf-g++)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-m32 -g -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -O0 -falign-jumps -falign-functions -falign-labels -fomit-frame-pointer -finline-functions -falign-loops -fstrength-reduce -Wno-unused-function -Wno-unused-parameter -fno-builtin -Wno-unused-label -Wno-cpp -fno-rtti -fno-exceptions -Iinc")
set(CMAKE_CXX_FLAGS_NQ -m32 -g -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -O0 -falign-jumps -falign-functions -falign-labels -fomit-frame-pointer -finline-functions -falign-loops -fstrength-reduce -Wno-unused-function -Wno-unused-parameter -fno-builtin -Wno-unused-label -Wno-cpp -fno-rtti -fno-exceptions -Iinc)

# LINKER CONFIGS
set(CMAKE_LINKER ${CMAKE_TOOLCHAIN_PATH}/i686-elf-ld)
set(CMAKE_LINKER_FLAGS -g -relocatable)
set(KERNEL_LINKERSCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Kernel/linker.ld)

# ASSEMBLER FLAGS
set(CMAKE_ASM_NASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_NASM_FLAGS "-f elf32 -g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)

# OUTPUT
set(KERNEL_OUTPUT_NAME Kernel.elf)
set(KERNEL_ISO_OUTPUT_NAME Kernel.iso)

set(KERNEL_TARGET_NAME Kernel)
set(KERNEL_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Kernel)

add_subdirectory(${KERNEL_TARGET_DIR})

# RUN QEMU
add_custom_target(runqemu
	COMMAND dd if=/dev/zero bs=512 count=2000 >> ${CMAKE_BINARY_DIR}/Kernel/EagleOS/hdd.img
	COMMAND echo "Hello EagleOS!" | dd of=${CMAKE_BINARY_DIR}/Kernel/EagleOS/hdd.img conv=notrunc
    COMMAND ${QEMU_EXECUTABLE} -m 1024M -d int,cpu_reset -no-reboot -no-shutdown -kernel ${CMAKE_BINARY_DIR}/Kernel/EagleOS/${KERNEL_OUTPUT_NAME} -hda ${CMAKE_BINARY_DIR}/Kernel/EagleOS/hdd.img
)

# Generate ISO
add_custom_target(geniso
    COMMAND cp ${CMAKE_BINARY_DIR}/Kernel/EagleOS/${KERNEL_OUTPUT_NAME} ${CMAKE_SOURCE_DIR}/iso/boot
	COMMAND grub-mkrescue -o ${CMAKE_BINARY_DIR}/Kernel/EagleOS/${KERNEL_ISO_OUTPUT_NAME} ${CMAKE_SOURCE_DIR}/iso
)
