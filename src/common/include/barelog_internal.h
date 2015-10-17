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
 * @file barelog_internal.h
 * @brief Module defining the internal configurations of barelog.
 *
 * This header defines every configuration's data needed internally
 * by every barelog's file. Modify at your own risks !
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_INTERNAL_H__
#define __BARELOG_INTERNAL_H__

#include "barelog_config.h"

/*
 * ------------------------
 * - Error codes (int8_t) -
 * ------------------------
 * Use binary masks ?
 */
/** Success return code */
#define BARELOG_SUCCESS 0
/** General error return code */
#define BARELOG_ERR -1
/** Unitialized parameter error return code */
#define BARELOG_UNINITIALIZED_PARAM_ERR -2
/** Inconsistent parameter error return code */
#define BARELOG_INCONSISTENT_PARAM_ERR -3
/** Shared memory writing error return code */
#define BARELOG_SHRMEM_WRITE_ERR -4
/** Shared memory reading error return code */
#define BARELOG_SHRMEM_READ_ERR -5
/** Timeout expired error return code */
#define BARELOG_TIMEOUT_ERR -6
/** Event conversion error return code */
#define BARELOG_EVENT_CONVERSION_ERR -7
/** Barelog initialization error return code */
#define BARELOG_INIT_ERR -8

/*
 * -----------------
 * - Define types. -
 * -----------------
 */
/** barelog mutex type */
#define barelog_shrmem_mutex_t uint8_t

/*
 * ---------------------------------------------------
 * - Internal parameters (be careful if overridden). -
 * ---------------------------------------------------
 */

/** Number of tries to do in order to get a mutex */
#define BARELOG_MUTEX_TRY_MAX 5

/* Computing offsets regarding the Barelog's policies :*/
#if BARELOG_SAFE_MODE
/** Size (in bytes) taken by the mutexes in shared memory */
#define BARELOG_NB_MUTEX_BYTES BARELOG_NB_CORES
/** Size (in bytes) taken by all data used by the safe mode */
#define BARELOG_SAFE_MEM_SIZE BARELOG_NB_MUTEX_BYTES
/** Index of the safe mode in the mem_space hierarchy */
#define BARELOG_SAFE_MODE_I BARELOG_NB_CORES
#else
#define BARELOG_NB_MUTEX_BYTES 0
#define BARELOG_SAFE_MODE_I 0
#define BARELOG_SAFE_MEM_SIZE 0
#endif

/* Computing offsets regarding the Barelog's policies :*/
#if BARELOG_DEBUG_MODE
/** Size (in bytes) taken by all data used by the debug mode */
#define BARELOG_DEBUG_MEM_SIZE sizeof(barelog_event_t)
/** Index of the debug mode in the mem_space hierarchy */
#define BARELOG_DEBUG_MODE_I (BARELOG_NB_CORES + BARELOG_SAFE_MODE)
/** Offset in the shared memory of the beginning of the debug mode section*/
#define BARELOG_DEBUG_OFF BARELOG_SAFE_MEM_SIZE
#else
#define BARELOG_DEBUG_MEM_SIZE 0
#define BARELOG_DEBUG_MODE_I 0
#define BARELOG_DEBUG_OFF 0
#endif

/** Defines the offset (in bytes) to use to access the events part in the shared
 * memory. It corresponds to the reserved size at the beginning of the allowed
 * shared memory used for barelog's settings such as synchronization flags.  */
#define BARELOG_SHARED_MEM_DATA_OFFSET (BARELOG_NB_MUTEX_BYTES + BARELOG_DEBUG_MEM_SIZE)

/** Maximum size (in bytes) taken in the shared memory by barelog data */
#define BARELOG_SHARED_MEM_MAX (BARELOG_EVENT_SHARED_MEM_MAX + BARELOG_SHARED_MEM_DATA_OFFSET)

/** Maximum size (in bytes) of the string buffer inside a barelog event : */
#define BARELOG_BUF_MAX_SIZE (BARELOG_EVENT_MAX_SIZE - 2*sizeof(uint32_t))

/** Maximum number of events manageable locally per core : */
#define BARELOG_EVENT_PER_CORE_MAX (BARELOG_LOCAL_MEM_PER_CORE/BARELOG_EVENT_MAX_SIZE)

/** Size (in bytes) of each shared memory area reserved per core : */
#define BARELOG_SHARED_MEM_PER_CORE_MAX (BARELOG_EVENT_SHARED_MEM_MAX/BARELOG_NB_CORES)

/** Maximum number of events manageable in shared memory per core : */
#define BARELOG_EVENT_PER_CORE_SHR_MEM_MAX (BARELOG_SHARED_MEM_PER_CORE_MAX/BARELOG_EVENT_MAX_SIZE)

/** Number of used barelog_mem_space_t in the host manager : */
#define BARELOG_HOST_NB_MEM_SPACE (BARELOG_NB_CORES + BARELOG_SAFE_MODE + BARELOG_DEBUG_MODE)

#endif /* __BARELOG_INTERNAL_H__ */
