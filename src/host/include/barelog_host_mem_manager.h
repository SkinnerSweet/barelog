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
 * @file barelog_host_mem_manager.h
 * @brief Module defining all functions offered by barelog for the host program.
 *
 * This header defines the functions structures and functions used to initialize
 * and finalize the host part of the logger and to read the events inside
 * the shared memory once the logging session is over.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_HOST_MEM_MANAGER__
#define __BARELOG_HOST_MEM_MANAGER__

#include <stdint.h>
#include <string.h>

#include "barelog_platform.h"
#include "barelog_buffer.h"
#include "barelog_policy.h"

/**
 * Structure used to hold all of the barelog host manager functions.
 * We use pointers to allow the user to use the functions of their choice,
 * depending on the logged platform.
 */
typedef struct {
	/**
	 * Function used to initialize a chunk in the shared memory space.
	 * @param address the beginning address of the chunk to initialize.
	 * @param size the size of the chunk to initialize.
	 * @param data (optional) parameter that may be used by the initialization function.
	 * @return must return the virtual address corresponding to the base of the allocated memory space
	 * (if any). After the initialization, one must use this address to access the
	 * allocated memory within the host. Should return NULL in case something went wrong.
	 */
	void * (*init)(void *address, size_t size, void *data);
	/** Function used by the host to read into the shared memory.
	 * @param address the address to read.
	 * @param size the size of the memory to read.
	 * @param buffer the buffer in which to store the reading result.
	 * @return BARELOG_SUCCESS if all is clear, an error code otherwise.
	 */
	int8_t (*read)(const void *address, size_t size, void *buffer);
	/** Function used by the host to write into the shared memory.
	 * @param address the address to write.
	 * @param size the size of the memory to write.
	 * @param buffer the buffer from which to write the reading result.
	 * @return BARELOG_SUCCESS if all is clear, an error code otherwise.
	 */
	int8_t (*write)(void *address, size_t size, const void *buffer);
	/** Function used to finalize a previously initialized chunk of shared memory.
	 * @param mem_space the mem_space to finalize.
	 * @return BARELOG_SUCCESS if all is clear, an error code otherwise.
	 */
	int8_t (*finalize)(void *mem_space);
} barelog_host_mem_manager_t;

/**
 * Initializes the host's memory manager. Should be called
 * before any subsequent call to any other function in this module.
 * @param platform the platform to allocate the (shared) memory spaces against.
 * @param init the function used by the host to initialize a memory section.
 * @param read the function used by the host to read data from a memory section.
 * @param write the function used by the host to write data into a memory section.
 * @param finalize the function used by the host to deallocate a (shared) memory space.
 * @return BARELOG_NB_CORES on success. Otherwise if ret > 0, it is the number of
 * memory segments correctly allocated and if ret < 0 it is an error code.
 */
extern int8_t host_mem_manager_init(const barelog_platform_t platform,
	void * (*init)(void * address, size_t size, void * data),
	int8_t (*read)(const void * address, size_t size, void *buffer),
	int8_t (*write)(void * address, size_t size, const void *buffer),
	int8_t (*finalize)(void * mem_space)) __attribute__ ((cold));

/**
 * Finalizes the host's memory manager. Deallocate all previously allocated
 * (shared) memory segments.
 * @return BARELOG_NB_CORES on success. Otherwise if ret > 0, it is the number of
 * memory segments correctly deallocated and if ret < 0 it indicates an error code.
 */
extern int8_t host_mem_manager_finalize(void) __attribute__ ((cold, destructor));

/**
 * Reads the memory section dedicated to a core and returns the corresponding
 * events buffer.
 * WARNING : it is the responsibility of the caller to free this buffer afterwards.
 * @param core the core on which to read the events.
 * @return the number of events read from shared memory.
 */
extern int32_t host_mem_manager_read_mem_space(uint32_t core,
	barelog_event_t **events);

#if BARELOG_DEBUG_MODE
/**
 * Function used to read and display on stderr
 * the shared memory error section (if applicable).
 * @see barelog_debug_log
 * @return BARELOG_SUCCESS if everything went well, an error code otherwise.
 */
extern int8_t host_mem_manager_read_debug(void);
#endif // BARELOG_DEBUG_MODE

#endif /* __BARELOG_HOST_MEM_MANAGER__ */
