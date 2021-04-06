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
		window = SDL_CreateWindow("", 100, 100, 600, 800, 0);
		if(!window) {
			throw runtime_error{"Failed to create window!"};
		}
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE | SDL_RENDERER_PRESENTVSYNC);
	}
	~Display() {
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

private:
	SDL_Window* window;
	SDL_Renderer* renderer;
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
		return -1;
	}
	int set_pixel(void* display, int x, int y, int color) {
		return -1;
	}
	int display(void* display) {
		return -1;
	}


#ifdef __cplusplus
}
#endif
