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
 * @file barelog_parallella.h
 * @brief Module defining the configurations used by barelog specifically
 * for the Parallella platform.
 *
 * This header is used to define every external parameters that
 * we might use to configure the behavior of the application on the
 * Parallella platform.
 *
 * @see https://www.parallella.org/
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_PARALLELLA__
#define __BARELOG_PARALLELLA__

#define BARELOG_NB_CORES 16

/* Maximum size (in bytes) taken in the shared memory by barelog events : */
#define BARELOG_EVENT_SHARED_MEM_MAX 1000000

/* Maximum string length of the Platform name (deprecated) : */
#define BARELOG_PLATFORM_NAME_LENGTH 20

/* Maximum size (in bytes) of a barelog_event :*/
#define BARELOG_EVENT_MAX_SIZE 100

/* Maximum size (in bytes) of each core's local memory reserved for barelog : */
#define BARELOG_LOCAL_MEM_PER_CORE 1000

#define BARELOG_LOCAL_MEM_ATTRIBUTE __attribute__ ((section(".data_bank0")))

/* Debug attribute (TODO : implement mechanism) */
#define BARELOG_VERBOSE 0

/* Memory synchronization (mutexes) between host and device */
#define BARELOG_SAFE_MODE 0

/* Defines whether or not we should apply defensive strategies on code */
#define BARELOG_CHECK_MODE 0

#endif /* __BARELOG_PARALLELLA__ */
