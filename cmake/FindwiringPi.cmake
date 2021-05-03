find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)

add_library(wiringPi::wiringPi IMPORTED INTERFACE GLOBAL)
target_link_libraries(wiringPi::wiringPi INTERFACE ${WIRINGPI_LIBRARIES})
set_target_properties(wiringPi::wiringPi PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES ${WIRINGPI_INCLUDE_DIRS}
)
