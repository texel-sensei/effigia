add_subdirectory(sdl_display)

# We need to link with pthreads, because of ... something related to opengl
# Not doing it causes random __gnu_cxx::recursive_init_error exceptions to happen,
# both when trying to initialize SDL or GLFW. Apparently it is related to dynamic
# loading of GL, mesa and some threading shenanigans.
# See this link for more info: https://gitlab.freedesktop.org/mesa/mesa/-/issues/3199
find_package(Threads REQUIRED)
add_executable(gallery
	main.cpp
	stb_image.h
	plugin.h
)
target_link_libraries(gallery PRIVATE
	plugin_interface
	-ldl Threads::Threads
)
