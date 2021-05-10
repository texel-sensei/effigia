#ifndef EFFIGIA_DISPLAY_MODULE_H
#define EFFIGIA_DISPLAY_MODULE_H

/**
 * \file DisplayModule.h
 *
 * Header that defines the functions a display module needs to support.
 */

#include <stddef.h>

#include "Event.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef unsigned int Color;

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
	int query_color_palette(void* display, Color* colors);

	int clear(void* display);

	/**
	 * Sets the color for a single pixel on the display. Pixel coordinates
	 * start with (0,0) in the upper left corner. Changes do not need to be
	 * visible until \ref present() is called.
	 *
	 * \param color Color for the given pixel in RGB format. If the display
	 * 				uses indexed color mode, then this is guaranteed to be
	 * 				one of the colors given by the palette (*not* the index).
	 */
	int set_pixel(void* display, int x, int y, Color color);

	/**
	 * Present the final image. This is called after all pixels have been
	 * submitted via \ref set_pixel().
	 */
	int present(void* display);

	/**
	 * Poll for new events signaled by the display. This function is called
	 * regularly by the main program and if any events have happened, then this
	 * function returns a pointer to an event struct with information about the
	 * event. Any non-NULL pointers that this function returns must be freed with
	 * \ref free_event().
	 *
	 * If no events did happen, of the display does not support event handling,
	 * then this function returns NULL.
	 */
	DisplayEvent* poll_events(void* display);

	/**
	 * Free the memory from a given \ref DisplayEvent. Freeing NULL is safe.
	 * This function must only be called for events received via \ref poll_events().
	 */
	void free_event(DisplayEvent*);


#ifdef __cplusplus
}
#endif

#endif
