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

#include <string.h>
#include <stdlib.h>

#include "barelog_internal.h"
#include "barelog_host_mem_manager.h"
#include "barelog_mem_space.h"
#include "barelog_buffer.h"

#if BARELOG_DEBUG_MODE
#include <stdio.h>
#include <unistd.h>
#endif

/* Size (in bytes) allowed to store data (if any) during
 * memory blocks initialization.
 */
#define BARELOG_MEM_SPACE_DATA_SIZE 512

/* Different shared memory sections to use to store
 * events for each logged core. The last mem_space
 * is used to reference the location of the mutexes
 * bytes in shared memory (if used, see BARELOG_SAFE_MODE flag).
 */
static barelog_mem_space_t mem_space[BARELOG_HOST_NB_MEM_SPACE];

static barelog_host_mem_manager_t manager;

static uint8_t initialized = 0;

#if BARELOG_SAFE_MODE
#define barelog_get_mutex(core, mutex) do { \
	if (manager.read(mem_space[BARELOG_NB_CORES].base + (core), 1, &(mutex)) != BARELOG_SUCCESS) { \
		return BARELOG_SHRMEM_READ_ERR; \
	} \
} while(0)

#define barelog_set_mutex(core, value) do { \
	uint8_t set_mutex_tmp = (value); \
	if (manager.write(mem_space[BARELOG_NB_CORES].base + (core), 1, &set_mutex_tmp) != BARELOG_SUCCESS) { \
		return BARELOG_SHRMEM_WRITE_ERR; \
	} \
} while(0)

#define barelog_try_mutex(core) do { \
uint8_t __mutex = 1; \
uint32_t __timeout = BARELOG_MUTEX_TRY_MAX; \
while (__mutex) { \
	--__timeout; \
	barelog_get_mutex((core), __mutex); \
	if (__timeout <= 0) { \
		return BARELOG_TIMEOUT_ERR; \
	} \
} \
} while(0)
#else

#define barelog_get_mutex(core, mutex)
#define barelog_set_mutex(core, value)
#define barelog_try_mutex(core)

#endif // BARELOG_SAFE_MODE

// Retourne le nombre de zones correctement allouees.
int8_t host_mem_manager_init(const barelog_platform_t platform,
		void * (*init)(void *address, size_t size, void *data),
		int8_t (*read)(const void *address, size_t size, void *buffer),
		int8_t (*write)(void *address, size_t size, const void *buffer),
		int8_t (*finalize)(void *mem_space)) {

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (initialized || !init || !finalize || !platform.mem_space.phy_base) {
		return BARELOG_UNINITIALIZED_PARAM_ERR;
	}

	if (platform.mem_space.length <= BARELOG_SHARED_MEM_MAX) {
		return BARELOG_INIT_ERR;
	}
#endif

	manager.init = init;
	manager.read = read;
	manager.write = write;
	manager.finalize = finalize;
	// TODO : alignment concerns...

	/* Barelog's configuration areas : */
	// Memory synchronization (mutexes between host and device) :
#if BARELOG_SAFE_MODE
	mem_space[BARELOG_SAFE_MODE_I].phy_base = platform.mem_space.phy_base;
	mem_space[BARELOG_SAFE_MODE_I].length = BARELOG_SAFE_MEM_SIZE;
	mem_space[BARELOG_SAFE_MODE_I].alignment = platform.mem_space.alignment;
	mem_space[BARELOG_SAFE_MODE_I].word_size = platform.mem_space.word_size;
	mem_space[BARELOG_SAFE_MODE_I].data = calloc(1, BARELOG_MEM_SPACE_DATA_SIZE);
	mem_space[BARELOG_SAFE_MODE_I].base = manager.init(mem_space[BARELOG_SAFE_MODE_I].phy_base,
							mem_space[BARELOG_SAFE_MODE_I].length,
							mem_space[BARELOG_SAFE_MODE_I].data);
	if (mem_space[BARELOG_SAFE_MODE_I].base == NULL) {
		free(mem_space[BARELOG_SAFE_MODE_I].data);
		return BARELOG_ERR;
	}
	memset(mem_space[BARELOG_SAFE_MODE_I].base, 0, BARELOG_NB_MUTEX_BYTES);
#endif // BARELOG_SAFE_MODE

#if BARELOG_DEBUG_MODE
	mem_space[BARELOG_DEBUG_MODE_I].phy_base = platform.mem_space.phy_base + BARELOG_DEBUG_OFF;
	mem_space[BARELOG_DEBUG_MODE_I].length = BARELOG_DEBUG_MEM_SIZE;
	mem_space[BARELOG_DEBUG_MODE_I].alignment = platform.mem_space.alignment;
	mem_space[BARELOG_DEBUG_MODE_I].word_size = platform.mem_space.word_size;
	mem_space[BARELOG_DEBUG_MODE_I].data = calloc(1, BARELOG_MEM_SPACE_DATA_SIZE);
	mem_space[BARELOG_DEBUG_MODE_I].base = manager.init(mem_space[BARELOG_DEBUG_MODE_I].phy_base,
							mem_space[BARELOG_DEBUG_MODE_I].length,
							mem_space[BARELOG_DEBUG_MODE_I].data);
	if (mem_space[BARELOG_DEBUG_MODE_I].base == NULL) {
		free(mem_space[BARELOG_DEBUG_MODE_I].data);
		return BARELOG_ERR;
	}
	memset(mem_space[BARELOG_DEBUG_MODE_I].base, 0, BARELOG_DEBUG_MEM_SIZE);
#endif // BARELOG_DEBUG_MODE
	/* End of Barelog's configuration areas. */

	/* Barelog's data areas, used to store events in shared memory : */
	void *base = platform.mem_space.phy_base + BARELOG_SHARED_MEM_DATA_OFFSET;
	for (uint32_t i = 0; i < BARELOG_NB_CORES; ++i) {
		mem_space[i].phy_base = base
				+ i * BARELOG_SHARED_MEM_PER_CORE_MAX;
		mem_space[i].length = BARELOG_SHARED_MEM_PER_CORE_MAX;
		mem_space[i].alignment = platform.mem_space.alignment;
		mem_space[i].word_size = platform.mem_space.word_size;
		mem_space[i].data = calloc(1, BARELOG_MEM_SPACE_DATA_SIZE);
		mem_space[i].base = manager.init(mem_space[i].phy_base,
				mem_space[i].length, mem_space[i].data);
		if (mem_space[i].base == NULL) {
			free(mem_space[i].data);
			return i - 1;
		}
		memset(mem_space[i].base, 0, mem_space[i].length);
	}
	/* End of Barelog's data areas. */

	initialized = 1;

	return BARELOG_NB_CORES;
}

int8_t host_mem_manager_finalize(void) {

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (!initialized) {
		return BARELOG_INCONSISTENT_PARAM_ERR;
	}
#endif

	for (uint32_t i = 0; i < BARELOG_HOST_NB_MEM_SPACE; ++i) {
		if (manager.finalize(mem_space[i].data) != BARELOG_SUCCESS) {
			/* FIXME : it could not always be the "data" field that corresponds to
			 * the data to deallocate. A safest way to proceed could be
			 * to do a reverse on mmap and to clean the corresponding allocated
			 * area since we know that "data" corresponds to an address created
			 * by the use of mmap.
			*/
			return i;
		}
		free(mem_space[i].data);
	}

	initialized = 0;

	return BARELOG_NB_CORES;
}

int32_t host_mem_manager_read_mem_space(uint32_t core, barelog_event_t **events) {

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (core >= BARELOG_NB_CORES || core < 0) {
		return BARELOG_INCONSISTENT_PARAM_ERR;
	}

	if (events == NULL) {
		return BARELOG_UNINITIALIZED_PARAM_ERR;
	}
#endif

	int8_t ret = 0;
	(void) ret;
	*events = calloc(BARELOG_EVENT_PER_CORE_SHR_MEM_MAX, sizeof(barelog_event_t));
	uint32_t n = BARELOG_EVENT_PER_CORE_SHR_MEM_MAX; // real number of events read;

	barelog_try_mutex(core);
	barelog_set_mutex(core, 1);
	const void *base = mem_space[core].base; // On lit du cote host donc on lit dans les @virtuelles !
	ret = manager.read(base, BARELOG_EVENT_PER_CORE_SHR_MEM_MAX*sizeof(barelog_event_t), (void *) (*events));

#if BARELOG_CHECK_MODE || BARELOG_DEBUG_MODE
	if (ret != BARELOG_SUCCESS) {
		barelog_set_mutex(core, 0);
		return BARELOG_SHRMEM_READ_ERR;
	}
#endif
	barelog_set_mutex(core, 0);

	for (uint32_t i = 0; i < BARELOG_EVENT_PER_CORE_SHR_MEM_MAX; ++i) {
		if (strlen((*events)[i].data) == 0) { // FIXME test a ameliorer
			n = i;
			break;
		}
	}

	return n;
}

#if BARELOG_DEBUG_MODE
int8_t host_mem_manager_read_debug(void) {
	int8_t ret = BARELOG_SUCCESS;
	barelog_event_t event;
	void *base = mem_space[BARELOG_DEBUG_MODE_I].base;

	memcpy(&event, base, sizeof(barelog_event_t));

	fprintf(stderr, "%s\n", event.data);

	return ret;
}
#endif // BARELOG_DEBUG_MODE

