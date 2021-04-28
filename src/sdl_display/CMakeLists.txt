cmake_minimum_required(VERSION 3.7)
project(sdl_display LANGUAGES CXX)

find_package(SDL2)
find_package(Threads)

add_library(sdl_display MODULE
	plugin_main.cpp
)
target_link_libraries(sdl_display PRIVATE
	SDL2::SDL2
	Threads::Threads
)
target_include_directories(sdl_display PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)

set_target_properties(sdl_display PROPERTIES
	PREFIX ""
	SUFFIX ".mod"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
