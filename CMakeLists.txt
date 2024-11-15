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
set(BOOTLOADER_OUTPUT_NAME EagleBootloader.bin)
set(KERNEL_OUTPUT_NAME EagleKernel.elf)

set(ESTD_TARGET_NAME ESTD)
set(ESTD_TARGET_DIR ESTD)
set(BOOTLOADER_TARGET_NAME Axe)
set(BOOTLOADER_TARGET_DIR Axe)
set(KERNEL_TARGET_NAME Kernel)
set(KERNEL_TARGET_DIR Kernel)
set(ARCH_TARGET_NAME KernelArch)
set(ARCH_TARGET_DIR Kernel/Arch)
set(DRIVERS_TARGET_NAME KernelDrivers)
set(DRIVERS_TARGET_DIR Kernel/Drivers)
set(MEMORY_TARGET_NAME KernelMemory)
set(MEMORY_TARGET_DIR Kernel/Memory)

add_subdirectory(${ESTD_TARGET_DIR})
add_subdirectory(${BOOTLOADER_TARGET_DIR})
add_subdirectory(${KERNEL_TARGET_DIR})
add_subdirectory(${ARCH_TARGET_DIR})
add_subdirectory(${DRIVERS_TARGET_DIR})
add_subdirectory(${MEMORY_TARGET_DIR})

# OS TARGET
add_custom_target(EagleOS ALL
	DEPENDS EagleBootloader
	DEPENDS EagleKernel
)

# BOOTLOADER TARGET
add_custom_command(OUTPUT EagleBootloader POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootsector.bin >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME}
	COMMAND dd if=/dev/zero bs=512 count=1000 >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootloader.bin of=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin conv=notrunc
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME}

	DEPENDS Bootloader
)

add_custom_command(OUTPUT Bootloader POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} $<TARGET_OBJECTS:${BOOTLOADER_TARGET_NAME}> $<TARGET_OBJECTS:${ESTD_TARGET_NAME}> -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o.map
	COMMAND ${CMAKE_CXX_COMPILER} -T ${BOOTLOADER_LINKERSCRIPT} ${CMAKE_CXX_FLAGS_NQ} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootloader.bin -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootloader.bin.map

	DEPENDS BootSector
)

add_custom_command(OUTPUT BootSector POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/EagleOS
	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin ${CMAKE_CURRENT_SOURCE_DIR}/${BOOTLOADER_TARGET_DIR}/Boot/Bootsector.asm -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootsector.bin

	DEPENDS ${BOOTLOADER_TARGET_NAME}
)

# KERNEL TARGET
add_custom_command(OUTPUT EagleKernel POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${KERNEL_OUTPUT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf
	COMMAND dd if=/dev/zero bs=512 count=1000 > ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Kernel.elf of=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf conv=notrunc
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${KERNEL_OUTPUT_NAME}

	DEPENDS ELFKernel
)

add_custom_command(OUTPUT ELFKernel POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND ${CMAKE_CXX_COMPILER} -T ${KERNEL_LINKERSCRIPT} ${CMAKE_CXX_FLAGS_NQ} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Kernel.elf -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Kernel.elf.map

	DEPENDS RelocatableKernel
)

add_custom_command(OUTPUT RelocatableKernel POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/EagleOS
	COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} $<TARGET_OBJECTS:${KERNEL_TARGET_NAME}> $<TARGET_OBJECTS:${ESTD_TARGET_NAME}> $<TARGET_OBJECTS:${ARCH_TARGET_NAME}> $<TARGET_OBJECTS:${DRIVERS_TARGET_NAME}> $<TARGET_OBJECTS:${MEMORY_TARGET_NAME}> -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o.map

	DEPENDS ${ESTD_TARGET_NAME}
	DEPENDS ${KERNEL_TARGET_NAME}
	DEPENDS ${ARCH_TARGET_NAME}
	DEPENDS ${DRIVERS_TARGET_NAME}
	DEPENDS ${MEMORY_TARGET_NAME}
)

# RUN QEMU
add_custom_target(runqemu
    COMMAND ${QEMU_EXECUTABLE} -m 128M -d int,cpu_reset -no-reboot -no-shutdown -hda ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME} -hdb ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${KERNEL_OUTPUT_NAME}
    
	DEPENDS EagleOS
)
