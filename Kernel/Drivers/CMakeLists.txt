file(GLOB_RECURSE DriversSources
	# Graphics
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.cpp"

	# Storage
	"${CMAKE_CURRENT_SOURCE_DIR}/Storage/ATA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Storage/ATA.asm"
	"${CMAKE_CURRENT_SOURCE_DIR}/Storage/ATA.cpp"
)

set(DriversSources ${DriversSources} PARENT_SCOPE)
