#pragma once

/**
 * \file src/plugin.h
 *
 * Provides a class to hold the required data for a display plugin loaded from a
 * shared object.
 */

#include <dlfcn.h>
#include <memory>

#include "DisplayModule.h"

namespace detail {
	struct dlopen_deleter {
		void operator()(void* handle) {
			dlclose(handle);
		}
	};
	using dl_ptr = std::unique_ptr<void, dlopen_deleter>;

	inline void noop(void*) {}
}

#define PUBLIC_FUNCTION_LIST(X)\
	X(query_display_properties, int(*)(void*, DisplayProperties*))\
	X(query_color_palette, int(*)(void*, int*))\
	X(clear, int(*)(void*))\
	X(set_pixel, int(*)(void*, int, int, int))\
	X(display, int(*)(void*))

#define FUNCTION_LIST(X)\
	X(initialize_display, void*(*)())\
	X(destroy_display, void(*)(void*))\
	PUBLIC_FUNCTION_LIST(X)


class Plugin {
	// create typedefs for the function pointers
#define X(name, type) using name##_fp = type;
	FUNCTION_LIST(X)
#undef X

	using plugin_ptr = std::unique_ptr<void, destroy_display_fp>;
public:
	explicit Plugin(const char* name):
		plugin_library{dlopen(name, RTLD_NOW)} {
		if(!plugin_library) {
			std::cerr << dlerror() << std::endl;
			throw std::runtime_error{"Failed to open library!"};
		}
#define X(name, type) name##_ptr = load_function<name##_fp>(#name);
		FUNCTION_LIST(X)
#undef X

		auto* d = initialize_display_ptr();
		if(!d) {
			throw std::runtime_error{"Failed to initialize display!"};
		}
		loaded_display = {d, destroy_display_ptr};
	}

#define X(name, type) \
	template<typename... Args>\
	auto name(Args... args) {\
		return name##_ptr(loaded_display.get(), args...);\
	}
PUBLIC_FUNCTION_LIST(X)
#undef X

private:
	detail::dl_ptr plugin_library;

	// create list of function pointers
#define X(name, type) name##_fp name##_ptr = {};
	FUNCTION_LIST(X)
#undef X

	plugin_ptr loaded_display = {nullptr, detail::noop};

	template<typename F>
	F load_function(const char* name) {
		F ptr = reinterpret_cast<F>(dlsym(plugin_library.get(), name));
		if(!ptr){
			std::cerr << dlerror() << std::endl;
			throw std::runtime_error{"Failed to lookup function!"};
		}
		return ptr;
	}

};

#undef FUNCTION_LIST
#undef PUBLIC_FUNCTION_LIST
