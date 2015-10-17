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
 * @file barelog_device_mem_manager.h
 * @brief Module defining all functions offered by barelog for the host program.
 *
 * This header defines the functions structures and functions used to initialize
 * and finalize the host part of the logger and to read the events inside
 * the shared memory once the logging session is over.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_DEVICE_MEM_MANAGER__
#define __BARELOG_DEVICE_MEM_MANAGER__

#include <stdint.h>
#include <string.h>

#include "barelog_buffer.h"
#include "barelog_event.h"
#include "barelog_policy.h"
#include "barelog_platform.h"

/**
 * Structure used to hold all of the barelog device manager functions.
 * We use pointers to allow the user to use the functions of their choice,
 * depending on the logged platform.
 */
typedef struct {
	/** Function used by the target to read into the shared memory.
	 * @param address the address to read.
	 * @param size the size of the memory to read.
	 * @param buffer the buffer in which to store the reading result.
	 * @return BARELOG_SUCCESS if all is clear, an error code otherwise.
	 */
	int8_t (*read)(const void * address, size_t size, void *buffer);
	/** Function used by the target to write into the shared memory.
	 * @param address the address to write.
	 * @param size the size of the memory to write.
	 * @param buffer the buffer from which to write the reading result.
	 * @return BARELOG_SUCCESS if all is clear, an error code otherwise.
	 */
	int8_t (*write)(void * address, size_t size, const void *buffer);
	/* policy to apply on the local events buffer */
	barelog_policy_t buffer_policy;
	/* policy to apply on the shared memory events buffer */
	barelog_policy_t memory_policy;
} barelog_device_mem_manager_t;

/**
 * Defines and initializes the device memory manager. Should be called
 * before any subsequent call to any other function in this module.
 * @param core index of the core to initialize.
 * @param platform platform used to log (the device memory manager
 * will be created against this platform information).
 * @param buffer_policy policy to use when the events buffer is full.
 * @param memory_policy policy to use when the shared memory buffer is full.
 * @param read function used by device to read in shared memory.
 * @param write function used by device to write in shared memory.
 * @return BARELOG_NB_CORES on success, an error code in case of exception.
 **/
extern int8_t device_mem_manager_init(const uint32_t core,
		const barelog_platform_t platform,
		const barelog_policy_t buffer_policy,
		const barelog_policy_t memory_policy,
		int8_t (*read)(const void * address, size_t size,
				void *buffer),
		int8_t (*write)(void * address, size_t size,
				const void *buffer)) __attribute__ ((cold));

/**
 * Discards all current events in the calling core's local buffer.
 * @return BARELOG_SUCCESS on success or an error code in case of exception.
 */
extern int8_t device_mem_manager_clean_buffer(void);

/**
 * Discards the events from the oldest one to n further events
 * in the local buffer of the calling core.
 * @param n number of events to discard.
 * @return BARELOG_SUCCESS on success, an error code if an error occurs.
 */
extern int8_t device_mem_manager_clean(uint32_t n);

/**
 * Writes an event into the local event buffer of the calling core.
 * @param event the event to write.
 * @return BARELOG_SUCCESS on success, an error code if an error occurs.
 */
extern int8_t device_mem_manager_write_buffer(barelog_event_t event) __attribute__ ((hot));

/**
 * Flushes the local event buffer into the shared memory
 * section associated to the calling core.
 * @return BARELOG_SUCCESS on success, an error code if an error occurs.
 */
extern int8_t device_mem_manager_flush_buffer(void);

/**
 * Flushes all event contained in the calling core's event buffer
 * from the older one to n events further into the corresponding
 * shared memory section.
 * @param n number of events to flush.
 * @return BARELOG_SUCCESS on success, an error code if an error occurs.
 */
extern int8_t device_mem_manager_flush(uint32_t n);

/**
 * Erases all events in the shared memory buffer.
 * @return BARELOG_SUCCESS on success, an error code if something went wrong.
 */
extern int8_t device_mem_manager_clean_memory(void);

/**
 * Indicates whether or not the local events buffer is full (i.e we
 * can possibly override older events, depending on the used policy).
 * @return 1 if the buffer is full, 0 otherwise.
 */
extern int8_t device_mem_manager_is_buffer_full(void);

#if BARELOG_DEBUG_MODE
/**
 * Internal function used for debugging purposes : writes the latest
 * errcode with full description into the shared memory.
 *
 * Note that for obvious debugging reasons, this functions doesn't call
 * any other functions in the barelog's modules and use only memcpy
 * to interact with the shared memory, thus disregarding the manager.read
 * function.
 *
 * @see host_mem_manager_read_debug
 * @param file the file in which the error occurred (usually __FILE__).
 * @param line the line on which the error occurred (usually __LINE__).
 * @param errcode the error code to return.
 * @param message a description message to go along with the error code.
 */
extern void barelog_debug_log(char *file, int line, int8_t errcode, const char *message);

#define BARELOG_DEBUG(file, line, errcode, message) barelog_debug_log(file, line, errcode, message)
#else
#define BARELOG_DEBUG(file, line, errcode, message)

#endif // BARELOG_DEBUG_MODE

#endif /* __BARELOG_DEVICE_MEM_MANAGER__ */
