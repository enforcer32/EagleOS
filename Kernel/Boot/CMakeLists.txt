file(GLOB_RECURSE BootSources
	"${CMAKE_CURRENT_SOURCE_DIR}/BootInfo.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/MemoryMap.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Multiboot.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Multiboot.cpp"
)

set(BootSources ${BootSources} PARENT_SCOPE)
