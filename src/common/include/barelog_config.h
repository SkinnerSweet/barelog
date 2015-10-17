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
 * @file barelog_config.h
 * @brief Module defining the configurations used by barelog.
 *
 * This header is used to define every external parameters that
 * we might use to configure the behavior of the application.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_CONFIG__
#define __BARELOG_CONFIG__

/*--------------------------------*/
/**
 * Extern configuration file to load (if any).
 */
#include "barelog_parallella.h"
/*--------------------------------*/

/** Number of cores to log on */
#ifndef BARELOG_NB_CORES
#define BARELOG_NB_CORES 16
#endif

/** Maximum size (in bytes) taken in the shared memory by barelog events : */
#ifndef BARELOG_EVENT_SHARED_MEM_MAX
#define BARELOG_EVENT_SHARED_MEM_MAX 1000000
#endif

/** Maximum string length of the platform name (deprecated) : */
#ifndef BARELOG_PLATFORM_NAME_LENGTH
#define BARELOG_PLATFORM_NAME_LENGTH 20
#endif

/** Maximum size (in bytes) of a barelog_event :*/
#ifndef BARELOG_EVENT_MAX_SIZE
#define BARELOG_EVENT_MAX_SIZE 100
#endif

/** Maximum size (in bytes) of each core's local memory reserved for barelog : */
#ifndef BARELOG_LOCAL_MEM_PER_CORE
#define BARELOG_LOCAL_MEM_PER_CORE 1000
#endif

/** (Optional) attribute used to ensure that some parts of the code are stored
 * in the local memory of the traced core.
 */
#ifndef BARELOG_LOCAL_MEM_ATTRIBUTE
#define BARELOG_LOCAL_MEM_ATTRIBUTE
#endif

/** Memory synchronization (mutexes) between host and device (/!\ not fully tested) */
#ifndef BARELOG_SAFE_MODE
#define BARELOG_SAFE_MODE 0
#endif

/** Defines whether or not we should apply defensive strategies on code */
#ifndef BARELOG_CHECK_MODE
#define BARELOG_CHECK_MODE 1
#endif

/** Allows the use of debugging functions (such as fprintf()) that are not useful
 * for the stable version */
#ifndef BARELOG_DEBUG_MODE
#define BARELOG_DEBUG_MODE 1
#endif

#endif /* __BARELOG_CONFIG__ */
