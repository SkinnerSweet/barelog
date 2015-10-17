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
 * @file barelog_mem_space.h
 * @brief Module defining mem_space structure.
 *
 * This header defines the structure of what will be called
 * a mem_space. It represents a chunk of the shared memory.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */

#ifndef __BARELOG_MEM_SPACE__
#define __BARELOG_MEM_SPACE__

#include <stdint.h>

#define BARELOG_WORD 1
#define BARELOG_DOUBLE_WORD 2
#define BARELOG_HALF_WORD (1/2)
#define BARELOG_BYTE 0

/**
 * Main structure of a mem_space, representing a chunk
 * of the shared memory.
 */
typedef struct  __attribute__((packed, aligned)) {
	/** physical address*/
	void *phy_base;
	/** (possibly) virtual address (the one used by memcpy on the target of execution) */
	void *base;
	/** length of the memory space */
	uint32_t length;
	/** prefered alignment of data inside this memory space (reserved for future use)*/
	uint8_t alignment; 
	/** size of words inside this memory space (reserved for future use)*/
	uint8_t word_size;
	/** field used to store any return value of the shared memory initialization function*/
	void *data;
} barelog_mem_space_t;

/**
 * mem_space initializer.
 */
extern const barelog_mem_space_t MEM_SPACE_INITIALIZER;

#endif /* __BARELOG_MEM_SPACE__ */
