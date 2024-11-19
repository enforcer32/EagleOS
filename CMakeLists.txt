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
set(BOOTLOADER_LINKERSCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Axe/linker.ld)
set(KERNEL_LINKERSCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/Kernel/linker.ld)

# ASSEMBLER FLAGS
set(CMAKE_ASM_NASM_COMPILER /usr/bin/nasm)
set(CMAKE_ASM_NASM_FLAGS "-f elf32 -g")
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
enable_language(ASM_NASM)

# OUTPUT
set(BOOTLOADER_OUTPUT_NAME Bootloader.bin)
set(KERNEL_OUTPUT_NAME Kernel.elf)

set(ESTD_TARGET_NAME ESTD)
set(ESTD_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ESTD)
set(ELIB_TARGET_NAME ELIB)
set(ELIB_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ELIB)
set(BOOTLOADER_TARGET_NAME Axe)
set(BOOTLOADER_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Axe)
set(KERNEL_TARGET_NAME Kernel)
set(KERNEL_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Kernel)

add_subdirectory(${ESTD_TARGET_DIR})
add_subdirectory(${ELIB_TARGET_DIR})
add_subdirectory(${BOOTLOADER_TARGET_DIR})
add_subdirectory(${KERNEL_TARGET_DIR})

# RUN QEMU
add_custom_target(runqemu
    COMMAND ${QEMU_EXECUTABLE} -m 128M -d int,cpu_reset -no-reboot -no-shutdown -hda ${CMAKE_BINARY_DIR}/Axe/EagleOS/${BOOTLOADER_OUTPUT_NAME} -hdb ${CMAKE_BINARY_DIR}/Kernel/EagleOS/${KERNEL_OUTPUT_NAME}
)
