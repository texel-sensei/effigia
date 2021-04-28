set(modules sdl_display waveshare_7in5)

foreach(module ${modules})
	message(STATUS "Trying to compile ${module}")
	try_compile(RESULT_VAR
		${CMAKE_BINARY_DIR}/${module}
		${CMAKE_CURRENT_LIST_DIR}/${module}
		${module}
	)
	if(RESULT_VAR)
		message(STATUS "Adding module ${module} to project!")
		add_subdirectory(${module})
	else()
		message(STATUS "Module ${module} failed to compile, ignoring for build")
	endif()
endforeach()
