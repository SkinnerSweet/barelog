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

#include "barelog_event.h"
#include "barelog_internal.h"
#include "barelog_buffer.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

const barelog_event_t BARELOG_EVENT_INITIALIZER = {
	.timestamp = 0,
	.core = 0,
	.data = ""
};

int8_t barelog_event_to_string(const barelog_event_t event, char *buffer) {
#if BARELOG_CHECK_MODE
	if (strlen(event.data) > EVENT_TO_STRING_SIZE) {
		return BARELOG_INCONSISTENT_PARAM_ERR;
	}
#endif

	return snprintf(buffer, EVENT_TO_STRING_SIZE, "%"PRIu32" %"PRIu32" %s",
			event.timestamp, event.core, event.data);
}

// On suppose que tous les sous-buffers (buffer[i]) font la meme taille => pourrait devenir dynamique avec parametre strlen(event.data)
int8_t barelog_events_to_strings(const barelog_event_t *events, size_t n, barelog_result_buffer_t *res) {
#if BARELOG_CHECK_MODE
	if (n < 0) {
		return BARELOG_INCONSISTENT_PARAM_ERR;
	}
	if (!res) {
		return BARELOG_UNINITIALIZED_PARAM_ERR;
	}
#endif

	res->buffer = calloc(n, sizeof(char *));

#if BARELOG_CHECK_MODE
	if (res->buffer == NULL) {
		return BARELOG_ERR;
	}
#endif
	res->buffer_length = n;
	res->sub_buffer_length = EVENT_TO_STRING_SIZE;

	int8_t ret = 0;
	(void) ret;
	for (uint32_t i = 0; i < n; ++i) {
		res->buffer[i] = (char *)calloc(EVENT_TO_STRING_SIZE, sizeof(char));
		ret = barelog_event_to_string(events[i], res->buffer[i]);
#if BARELOG_CHECK_MODE
		if (ret != BARELOG_SUCCESS) {
			return BARELOG_EVENT_CONVERSION_ERR;
		}
#endif
	}

	return ret;
}

