file(GLOB_RECURSE KernelSources
	"${CMAKE_CURRENT_SOURCE_DIR}/Kernel.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kernel.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPrintf.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPrintf.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPanic.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Kern/KPanic.cpp"
)

add_library(${KERNEL_TARGET_NAME} OBJECT ${KernelSources})
target_include_directories(${KERNEL_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../")
