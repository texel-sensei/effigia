cmake_minimum_required(VERSION 3.7)
project(waveshare_display LANGUAGES C)

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

target_link_libraries(waveshare_display PRIVATE -lwiringPi)
target_include_directories(waveshare_display PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../../include)
set_target_properties(waveshare_display PROPERTIES
	PREFIX ""
	SUFFIX ".mod"
	LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)
