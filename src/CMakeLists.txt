set(modules sdl_display waveshare_7in5)

foreach(module ${modules})
	set(SKIP_MODULE FALSE)

	add_subdirectory(${module})

	if(NOT SKIP_MODULE)
		message(STATUS "Adding module ${module} to project!")
	else()
		message(STATUS "Module ${module} failed to compile, ignoring for build")
	endif()
endforeach()
