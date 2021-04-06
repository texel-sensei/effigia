#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <dlfcn.h>
#include <memory>
#include <iostream>
#include <stdexcept>

using namespace std;

struct dlopen_deleter {
	void operator()(void* handle) {
		dlclose(handle);
	}
};
using dl_ptr = std::unique_ptr<void, dlopen_deleter>;

void noop(void*) {}

class Plugin {
	using initialize_fp = void*(*)();
	using destroy_fp = void(*)(void*);
	using clear_fp = int(*)(void*);
	using set_pixel_fp = int(*)(void*, int, int, int);
	using display_fp = int(*)(void*);

	using display_ptr = std::unique_ptr<void, destroy_fp>;
public:
	Plugin(const char* name):
		plugin_library{dlopen(name, RTLD_NOW)} {
		if(!plugin_library) {
			std::cerr << dlerror() << std::endl;
			throw std::runtime_error{"Failed to open library!"};
		}
		initialize_display = load_function<initialize_fp>("initialize_display");
		destroy_display = load_function<destroy_fp>("destroy_display");
		clear_display = load_function<clear_fp>("clear");
		set_pixel_display = load_function<set_pixel_fp>("set_pixel");
		display_display = load_function<display_fp>("display");

		auto* d = initialize_display();
		if(!d) {
			throw runtime_error{"Failed to initialize display!"};
		}
		display = {d, destroy_display};
	}

	void clear() {
		clear_display(display.get());
	}

	void set_pixel(int x, int y, int color) {
		set_pixel_display(display.get(), x, y, color);
	}

	void display_() {
		display_display(display.get());
	}

private:
	dl_ptr plugin_library;
	initialize_fp initialize_display = {};
	destroy_fp destroy_display = {};
	clear_fp clear_display = {};
	set_pixel_fp set_pixel_display = {};
	display_fp display_display = {};

	display_ptr display = {nullptr, noop};

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

int main(int argc, char* argv[]) {
	if(argc != 2) {
		return 1;
	}
	Plugin sdl_plugin{"plugins/libsdl_display.so"};
	std::cout << "successfully loaded plugin" << std::endl;

	auto* file = argv[1];
	std::cout << "Loading image " << file << std::endl;
	int x, y, n;
	unsigned char* data = stbi_load(file, &x, &y, &n, 4);
	if(!data) {
		std::cout << "Can't load image!" << std::endl;
		return 1;
	}
	sdl_plugin.clear();
	for(int line = 0; line < y; ++line) {
		for(int col = 0; col < x; ++col) {
			auto pixel = (line * x + col)*4;
			uint8_t rgb[3];
			for(int channel = 0; channel < 3; ++channel) {
				rgb[channel] = data[pixel + channel];
			}
			auto color = (rgb[0] << 16) | (rgb[1] << 8) | rgb[2];
			sdl_plugin.set_pixel(col, line, color);
		}
	}
	stbi_image_free(data);
	sdl_plugin.display_();
	int c;
	std::cin >> c;
}
