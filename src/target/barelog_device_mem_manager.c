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

#include "barelog_device_mem_manager.h"

#include "barelog_internal.h"

#if BARELOG_DEBUG_MODE
#include <stdio.h>
#include <string.h>
#endif // BARELOG_DEBUG_MODE

/* WARNINGS :
 * This code should be stored in the internal memory of the logged core.
 * The numbering of the cores must begin at 0.
 */

/* FIXME : There could be cases where we will not have any attribute to ensure
 that the data are correctly stored in the local memory. We should look for ways to
 counter those cases. */

static barelog_device_mem_manager_t manager BARELOG_LOCAL_MEM_ATTRIBUTE;

/* Host/Device synchronization functions */
#if BARELOG_SAFE_MODE
static void* mutex_byte_address BARELOG_LOCAL_MEM_ATTRIBUTE;

#define barelog_get_mutex(mutex) do { \
	if (manager.read(mutex_byte_address, 1, &(mutex)) != BARELOG_SUCCESS) { \
		return BARELOG_SHRMEM_READ_ERR; \
	} \
}while(0)

#define barelog_set_mutex(value) do { \
	uint8_t __set_mutex_tmp = (value); \
	if (manager.write(mutex_byte_address, 1, &__set_mutex_tmp) != BARELOG_SUCCESS) { \
		return BARELOG_SHRMEM_WRITE_ERR; \
	} \
} while(0)

#define barelog_try_mutex() do { \
uint8_t __mutex = 1; \
uint32_t __timeout = BARELOG_MUTEX_TRY_MAX; \
while (__mutex) { \
	--__timeout; \
	barelog_get_mutex(__mutex); \
	if (__timeout <= 0) { \
		return BARELOG_TIMEOUT_ERR; \
	} \
} \
} while (0)
#else

#define barelog_get_mutex(mutex)
#define barelog_set_mutex(value)
#define barelog_try_mutex()

#endif // BARELOG_SAFE_MODE

#if BARELOG_DEBUG_MODE

void barelog_debug_log(char *file, int line, int8_t errcode,
	const char *message) {
	barelog_event_t event;
	snprintf(event.data, BARELOG_BUF_MAX_SIZE, "%s:%d:%i: %s", file, line, errcode, message);
	memcpy(manager.debug_address, &event, sizeof(barelog_event_t));
}

#endif // BARELOG_DEBUG_MODE

static inline uint32_t mod(int32_t a, int32_t b) {
	const int32_t tmp = (a) % (b);

	uint32_t result =
		(tmp >= 0) ? ((uint32_t) tmp) : ((uint32_t) (tmp + b));

	return result;
}

int8_t device_mem_manager_init(const uint32_t my_core,
	const barelog_platform_t platform, const barelog_policy_t buffer_policy,
	const barelog_policy_t memory_policy,
	int8_t (*read)(const void * address, size_t size, void *buffer),
	int8_t (*write)(void * address, size_t size, const void *buffer)) {

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	int8_t ret = 0;
	if (manager.initialized || !read || !write || !platform.mem_space.phy_base) {
		ret = BARELOG_UNINITIALIZED_PARAM_ERR;
		BARELOG_DEBUG(__FILE__, __LINE__, ret,
			"device_mem_manager_init param");
		return ret;
	}
#endif

#if BARELOG_DEBUG_MODE
	manager.debug_address = platform.mem_space.phy_base + BARELOG_DEBUG_OFF;
#endif

	manager.core = my_core;
	manager.read = read;
	manager.write = write;
	manager.buffer_policy = buffer_policy;
	manager.memory_policy = memory_policy;

	void *base = platform.mem_space.phy_base
		+ BARELOG_SHARED_MEM_DATA_OFFSET;
	manager.mem_space.phy_base = base + manager.core * BARELOG_SHARED_MEM_PER_CORE_MAX;
	manager.mem_space.length = BARELOG_SHARED_MEM_PER_CORE_MAX;
	manager.mem_space.alignment = platform.mem_space.alignment;
	manager.mem_space.word_size = platform.mem_space.word_size;
	manager.mem_space.data = 0;
	manager.mem_space.base = manager.mem_space.phy_base;

	manager.shr_events.events = (barelog_event_t *) (manager.mem_space.phy_base);
	manager.shr_events.imax = BARELOG_SHARED_MEM_PER_CORE_MAX
		/ sizeof(barelog_event_t);
	manager.shr_events.index = 0;

	manager.events.head = 0;
	manager.events.tail = 0;
	manager.events.full = 0;
	manager.events.empty = 1;

#if BARELOG_SAFE_MODE
	mutex_byte_address = platform.mem_space.phy_base + core;
#endif

	manager.initialized = 1;

	return BARELOG_NB_CORES;
}

int8_t device_mem_manager_write_buffer(barelog_event_t event) {
	if (manager.events.full) {
		int8_t ret = 0;
		(void) ret;
		switch (manager.buffer_policy) {
		case SKIP:
			return BARELOG_SUCCESS;
			break;
		case REPLACE:
			manager.events.tail = (manager.events.tail + 1)
				% BARELOG_EVENT_PER_CORE_MAX;
			break;
		case FLUSH:
			ret = device_mem_manager_flush_buffer();
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
			if (ret != BARELOG_SUCCESS) {
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"device_mem_manager_flush_buffer call");
				return ret;
			}
#endif
			break;
		case DESTROY:
			ret = device_mem_manager_flush_buffer();
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
			if (ret != BARELOG_SUCCESS) {
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"device_mem_manager_flush_buffer call");
				return ret;
			}
#endif
			ret = device_mem_manager_clean_buffer();
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
			if (ret != BARELOG_SUCCESS) {
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"device_mem_manager_cleah_buffer call");
				return ret;
			}
#endif
			break;
		default:
			BARELOG_DEBUG(__FILE__, __LINE__, BARELOG_ERR, "unrecognized policy");
			return BARELOG_ERR;
		}
	}

	manager.events.buffer[manager.events.head] = event;
	manager.events.buffer[manager.events.head].core = manager.core;
	manager.events.empty = 0;

	manager.events.head = (manager.events.head + 1) % BARELOG_EVENT_PER_CORE_MAX;

	/* If the next case to fulfill is already taken */
	if (manager.events.head == manager.events.tail) {
		/* The buffer is full */
		manager.events.full = 1;
	}

	return BARELOG_SUCCESS;
}

inline int8_t device_mem_manager_clean_buffer(void) {
	uint32_t events_to_read =
			(manager.events.full) ?
				BARELOG_EVENT_PER_CORE_MAX :
				(mod((manager.events.head - manager.events.tail),
					BARELOG_EVENT_PER_CORE_MAX));
	return device_mem_manager_clean(events_to_read);
}

int8_t device_mem_manager_clean(uint32_t n) {
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	int8_t ret = 0;
	if (n <= 0 || n > BARELOG_EVENT_PER_CORE_MAX) {
		ret = BARELOG_INCONSISTENT_PARAM_ERR;
		BARELOG_DEBUG(__FILE__, __LINE__, ret,
			"device_mem_manager_clean param");
		return ret;
	}
#endif

	if (manager.events.empty) {
		return BARELOG_SUCCESS;
	}

	uint32_t ind = 0;
	uint32_t imax = mod((manager.events.head - 1), BARELOG_EVENT_PER_CORE_MAX);
	for (uint32_t i = 0; i < n; ++i) {
		ind = (manager.events.tail + i) % BARELOG_EVENT_PER_CORE_MAX;
		manager.events.buffer[ind] = BARELOG_EVENT_INITIALIZER;
		if (ind == imax) {
			manager.events.empty = 1;
			break;
		}
	}

	manager.events.tail = (ind + 1) % BARELOG_EVENT_PER_CORE_MAX;
	manager.events.full = 0;

	return BARELOG_SUCCESS;
}

inline int8_t device_mem_manager_flush_buffer(void) {
	uint32_t events_to_read =
			(manager.events.full) ?
				BARELOG_EVENT_PER_CORE_MAX :
				(mod((manager.events.head - manager.events.tail),
					BARELOG_EVENT_PER_CORE_MAX));
	return device_mem_manager_flush(events_to_read);
}

int8_t device_mem_manager_flush(uint32_t n) {
	int8_t ret = 0;
	(void) ret;

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (n <= 0 || n > BARELOG_EVENT_PER_CORE_MAX) {
		ret = BARELOG_INCONSISTENT_PARAM_ERR;
		BARELOG_DEBUG(__FILE__, __LINE__, ret,
			"device_mem_manager_clean param");
		return ret;
	}
#endif

	uint32_t nmax = n;
	uint32_t n1 = 0;
	uint32_t n2 = 0;

	uint32_t events_to_read =
		(manager.events.full) ?
			BARELOG_EVENT_PER_CORE_MAX :
			(mod((manager.events.head - manager.events.tail),
				BARELOG_EVENT_PER_CORE_MAX));

	if (events_to_read == 0) {
		return BARELOG_SUCCESS;
	}

	// We make sure we don't read more events than there are available.
	nmax = (n > events_to_read) ? events_to_read : n;

	uint32_t barelog_total_events_size = nmax * sizeof(barelog_event_t);

	if ((manager.shr_events.imax - manager.shr_events.index + 1) < nmax) {
		switch (manager.memory_policy) {
		case SKIP:
			return BARELOG_SUCCESS;
			break;
		case REPLACE:
			manager.shr_events.index = 0;
			break;
		case DESTROY:
			manager.shr_events.index = 0;
			ret = device_mem_manager_clean_memory();
#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
			if (ret != BARELOG_SUCCESS) {
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"device_mem_manager_clean_memory call");
				return ret;
			}
#endif
			break;
		default:
			BARELOG_DEBUG(__FILE__, __LINE__, BARELOG_ERR, "unrecognized policy");
			return BARELOG_ERR;
		}
	}

	barelog_try_mutex(); barelog_set_mutex(1);

	/* If there are too many events, we must have to separate the writings
	 * in two : first the section from tail to BARELOG_EVENT_PER_CORE_MAX - 1
	 * then the section from 0 to nmax.
	 *
	 * Otherwise we just flush the buffer from the tail of the queue to nmax.
	 */
	if ((manager.events.tail + nmax - 1) >= BARELOG_EVENT_PER_CORE_MAX) {
		n1 = BARELOG_EVENT_PER_CORE_MAX - manager.events.tail;
		if (n1) {
			if (manager.write(
				&(manager.shr_events.events[manager.shr_events.index]),
				n1 * sizeof(barelog_event_t),
				(const void *) (&(manager.events.buffer[manager.events.tail]))) != BARELOG_SUCCESS) {
				barelog_set_mutex(0);
				ret = BARELOG_SHRMEM_WRITE_ERR;
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"shared memory writing error");
				return ret;
			}
		}
		n2 = nmax - n1;
		if (n2) {
			if (manager.write(
				&(manager.shr_events.events[manager.shr_events.index]),
				n2 * sizeof(barelog_event_t),
				(const void *) (&(manager.events.buffer[0]))) != BARELOG_SUCCESS) {
				barelog_set_mutex(0);
				ret = BARELOG_SHRMEM_WRITE_ERR;
				BARELOG_DEBUG(__FILE__, __LINE__, ret,
					"shared memory writing error");
				return ret;
			}
		}
	} else {
		if (manager.write(&(manager.shr_events.events[manager.shr_events.index]),
			barelog_total_events_size,
			(const void *) (&(manager.events.buffer[manager.events.tail]))) != BARELOG_SUCCESS) {
			barelog_set_mutex(0);

			ret = BARELOG_SHRMEM_WRITE_ERR;
			BARELOG_DEBUG(__FILE__, __LINE__, ret,
				"shared memory writing error");
			return ret;
		}
	} barelog_set_mutex(0);

	manager.shr_events.index += nmax;

	ret = BARELOG_SUCCESS;
	BARELOG_DEBUG(__FILE__, __LINE__, ret, "flushing success !");

	return BARELOG_SUCCESS;
}

int8_t device_mem_manager_clean_memory(void) {
	barelog_try_mutex(); barelog_set_mutex(1);
	memset(manager.mem_space.base, 0, manager.mem_space.length);
	barelog_set_mutex(0);

	manager.shr_events.index = 0;

	return BARELOG_SUCCESS;
}

int8_t device_mem_manager_is_buffer_full(void) {
	return manager.events.full;
}
