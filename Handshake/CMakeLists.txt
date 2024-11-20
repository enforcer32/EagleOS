file(GLOB_RECURSE BootloaderSources
	# HS
	"${CMAKE_CURRENT_SOURCE_DIR}/HS/Bootloader.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/HS/Handshake.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/HS/Handshake.cpp"

	# BIOS
	"${CMAKE_CURRENT_SOURCE_DIR}/BIOS/Video/Print.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/BIOS/Video/Print.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/BIOS/Video/Print.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/BIOS/Memory/E820Memory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/BIOS/Memory/E820Memory.asm"

	# CPU
	"${CMAKE_CURRENT_SOURCE_DIR}/CPU/IO.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/CPU/IO.asm"

	# Memory
	"${CMAKE_CURRENT_SOURCE_DIR}/Memory/Malloc.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Memory/Malloc.cpp"

	# VIDEO
	"${CMAKE_CURRENT_SOURCE_DIR}/Video/VGA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Video/VGA.cpp"

	# STDIO
	"${CMAKE_CURRENT_SOURCE_DIR}/Stdio/Print.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Stdio/Print.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Stdio/Panic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Stdio/Panic.cpp"

	# SYSTEM
	"${CMAKE_CURRENT_SOURCE_DIR}/System/MemoryMap.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/System/MemoryMap.cpp"
)

add_library(${BOOTLOADER_TARGET_NAME} OBJECT ${ESTDSources} ${ELIBSources} ${BootloaderSources})
target_compile_definitions(${BOOTLOADER_TARGET_NAME} PRIVATE ESTD_NONE_BUILD)
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
	COMMAND ${CMAKE_ASM_NASM_COMPILER} -f bin ${BOOTLOADER_TARGET_DIR}/HS/Bootsector.asm -o ${CMAKE_CURRENT_BINARY_DIR}/EagleOS/Bootsector.bin
	DEPENDS ${BOOTLOADER_TARGET_NAME}
)

# OS BOOTLOADER
add_custom_target(EOSBootloader ALL
	DEPENDS BootloaderBIN
)
