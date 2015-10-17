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
 * @file barelog_platform.h
 * @brief Module defining a platform to use barelog against.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_PLATFORM__
#define __BARELOG_PLATFORM__

#include <stdint.h>

#include "barelog_mem_space.h"
#include "barelog_internal.h"

/**
 * Structure of a platform to use barelog against.
 */
typedef struct {
	/** Name of the platform (deprecated)*/
	char name[BARELOG_PLATFORM_NAME_LENGTH];
	/** Shared memory space to use barelog on */
	barelog_mem_space_t mem_space;
} barelog_platform_t;

#endif /* __BARELOG_PLATFORM__ */
