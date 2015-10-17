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
 * @file barelog_host.h
 * @brief Module providing some nice wrapping for the host_mem_manager.
 *
 * Only this module should be used by the host program.
 *
 * @author Thomas Bertauld
 * @date 17/10/2015
 */


#ifndef __BARELOG_HOST_H__
#define __BARELOG_HOST_H__

#include "barelog_host_mem_manager.h"
#include "barelog_internal.h"

/**
 * @see host_mem_manager_init
 */
#define barelog_host_init(platform, initfct, readfct, writefct, finalizefct) \
host_mem_manager_init(platform, initfct, readfct, writefct, finalizefct)

/**
 * @see host_mem_manager_finalize
 */
#define barelog_host_finalize() host_mem_manager_finalize()

/**
 * @see host_mem_manager_read_mem_space
 */
#define barelog_read_log(core, res) host_mem_manager_read_mem_space(core, res)

#if BARELOG_DEBUG_MODE
/**
 * @see host_mem_manager_read_debug
 */
#define barelog_read_debug() host_mem_manager_read_debug()
#else
#define barelog_read_debug()
#endif // BARELOG_DEBUG_MODE

#endif /*__BARELOG_HOST_H__*/
