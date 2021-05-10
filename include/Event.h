#ifndef EFFIGIA_EVENT_H
#define EFFIGIA_EVENT_H

/**
 * \file include/Event.h
 *
 * This file contains the definition for all the events that can be sent to the
 * main program.
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * ID type used to differentiate events.
 */
typedef unsigned int EventID;

/**
 * A shutdown of the application is requested. On response to this event the
 * main program will terminate.
 */
#define EFFIGIA_EVENT_SHUTDOWN_REQUESTED 1

/**
 * A new image was requested to be displayed. This event uses the `new_image`
 * field of the \ref EventData struct to include the filename of the new image
 * that should be displayed.
 */
#define EFFIGIA_EVENT_NEW_IMAGE_REQUESTED 2

/**
 * \brief The properties of the display have changed (e.g. the display window
 * was resized).
 * The main program will rerun \ref query_display_properties() and refresh the
 * image with the new properties.
 */
#define EFFIGIA_EVENT_DISPLAY_PROPERTIES_CHANGED 3

typedef struct DisplayEvent {
	EventID id;
	union EventData {
		struct { char* filename; } new_image;
	} data;
} DisplayEvent;

#ifdef __cplusplus
}
#endif

#endif
