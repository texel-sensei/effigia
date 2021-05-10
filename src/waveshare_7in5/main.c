#include <stdlib.h>     //exit()
#include <stdio.h>
#include <signal.h>     //signal()
#include <time.h>
#ifdef DEBUG
#	undef DEBUG
#endif
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "EPD_7in5b.h"

#include "DisplayModule.h"

typedef struct WaveshareDisplay {
	UBYTE* red;
	UBYTE* black;
} WaveshareDisplay;

typedef enum ColorName {
	black = 0, white, red
} ColorName;

void Handler(int signo);

static WaveshareDisplay* alloc_display() {
	WaveshareDisplay* display = malloc(sizeof(WaveshareDisplay));
	if (!display) return NULL;

    UWORD imagesize = ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
	display->red = malloc(imagesize);
	display->black = malloc(imagesize);

	if(!display->red || !display->black) {
		free(display->red);
		free(display->black);
		free(display);
		return NULL;
	}
	return display;
}

static void free_display(WaveshareDisplay* display) {
	if(!display) return;

	free(display->red);
	free(display->black);
	free(display);
}

void* initialize_display() {
    if(DEV_ModuleInit() != 0) {
		return NULL;
	}
	if(EPD_Init() != 0) {
		DEV_ModuleExit();
		return NULL;
	}

	WaveshareDisplay* display = alloc_display();
	if(!display) return NULL;

    Paint_NewImage(display->red, EPD_WIDTH, EPD_HEIGHT, 0, WHITE);
    Paint_NewImage(display->black, EPD_WIDTH, EPD_HEIGHT, 0, WHITE);

    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

	return display;
}

void destroy_display(void* display) {
    EPD_Sleep();
	DEV_ModuleExit();
	free_display((WaveshareDisplay*)display);
}

int query_display_properties(void* display, DisplayProperties* properties) {
	// Transpose width/height, as we have the display in potrait
	properties->width = EPD_HEIGHT;
	properties->height = EPD_WIDTH;
	properties->mode = indexed;
	properties->color_depth = 3;
	return 0;
}

int query_color_palette(void* display, Color* colors) {
	// colors measured using phone camera
	colors[0] = 0x100409; // black
	colors[1] = 0xF2E0CE; // white
	colors[2] = 0xD1281A; // red
	return 0;
}

int clear(void* user_data) {
	WaveshareDisplay* display = (WaveshareDisplay*)user_data;

    Paint_SelectImage(display->black);
    Paint_Clear(WHITE);
    Paint_SelectImage(display->red);
    Paint_Clear(WHITE);

	return 0;
}

int set_pixel(void* user_data, int x, int y, Color color) {
	WaveshareDisplay* display = (WaveshareDisplay*)user_data;

	// Transpose width/height, as we have the display in potrait
	int tmp = x;
	x = EPD_WIDTH - y;
	y = tmp;

	// clear that pixel to white
	Paint_SelectImage(display->black);
	Paint_SetPixel(x, y, WHITE);
	Paint_SelectImage(display->red);
	Paint_SetPixel(x, y, WHITE);

	switch(color) {
		case black:
			Paint_SelectImage(display->black);
			Paint_SetPixel(x, y, BLACK);
			break;
		case white:
			// nothing to do
			break;
		case red:
			Paint_SelectImage(display->red);
			Paint_SetPixel(x, y, BLACK);
			break;
		default:
			return 1;
	}
	return 0;
}

int present(void* user_data) {
	WaveshareDisplay* display = (WaveshareDisplay*)user_data;
    EPD_Display(display->black, display->red);
    EPD_Sleep();
	return 0;
}

// Signal a single stop event. Further calls return NULL, to avoid
// looping infinitely in the event loop (as first all pending events
// are handled and then the loop is exited after a stop).
DisplayEvent* poll_events(void* display){
	static int first_call = 1;
	if(!first_call) return NULL;
	first_call = 0;

	static DisplayEvent always_quit;
	always_quit.id = EFFIGIA_EVENT_SHUTDOWN_REQUESTED;
	return &always_quit;
}

void free_event(DisplayEvent* event){}

void Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Goto Sleep mode\r\n");
    EPD_Sleep();
    DEV_ModuleExit();

    exit(0);
}
