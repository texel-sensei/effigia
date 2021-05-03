cmake_minimum_required(VERSION 3.7)
project(waveshare_display LANGUAGES C)

find_package(wiringPi QUIET)

if(NOT ${wiringPi_FOUND})
	set(SKIP_MODULE TRUE PARENT_SCOPE)
	return()
endif()

add_library(waveshare_display MODULE
	Debug.h
	DEV_Config.c
	DEV_Config.h
	EPD_7in5b.c
	EPD_7in5b.h
	GUI_BMPfile.c
	GUI_BMPfile.h
	GUI_Paint.c
	GUI_Paint.h
	ImageData.c
	ImageData.h
	main.c
)

target_link_libraries(waveshare_display
	PRIVATE
		wiringPi::wiringPi
		plugin_interface
)
set_target_properties(waveshare_display PROPERTIES
	PREFIX ""
	SUFFIX ".mod"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
