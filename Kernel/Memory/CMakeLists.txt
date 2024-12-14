file(GLOB_RECURSE MemorySources
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualAddress.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PhysicalMemoryManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageInfo.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/MemoryUtil.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/MemoryUtil.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageDirectory.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageDirectory.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageTable.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/PageTable.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryManager.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryManager.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryManager.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryAllocator.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/VirtualMemoryAllocator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Heap.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Heap.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/KMalloc.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/KMalloc.cpp"
)

set(MemorySources ${MemorySources} PARENT_SCOPE)
