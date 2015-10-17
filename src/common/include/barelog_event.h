/* The MIT License (MIT)

 Copyright (c) 2015 Thomas Bertauld <thomas.bertauld@gmail.com>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*/

/**
 * @file barelog_event.h
 * @brief Module defining the events and their related functions.
 *
 * This header defines the main structure of an event as seen by
 * every other barelog files. It also defines some common functions
 * to manipulate those events.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_EVENT__
#define __BARELOG_EVENT__

#include <stdint.h>
#include <stddef.h>

#include "barelog_internal.h"

typedef struct barelog_result_buffer_t_ barelog_result_buffer_t;

/** Maximum size (in bytes) of a formatted string containing all
 * barelog event information.
 */
#define EVENT_TO_STRING_SIZE BARELOG_EVENT_MAX_SIZE*2

/**
 * Main structure of what we call an event.
 */
typedef struct __attribute__((packed)) {
	/** timestamp of the event */
	uint32_t timestamp;
	/** core on which the event occured */
	uint32_t core;
	/** actual data contained by the event */
	char data[BARELOG_BUF_MAX_SIZE];
} barelog_event_t;

/**
 * Event initializer, every field is set to 0 except for data, set to "".
 */
extern const barelog_event_t BARELOG_EVENT_INITIALIZER;

/**
 * Converts an event structure into a single string.
 * @param event event to convert.
 * @param buffer buffer to use for the conversion (should be
 * at least EVENT_TO_STRING_SIZE bytes long).
 * @return the return code of snprintf().
 */
extern int8_t barelog_event_to_string(const barelog_event_t event, char *buffer);

/**
 * Converts an events queue into a buffer of strings.
 * @param events events queue to convert.
 * @param n size of the events queue.
 * @param buffer result buffer.
 * @return the BARELOG_SUCCESS if everything went well, an error code otherwise.
 */
extern int8_t barelog_events_to_strings(const barelog_event_t *events, size_t n, barelog_result_buffer_t *buffer);

#endif /* __BARELOG_EVENT__ */
