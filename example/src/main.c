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

#include <stdlib.h>
#include <stdio.h> 
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#include "e-hal.h" 
#include "e-loader.h"
#include "trace.h"
#include "barelog_host.h"
#include "barelog_event.h"

static void print_platform_info(e_platform_t platform);
static void print_shm_info(e_mem_t mbuff);


/* <BARELOG OVERLOAD>*/
void * my_init(void * address, size_t size, void * data) {

	void *ret = NULL;
	if (e_alloc(data, (off_t) address, size) == E_OK) {
		e_mem_t *tmp = (e_mem_t *) data;
		ret = tmp->base;
	}

	return ret;
}


int8_t my_finalize(void * mem_space) {
	return e_free(mem_space);
}

int8_t my_read(const void *address, size_t size, void *buffer) {
	memcpy(buffer, address, size);
	return BARELOG_SUCCESS;
}

int8_t my_write(void *address, size_t size, const void *buffer) {
	memcpy(address, buffer, size);
	return BARELOG_SUCCESS;
}
/* </BARELOG OVERLOAD>*/

int main(int argc, char **argv) {

	/* <BARELOG OVERLOAD>*/
	barelog_mem_space_t mem_space =
		{
			.phy_base = (void *) 0x01000000, //offset par rapport a la base de la mem partagee
			.length = 0x01000000, .alignment = 1, .word_size = 4,
			.data = 0 };

	barelog_platform_t my_platform = { .name = "PARALLELLA", .mem_space =
		mem_space, };

	/* </BARELOG_OVERLOAD>*/

	e_platform_t platform; // Stock infos sur la plateforme.

	e_epiphany_t wg_single; // Stock infos sur un workgroup (ensemble de eCores) cree.

	/* Initialisation de l'epiphany avec le fichier
	 * de configuration .hdf par defaut (parametre
	 * NULL) et etabli la connexion avec la chip.
	 */
	if (e_init(NULL) != E_OK) {
		print_trace(TRACE_ERROR, "e_init error.");
		exit(EXIT_FAILURE);
	}

	/* Reset le systeme de l'epiphany pour s'assurer
	 * qu'il est dans un etat coherent.
	 */
	if (e_reset_system() != E_OK) {
		print_trace(TRACE_ERROR, "e_reset_system error.");
		exit(EXIT_FAILURE);
	}

	int8_t ret = barelog_host_init(my_platform, my_init, my_read, my_write,
		my_finalize);
	if (ret != 16) {
		print_trace(TRACE_ERROR, "barelog_init error");
		exit(EXIT_FAILURE);
	}

	/* Recuperation des informations sur la chip
	 * epiphany.
	 */
	if (e_get_platform_info(&platform) != E_OK) {
		print_trace(TRACE_ERROR, "e_get_platform_info error.");
		exit(EXIT_FAILURE);
	}

	print_platform_info(platform);

	/* Allocation d'un espace memoire partage entre les
	 * eCores et le programme hote :
	 */
	e_mem_t mbuff;
	size_t shm_size = 1024;
	if (e_alloc(&mbuff, (off_t) (0x01000000 + BARELOG_SHARED_MEM_MAX),
		shm_size) != E_OK) {
		print_trace(TRACE_ERROR, "e_alloc error\n");
		exit(EXIT_FAILURE);
	}

	print_shm_info(mbuff);

	/* Ecriture d'un message pour l'epiphany :
	 * ssize_t e_write(void *dev, unsigned row, unsigned col, off_t to_addr, const void *buf, size_t size);
	 * On success, returns number of bytes read. Else returns E_ERR.
	 */
	if (e_write(&mbuff, 0, 0, 0, "Hello EPIPHANY !", 17) < 0) {
		print_trace(TRACE_ERROR, "e_read error.\n");
		exit(EXIT_FAILURE);
	}

	/* Creation d'un workgroup avec un 4 eCore
	 * situes a la position (relative) (0,0).
	 */
	if (e_open(&wg_single, 0, 0, 4, 4) != E_OK) {
		print_trace(TRACE_ERROR, "e_open error.");
		exit(EXIT_FAILURE);
	}

	/* Chargement d'un programme sur tout un workgroup.
	 */
	if (e_load_group("e-main.srec", &wg_single, 0, 0, 4, 4, E_TRUE)
		!= E_OK) {
		print_trace(TRACE_ERROR, "e_load_group error.");
		exit(EXIT_FAILURE);
	}

	// On attend la fin de l'execution des programmes.
	sleep(5);

	/* Recuperation des donnees en memoire partagee
	 * et ecrites par les processus de l'Epiphany :
	 */
	char buff[16][50];

	// ssize_t e_read(void *dev, unsigned row, unsigned col, off_t from_addr, void *buf, size_t size)
	// On success, returns number of bytes read. Else returns E_ERR.
	for (uint8_t i = 0; i < 16; ++i) {
		if (e_read(&mbuff, 0, 0, i * 50, buff[i], 50) < 0) {
			print_trace(TRACE_ERROR, "e_read error.\n");
			strcpy(buff[i], "READ ERROR");
		}
	}

	for (uint8_t i = 0; i < 16; ++i) {
		fprintf(stdout, "Received message from core %u : %s\n", i,
			buff[i]);
	}
	fprintf(stdout, "\n");

	uint32_t n = 0;
	barelog_result_buffer_t res_buffer[16];
	barelog_event_t *events_buff;
	for (uint8_t i = 0; i < 16; ++i) {
		n = (uint32_t) barelog_read_log(i, &events_buff);
		barelog_events_to_strings(events_buff, n, &(res_buffer[i]));
		fprintf(stderr, "LOGS :\n");
		for (uint32_t j = 0; j < res_buffer[i].buffer_length; ++j) {
			fprintf(stderr, " * %u %s\n", j,
				res_buffer[i].buffer[j]);
		}
	}

	fprintf(stderr, "\nBARELOG_DEBUG :\n");
	barelog_read_debug();
	fprintf(stderr, "\n");

	if (e_close(&wg_single) != E_OK) {
		print_trace(TRACE_ERROR, "e_close error.");
		exit(EXIT_FAILURE);
	}

	ret = barelog_host_finalize();
	if (ret != 16) {
		print_trace(TRACE_ERROR, "barelog_host_finalize error");
		fprintf(stderr, "%i \n", ret);
	}

	/* Termine la connection avec l'Epiphany
	 * et libere toutes les eventuelles ressources
	 * allouees lors de l'initialisation (appel
	 * a e_init()
	 */
	if (e_finalize() != E_OK) {
		print_trace(TRACE_ERROR, "e_finalize error.");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}

static void print_platform_info(e_platform_t platform) {
	fprintf(stdout, "Epiphany platform info :\n");
	fprintf(stdout, "  -> platform part number : %s\n", platform.version);
	fprintf(stdout, "  -> e-hal version number : %u\n", platform.hal_ver);
	fprintf(stdout, "  -> platform initialized ? %i\n",
		platform.initialized);
	fprintf(stdout, "  -> base address of platform registers : %u\n",
		platform.regs_base);
	fprintf(stdout, "  -> number of epiphany chips in platform : %i\n",
		platform.num_chips);
	fprintf(stdout, "  -> platform absolute minimum row number : %u\n",
		platform.row);
	fprintf(stdout, "  -> platform absolute minimum col number : %u\n",
		platform.col);
	fprintf(stdout, "  -> number of rows in platform : %u\n",
		platform.rows);
	fprintf(stdout, "  -> number of cols in platform : %u\n",
		platform.cols);
	fprintf(stdout,
		"  -> number of external memory segments in platform : %i\n\n",
		platform.num_emems);
}

static void print_shm_info(e_mem_t mbuff) {
	// Affichage des caracteristiques de l'espace alloue :
	fprintf(stdout, "Shared memory segment infos :\n");
	fprintf(stdout,
		"  -> Physical global base address of ext mem buffer as seen by host side : 0x%x\n",
		(unsigned) mbuff.phy_base);
	fprintf(stdout,
		"  -> Physical global base address of ext mem buffer as seen by device side (epiphany) : 0x%x\n",
		(unsigned) mbuff.ephy_base);
	fprintf(stdout, "  -> Physical base address of memory page : 0x%x\n",
		(unsigned) mbuff.page_base);
	fprintf(stdout,
		"  -> Offset of memory region base to memory page base : 0x%x\n",
		(unsigned) mbuff.page_offset);
	fprintf(stdout,
		"  -> Size of eDRAM allocated buffer for host side : 0x%x\n",
		mbuff.map_size);
	fprintf(stdout,
		"  -> Size of eDRAM allocated buffer for device side : 0x%x\n\n",
		mbuff.emap_size);
}
