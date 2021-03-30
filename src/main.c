#include <stdlib.h>     //exit()
#include <stdio.h>
#include <signal.h>     //signal()
#include <time.h>
#ifdef DEBUG
#	undef DEBUG
#endif
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "ImageData.h"
#include "EPD_7in5b.h"

#define GALLERY_FILE "gallery.txt"

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:Goto Sleep mode\r\n");
    EPD_Sleep();
    DEV_ModuleExit();

    exit(0);
}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		printf("usage: %s <name>\n", argv[0]);
		exit(1);
	}

    printf("7.5inch e-Paper B(C) demo\r\n");
    DEV_ModuleInit();

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
