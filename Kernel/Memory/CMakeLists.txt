file(GLOB_RECURSE MemorySources
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.cpp"
)

set(MemorySources ${MemorySources} PARENT_SCOPE)
