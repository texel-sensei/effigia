cmake_minimum_required(VERSION 3.7)
project(sdl_display LANGUAGES CXX)

find_package(SDL2 QUIET)
find_package(Threads QUIET)

if(NOT SDL2_FOUND OR NOT Threads_FOUND)
	set(SKIP_MODULE TRUE PARENT_SCOPE)
	return()
endif()

add_library(sdl_display MODULE
	plugin_main.cpp
)
target_link_libraries(sdl_display PRIVATE
	SDL2::SDL2
	Threads::Threads
	plugin_interface
)

set_target_properties(sdl_display PROPERTIES
	PREFIX ""
	SUFFIX ".mod"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
