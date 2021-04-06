add_subdirectory(sdl_display)

add_executable(gallery main.cpp stb_image.h)
target_link_libraries(gallery PRIVATE -ldl)
