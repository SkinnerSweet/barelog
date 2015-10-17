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
 * @file barelog_buffer.h
 * @brief Module defining the different buffers used by barelog's internals.
 *
 * This header defines the different types of buffer used by barelog.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_BUFFER__
#define __BARELOG_BUFFER__

#include <stdint.h>

#include "barelog_internal.h"
#include "barelog_event.h"

/**
 * Queue of events, used to store the local events into a core
 * local memory.
 */
typedef struct {
	/** buffer containing the events (queue) */
	barelog_event_t buffer[BARELOG_EVENT_PER_CORE_MAX];
	/** index of the next position to store an event */
	uint32_t head;
	/** index of the first position effectively used */
	uint32_t tail;
	/** indicates whether or not the buffer is full */
	uint8_t full;
	/** indicates whether or not the buffer is empty */
	uint8_t empty;
} barelog_event_buffer_t;

/**
 * Structure used to store the events of a logged core, represented
 * by strings and not actual events (for display or treatment purposes).
 */
typedef struct barelog_result_buffer_t_ {
	/** buffer of events (considered as strings) */
	char **buffer;
	/** number of events to consider */
	size_t buffer_length;
	/** length of each event */
	size_t sub_buffer_length;
} barelog_result_buffer_t;

/**
 * Structure used to store the events in the shared memory.
 */
typedef struct {
	/** events queue */
	barelog_event_t *events;
	/** current index inside the queue */
	uint32_t index;
	/** max index */
	uint32_t imax;
} barelog_shared_mem_buffer_t;

#endif /* __BARELOG_BUFFER__*/
