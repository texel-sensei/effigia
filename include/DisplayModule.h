#pragma once

/**
 * \file DisplayModule.h
 *
 * Header that defines the functions a display module needs to support.
 */

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum ColorMode {
		indexed,
		rgb,
		grayscale
	} ColorMode;

	typedef struct DisplayProperties {
		int width;
		int height;
		ColorMode mode;

		/**
		 * The semantic of color depth depends on the color mode:
		 * 	indexed -> number of supported colors
		 * 	grayscale -> bit depth
		 * 	rgb -> bit depth per channel
		 */
		int color_depth;
	} DisplayProperties;

	/**
	 * Do necessary setup work required for the display to function. The
	 * returned pointer is passed to all other display functions.
	 *
	 * \returns A pointer to an arbitrary display struct. Return NULL to
	 * indicate that setup failed.
	 */
	void* initialize_display();
	/**
	 * Tear down a display and free all related resources.
	 */
	void destroy_display(void* display);

	/**
	 * Query information about the display.
	 *
	 * \param properties Output parameter that will contain all display
	 * properties.
	 * \return 0 if operation was successful, non-zero otherwise.
	 */
	int query_display_properties(void* display, DisplayProperties* properties);

	/**
	 * Query the supported color palette. Can only be called for displays that
	 * report ColorMode indexed.
	 *
	 * \param colors An array of size \ref DisplayProperties.color_depth. This
	 *    array will be filled with the supported color palette. Each entry is
	 *    in the form 0x00RRGGBB.
	 */
	int query_color_palette(void* display, int* colors);

	int clear(void* display);
	int set_pixel(void* display, int x, int y, int color);
	int display(void* display);


#ifdef __cplusplus
}
#endif
