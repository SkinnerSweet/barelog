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
 * @file barelog_logger.h
 * @brief Module providing some nice wrapping for the device_mem_manager.
 *
 * Only this module should be used by the target program.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_LOGGER__
#define __BARELOG_LOGGER__

#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

#include "barelog_platform.h"
#include "barelog_policy.h"
#include "barelog_device_mem_manager.h"

/**
 * Structure used to hold all of the barelog logger functions.
 * We use pointers to allow the user to use the functions of their choice,
 * depending on the logged platform.
 */
typedef struct {
	/** Function used to retrieve the current clock of the core.
	 * @return a timestamp on 32 bits.
	 */
	uint32_t (*get_clock)(void);
	/** Function used to initialize (reset) the current clock of the core.
	 * @return BARELOG_SUCCESS on success, an error code otherwise.
	 */
	int8_t (*init_clock)(void);
	/** Function used to start the current clock of the core.
	 * @return BARELOG_SUCCESS on success, an error code otherwise.
	 */
	int8_t (*start_clock)(void);
} barelog_logger_t;

/**
 * Initializes the logger. Should be called
 * before any subsequent call to any other function in this module.
 * @param my_core index of the core to log.
 * @param platform the platform to allocate the (shared) memory spaces against.
 * @param buffer_policy policy to apply when the events buffer is full.
 * @param memory_policy policy to apply when the memory buffer is full.
 * @param read the function used by the target to read data from a memory section.
 * @param write the function used by the target to write data into a memory section.
 * @param get_clock the function used to retrieve timestamps.
 * @param init_clock the function used to initialize the target's clock.
 * @param start_clock the function used to start the target's clock.
 * @return BARELOG_SUCCESS on success.
 */
extern int8_t barelog_init_logger(const uint32_t my_core,
		const barelog_platform_t platform,
		const barelog_policy_t buffer_policy,
		const barelog_policy_t memory_policy,
		int8_t (*read)(const void * address, size_t size,
				void *buffer),
		int8_t (*write)(void * address, size_t size,
				const void *buffer),
		uint32_t (*get_clock)(void),
		int8_t (*init_clock)(void),
		int8_t (*start_clock)(void)) __attribute__ ((cold));

/**
 * Starts the logging engine. Should be called
 * before any subsequent call to the barelog_log() function.
 * @return BARELOG_SUCCESS on success, or an error code if something went wrong.
 */
extern int8_t barelog_start(void) __attribute__ ((cold));

/**
 * The logging function, follows the same format than printf().
 * If a real and functional get_clock() function was given upon initialization,
 * it will be used to automatically add a timestamp to the created event
 * containing the message.
 */
extern int8_t barelog_log(const char *format, ...) __attribute__ ((hot));

/**
 * @see device_mem_manager_clean_buffer
 */
#define barelog_clean_buffer() device_mem_manager_clean_buffer()

/**
 * @see device_mem_manager_clean
 */
#define barelog_clean(n) device_mem_manager_clean(n)

/**
 * @see device_mem_manager_flush_buffer
 */
#define barelog_flush_buffer() device_mem_manager_flush_buffer()

/**
 * @see device_mem_manager_flush
 */
#define barelog_flush(n) device_mem_manager_flush(n)

/**
 * @see device_mem_manager_is_buffer_full
 */
#define barelog_is_buffer_full() device_mem_manager_is_buffer_full()

/**
 * @see device_mem_manager_clean_memory
 */
#define barelog_clean_memory() device_mem_manager_clean_memory()

#endif /* __BARELOG_LOGGER__ */
