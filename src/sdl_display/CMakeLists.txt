find_package(SDL2 REQUIRED)

find_package(Threads REQUIRED)

add_library(sdl_display MODULE
	plugin_main.cpp
)
target_link_libraries(sdl_display PRIVATE
	plugin_interface
	SDL2::SDL2
	Threads::Threads
)

set_target_properties(sdl_display PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
