#include "DisplayModule.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

struct EmulatedDisplay {
	DisplayProperties properties;
	std::vector<Color> palette;
};

const EmulatedDisplay waveshare = {
	{ 384, 640, ColorMode::indexed, 3 },
    {0x000000, 0xFFFFFF, 0xFF0000}
};


class Display {
public:
	Display() {
		int w = 600;
		int h = 800;

		auto* emulate_id = getenv("EMULATE_DISPLAY");
		if(emulate_id && strcmp(emulate_id, "Waveshare") == 0) {
			cout << "Emulating waveshare display!" << endl;
			emulation = &waveshare;
		}

		if(emulation) {
			w = emulation->properties.width;
			h = emulation->properties.height;
		}

		if(SDL_Init(SDL_INIT_VIDEO) != 0) {
			throw runtime_error{"Failed to init SDL!"};
		}
		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
		if(!window) {
			throw runtime_error{"Failed to create window!"};
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		surface = SDL_CreateRGBSurface(0, w, h, 32, 0x00FF0000, 0x0000FF00, 0x000000FF,0xFF000000);
		if(!surface) {
			cerr << "Failed to create surface!" << endl;
			throw runtime_error{"Failed to create surface!"};
		}
	}
	~Display() {
		SDL_FreeSurface(surface);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void clear() {
		SDL_RenderClear(renderer);
	}

	void present() {
		auto texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
		SDL_RenderCopy(renderer, texture, nullptr, nullptr);
		SDL_RenderPresent(renderer);
		SDL_DestroyTexture(texture);
	}

	int set_pixel(int x, int y, int color) {
		if(x < 0 || x >= surface->w || y < 0 || y >= surface->h) {
			return -1;
		}

		static_cast<uint32_t*>(surface->pixels)[y*surface->w+x] = color;
		return 0;
	}

	int query_properties(DisplayProperties* properties) {
		if(emulation) {
			*properties = emulation->properties;
			return 0;
		}
		properties->width = surface->w;
		properties->height = surface->h;
		properties->mode = ColorMode::rgb;
		properties->color_depth = 8;
		return 0;
	}

	int query_color_palette(Color* colors) {
		if(!emulation || emulation->properties.mode != ColorMode::indexed) {
			return -1;
		}
		copy(emulation->palette.begin(), emulation->palette.end(), colors);
		return 0;
	}

private:
	EmulatedDisplay const* emulation = nullptr;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Surface* surface;
};

#ifdef __cplusplus
extern "C" {
#endif

	void* initialize_display() {
		try{
			return new Display{};
		} catch(std::exception const& e) {
			std::cerr << e.what() << std::endl;
		} catch(...) {
			/* ignore */
		}
		return nullptr;
	}

	void destroy_display(void* display) {
		delete static_cast<Display*>(display);
	}

	int query_display_properties(void* display, DisplayProperties* properties) {
		auto d = static_cast<Display*>(display);
		return d->query_properties(properties);
	}

	int query_color_palette(void* display, Color* colors) {
		auto d = static_cast<Display*>(display);
		return d->query_color_palette(colors);
	}

	int clear(void* display) {
		auto d = static_cast<Display*>(display);
		d->clear();
		return 0;
	}
	int set_pixel(void* display, int x, int y, Color color) {
		auto d = static_cast<Display*>(display);
		return d->set_pixel(x, y, color);
	}
	int present(void* display) {
		auto d = static_cast<Display*>(display);
		d->present();
		return -1;
	}


#ifdef __cplusplus
}
#endif
