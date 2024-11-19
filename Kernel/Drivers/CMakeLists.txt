file(GLOB_RECURSE DriversSources
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.cpp"
)

set(DriversSources ${DriversSources} PARENT_SCOPE)
