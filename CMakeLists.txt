cmake_minimum_required(VERSION 3.7)
project(pictureframe)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

add_library(plugin_interface INTERFACE)
target_include_directories(plugin_interface INTERFACE include/)

add_subdirectory(src)
