#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <dlfcn.h>
#include <memory>
#include <iostream>
#include <stdexcept>

#include "plugin.h"

using namespace std;

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
	sdl_plugin.display();
	int c;
	std::cin >> c;
}
