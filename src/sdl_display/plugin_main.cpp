#include "DisplayModule.h"

#include <SDL2/SDL.h>

#include <iostream>
#include <stdexcept>

using namespace std;


class Display {
public:
	Display() {
		if(SDL_Init(SDL_INIT_VIDEO) != 0) {
			throw runtime_error{"Failed to init SDL!"};
		}
		window = SDL_CreateWindow("", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 800, SDL_WINDOW_SHOWN);
		if(!window) {
			throw runtime_error{"Failed to create window!"};
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		surface = SDL_CreateRGBSurface(0, 750, 1000, 32, 0x00FF0000, 0x0000FF00, 0x000000FF,0xFF000000);
		if(!surface) {
			cerr << "Failed to create surface!" << endl;
//			throw runtime_error{"Failed to create surface!"};
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

//private:
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
		return -1;
	}

	int query_color_palette(void* display, int* colors) {
		return -1;
	}

	int clear(void* display) {
		auto d = static_cast<Display*>(display);
		d->clear();
		return 0;
	}
	int set_pixel(void* display, int x, int y, int color) {
		auto d = static_cast<Display*>(display);

		static_cast<uint32_t*>(d->surface->pixels)[y*d->surface->w+x] = color;

		return -1;
	}
	int display(void* display) {
		auto d = static_cast<Display*>(display);
		d->present();
		return -1;
	}


#ifdef __cplusplus
}
#endif
