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

#include <string.h>
#include <stdlib.h>

#include "barelog_logger.h"
#include "barelog_event.h"
#include "barelog_internal.h"
#include "include/barelog_snprintf.h"

static barelog_logger_t logger;

static uint32_t default_get_clock(void) {
	return 0;
}

int8_t barelog_init_logger(const uint32_t my_core,
		const barelog_platform_t platform,
		const barelog_policy_t buffer_policy,
		const barelog_policy_t memory_policy,
		int8_t (*read)(const void * address, size_t size, void *buffer),
		int8_t (*write)(void * address, size_t size, const void *buffer),
		uint32_t (*my_get_clock)(void),
		int8_t (*my_init_clock)(void),
		int8_t (*my_start_clock)(void)) {

	device_mem_manager_init(my_core, platform, buffer_policy, memory_policy, read, write);

	logger.get_clock = my_get_clock;
	logger.init_clock = my_init_clock;
	logger.start_clock = my_start_clock;
	logger.log_lvl = BARELOG_DEFAULT_LOG_LVL;

	if (!logger.get_clock) {
		logger.get_clock = default_get_clock;
	}

	return BARELOG_SUCCESS;
}

int8_t barelog_start(void) {
	int8_t ret = 0;
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (!logger.init_clock || !logger.start_clock) {
		ret = BARELOG_UNINITIALIZED_PARAM_ERR;
		BARELOG_DEBUG(__FILE__, __LINE__, ret, "barelog_start param");
		return ret;
	}
#endif
	ret = logger.init_clock();
	return (ret + logger.start_clock());
}

int8_t barelog_log(barelog_lvl_t lvl, const char *format, ...) {

	if (lvl > logger.log_lvl) {
		return -1;
	}

	barelog_event_t event = BARELOG_EVENT_INITIALIZER;

	event.timestamp = logger.get_clock();

	va_list ap;
	va_start(ap, format);
	portable_vsnprintf(event.data, BARELOG_BUF_MAX_SIZE, format, ap);
	//vsnprintf(event.data, BARELOG_BUF_MAX_SIZE, format, ap);

	va_end(ap);

	return device_mem_manager_write_buffer(event);

}

int8_t barelog_immediate_log(barelog_lvl_t lvl, const char *format, ...) {
	if (lvl > logger.log_lvl) {
		return -1;
	}
	int8_t ret = 0;
	va_list ap;
	va_start(ap, format);
	ret += barelog_log(lvl, format, ap);
	ret += barelog_flush(1);
	ret += barelog_clean(1);
	va_end(ap);

	return ret;
}

void barelog_set_log_lvl(barelog_lvl_t lvl) {
	logger.log_lvl = lvl;
}

barelog_lvl_t barelog_get_log_lvl(void) {
	return logger.log_lvl;
}
