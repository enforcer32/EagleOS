file(GLOB_RECURSE KernelSources
	"${CMAKE_CURRENT_SOURCE_DIR}/Kernel.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPrintf.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPrintf.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPanic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPanic.cpp"
)

set(ARCH_TARGET_NAME KernelArch)
set(ARCH_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Arch)
set(DRIVERS_TARGET_NAME KernelDrivers)
set(DRIVERS_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Drivers)
set(MEMORY_TARGET_NAME KernelMemory)
set(MEMORY_TARGET_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Memory)

add_subdirectory(${ARCH_TARGET_DIR})
add_subdirectory(${DRIVERS_TARGET_DIR})
add_subdirectory(${MEMORY_TARGET_DIR})

set(KernelEntrySourceFile ${CMAKE_CURRENT_SOURCE_DIR}/Kernel.asm)
set(KernelEntryObjectFile ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${KERNEL_TARGET_NAME}.dir/Kernel.asm.o)

add_library(${KERNEL_TARGET_NAME} OBJECT ${KernelEntrySourceFile} ${ESTDSources} ${ELIBSources} ${KernelSources} ${ArchSources} ${DriversSources} ${MemorySources})
target_compile_definitions(${KERNEL_TARGET_NAME} PRIVATE ESTD_KERNEL_BUILD)
target_include_directories(${KERNEL_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")

# KERNEL OBJECTS
set(KernelObjects "")

# APPEND ESTD
foreach(SOURCE_FILE ${ESTDSources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${KERNEL_TARGET_NAME}.dir/__ ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND KernelObjects ${ObjectFile})
	endif()
endforeach()

# APPEND ELIB
foreach(SOURCE_FILE ${ELIBSources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${KERNEL_TARGET_NAME}.dir/__ ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND KernelObjects ${ObjectFile})
	endif()
endforeach()

# APPEND INTERNAL SOURCES
foreach(SOURCE_FILE ${KernelSources} ${ArchSources} ${DriversSources} ${MemorySources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${KERNEL_TARGET_NAME}.dir ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND KernelObjects ${ObjectFile})
	endif()
endforeach()

# KERNEL TARGET
add_custom_command(OUTPUT KernelELF POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${KERNEL_OUTPUT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf
	COMMAND dd if=/dev/zero bs=512 count=1000 > ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelBuild.elf of=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf conv=notrunc
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelAligned.elf >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${KERNEL_OUTPUT_NAME}
	DEPENDS KernelBuild
)

add_custom_command(OUTPUT KernelBuild POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND ${CMAKE_CXX_COMPILER} -T ${KERNEL_LINKERSCRIPT} ${CMAKE_CXX_FLAGS_NQ} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelBuild.elf -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/KernelBuild.elf.map
	DEPENDS RelocatableKernel
)

add_custom_command(OUTPUT RelocatableKernel POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/EagleOS
	COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} ${KernelEntryObjectFile} ${KernelObjects} -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableKernel.o.map
	DEPENDS ${KERNEL_TARGET_NAME}
)

# OS KERNEL
add_custom_target(EOSKernel ALL
	DEPENDS KernelELF
)
