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

typedef enum Color {
	black = 0, white, red
} Color;

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

	return display;
}

void* destroy_display(void* display) {
    EPD_Sleep();
	DEV_ModuleExit();
	free_display((WaveshareDisplay*)display);
}

int query_display_properties(void* display, DisplayProperties* properties) {
	properties->width = EPD_WIDTH;
	properties->height = EPD_HEIGHT;
	properties->mode = indexed;
	properties->color_depth = 3;
	return 0;
}

int query_color_palette(void* display, int* colors) {
	colors[0] = 0x000000;
	colors[1] = 0xFFFFFF;
	colors[2] = 0xFF0000;
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

int set_pixel(void* user_data, int x, int y, int color) {
	WaveshareDisplay* display = (WaveshareDisplay*)user_data;
	//TODO(texel, 2021-03-30): define how the color parameter works

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

void Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Goto Sleep mode\r\n");
    EPD_Sleep();
    DEV_ModuleExit();

    exit(0);
}

int main(int argc, char* argv[])
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);

    if(EPD_Init() != 0) {
        printf("e-Paper init failed\r\n");
    }
    printf("clear...\r\n");

    // Create a new image cache named IMAGE_BW and fill it with white
    UBYTE *BlackImage, *RedImage;
    UWORD Imagesize = ((EPD_WIDTH % 8 == 0)? (EPD_WIDTH / 8 ): (EPD_WIDTH / 8 + 1)) * EPD_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        exit(0);
    }
    if((RedImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        exit(0);
    }
    printf("NewImage:BlackImage and RedImage\r\n");
    Paint_NewImage(BlackImage, EPD_WIDTH, EPD_HEIGHT, 0, WHITE);
    Paint_NewImage(RedImage, EPD_WIDTH, EPD_HEIGHT, 0, WHITE);

    //Select Image
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);

	FILE* source_folder_file = fopen(GALLERY_FILE, "r");
	if(source_folder_file == NULL){
		printf("Failed to open gallery file %s\n", GALLERY_FILE);
		exit(1);
	}

	char* folder = NULL;
	size_t len = 0;
	// Read full file
	ssize_t bytes_read = getline(&folder, &len, source_folder_file);
	if(bytes_read == -1) {
		printf("Failed to read gallery file %s\n", GALLERY_FILE);
		exit(1);
	}
	len = bytes_read;
	fclose(source_folder_file);

	if (folder[len-1] == '\n')
		folder[len-1] = '\0'; // remove newline in foldername

	const char* format = "%s/%s-%c.bmp";
	char* name = argv[1];
	int size = snprintf(NULL, 0, format, folder, name, 'r') + 1;
	char* path = malloc(size);

    printf("show bmp------------------------\r\n");
    printf("read black bmp\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
	snprintf(path, size, format, folder, name, 'b');
	printf("reading image file %s\n", path);
    GUI_ReadBmp(path, 0, 0);

    printf("read red bmp\r\n");
    Paint_SelectImage(RedImage);
    Paint_Clear(WHITE);
	snprintf(path, size, format, folder, name, 'r');
    GUI_ReadBmp(path, 0, 0);

	free(path);
	free(folder);

    EPD_Display(BlackImage, RedImage);

    printf("Goto Sleep mode...\r\n");
    EPD_Sleep();
    free(BlackImage);
    BlackImage = NULL;
    free(RedImage);
    RedImage = NULL;

    return 0;
}
