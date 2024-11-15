file(GLOB_RECURSE DriversSources
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Graphics/VGA.cpp"
)

add_library(${DRIVERS_TARGET_NAME} OBJECT ${DriversSources})
target_include_directories(${DRIVERS_TARGET_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../")
