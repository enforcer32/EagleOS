file(GLOB_RECURSE MemorySources
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/MemoryRegion.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageDirectory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageDirectory.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageTable.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageTable.cpp"
)

add_library(${MEMORY_TARGET_NAME} OBJECT ${MemorySources})
target_include_directories(${MEMORY_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../")
