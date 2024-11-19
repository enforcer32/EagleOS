file(GLOB_RECURSE BootloaderSources
	"${CMAKE_CURRENT_SOURCE_DIR}/BootInfo.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Boot/Bootloader.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/Boot/BLMain.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Boot/MemoryInfo.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Lib/BPrintf.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Lib/BPrintf.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Lib/BPanic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Lib/BPanic.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Video.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Video.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Memory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Memory.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/IO.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/IO.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/IO.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Paging/PageManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Paging/PageManager.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/x86/Paging/PageManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Memory/BMalloc.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Memory/BMalloc.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/System/SystemMemory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/System/SystemMemory.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.cpp"
)

add_library(${BOOTLOADER_TARGET_NAME} OBJECT ${ESTDSources} ${ELIBSources} ${BootloaderSources})
target_compile_definitions(${BOOTLOADER_TARGET_NAME} PRIVATE ESTD_BOOTLOADER_BUILD)
target_include_directories(${BOOTLOADER_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")

# BOOTLOADER OBJECTS
set(BootloaderObjects "")

# APPEND ESTD
foreach(SOURCE_FILE ${ESTDSources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${BOOTLOADER_TARGET_NAME}.dir/__ ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND BootloaderObjects ${ObjectFile})
	endif()
endforeach()

# APPEND ELIB
foreach(SOURCE_FILE ${ELIBSources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${PROJECT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${BOOTLOADER_TARGET_NAME}.dir/__ ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND BootloaderObjects ${ObjectFile})
	endif()
endforeach()

# APPEND INTERNAL SOURCES
foreach(SOURCE_FILE ${BootloaderSources})
	string(FIND ${SOURCE_FILE} ".h" index)
	if(index EQUAL -1)
			string(REPLACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${BOOTLOADER_TARGET_NAME}.dir ObjectFile ${SOURCE_FILE})
			string(REPLACE ".cpp" ".cpp.o" ObjectFile ${ObjectFile})
			string(REPLACE ".asm" ".asm.o" ObjectFile ${ObjectFile})
			list(APPEND BootloaderObjects ${ObjectFile})
	endif()
endforeach()

# BOOTLOADER TARGET
add_custom_command(OUTPUT BootloaderBIN POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND rm -f ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootsector.bin >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME}
	COMMAND dd if=/dev/zero bs=512 count=1000 >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderBuild.bin of=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin conv=notrunc
	COMMAND dd if=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderAligned.bin >> ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/${BOOTLOADER_OUTPUT_NAME}
	DEPENDS BootloaderBuild
)

add_custom_command(OUTPUT BootloaderBuild POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND ${CMAKE_LINKER} ${CMAKE_LINKER_FLAGS} ${BootloaderObjects} -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o.map
	COMMAND ${CMAKE_CXX_COMPILER} -T ${BOOTLOADER_LINKERSCRIPT} ${CMAKE_CXX_FLAGS_NQ} ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/RelocatableBootloader.o -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderBuild.bin -Xlinker -Map=${CMAKE_CURRENT_BINARY_DIR}/EagleOS/BootloaderBuild.bin.map
	DEPENDS BootSector
)

add_custom_command(OUTPUT BootSector POST_BUILD COMMAND_EXPAND_LISTS
	COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/EagleOS
	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin ${BOOTLOADER_TARGET_DIR}/Boot/Bootsector.asm -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootsector.bin
	DEPENDS ${BOOTLOADER_TARGET_NAME}
)

# OS BOOTLOADER
add_custom_target(EOSBootloader ALL
	DEPENDS BootloaderBIN
)
