cmake_minimum_required(VERSION 3.7)
project(pictureframe)

add_library(plugin_interface INTERFACE include/DisplayModule.h)
target_include_directories(plugin_interface INTERFACE include/)

add_subdirectory(src)
