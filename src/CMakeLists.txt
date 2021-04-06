add_subdirectory(sdl_display)

add_executable(gallery main.cpp)
target_link_libraries(gallery PRIVATE -ldl)
