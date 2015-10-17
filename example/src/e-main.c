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
#include <string.h>
#include <stdint.h>

#include "e-lib.h"
#include "barelog_logger.h"

/*typedef struct {
 e_objtype_t	 objtype;	  // object type identifier
 off_t		 phy_base;	  // physical global base address of external memory segment as seen by host side
 off_t		 ephy_base;	  // physical global base address of external memory segment as seen by device side
 size_t		 size;		  // size of eDRAM allocated buffer for host side
 e_memtype_t	 type;		  // type of memory RD/WR/RW
 } e_memseg_t;

 void *e_read(const void *remote, void *dst, unsigned row, unsigned col, const void *src, size_t n);
 void *e_write(const void *remote, const void *src, unsigned row, unsigned col, void *dst, size_t n);

 Dans les deux definitions precedentes, les champs precedent size correspondent
 aux offsets relatifs aux remote.base (base de la zone memoire) auxquels respectivement
 lire ou ecrire.
 */

#define WAND_BIT	(1 << 3)


/* <BARELOG OVERLOAD>*/
int8_t my_read(const void * address, size_t size, void *buffer) {
	memcpy(buffer, address, size);
	return BARELOG_SUCCESS;
}

int8_t my_write(void * address, size_t size, const void *buffer) {
	memcpy(address, buffer, size);
	return BARELOG_SUCCESS;
}

static uint32_t get_clock(void) {
	return ((uint32_t) (E_CTIMER_MAX-e_ctimer_get(E_CTIMER_0)));
}

/**
 * Initializes the clock (using CTIMER0).
 */
static int8_t init_clock(void) {
	e_ctimer_stop(E_CTIMER_0);
	e_ctimer_set(E_CTIMER_0, E_CTIMER_MAX);
	return 0;
}

/**
 * Starts the clock.
 */
static int8_t start_clock(void) {
	e_ctimer_start(E_CTIMER_0, E_CTIMER_CLK);
	return 0;
}
/* </BARELOG OVERLOAD>*/

static void __attribute__((interrupt)) wand_trace_isr(int signum)
{
	(void) signum;
}

static void sync(void)
{
	uint32_t irq_state;

	/* enable WAND interrupt */
	e_irq_attach(WAND_BIT, wand_trace_isr);
	e_irq_mask(WAND_BIT, E_FALSE);

	/* WAND + IDLE */
	__asm__ __volatile__("wand");
	__asm__ __volatile__("idle");

	/* acknowledge interrupt */
	irq_state = e_reg_read(E_REG_STATUS);
	irq_state &= ~WAND_BIT;
	e_reg_write(E_REG_STATUS, irq_state);
}

int main(int argc, char **argv) {

	size_t shm_size = 1024;
	(void) shm_size;
	char tmp[17];

	unsigned my_row;
	unsigned my_col;
	e_coreid_t coreid;

	coreid = e_get_coreid();
	e_coords_from_coreid(coreid, &my_row, &my_col);

	/* <BARELOG_OVERLOAD> */
	barelog_mem_space_t mem_space =
		{ .phy_base = (void *) 0x8f000000, .length = 0x01000000,
			.alignment = 1, .word_size = 4, .data = 0 };

	barelog_platform_t platform = { .name = "PARALLELLA", .mem_space =
		mem_space, };

	barelog_policy_t policy = REPLACE;

	barelog_init_logger(my_row * 4 + my_col, platform, policy, policy, my_read,
		my_write, get_clock, init_clock, start_clock);
	/* </BARELOG_OVERLOAD> */

	sync();
	barelog_start();

	uint32_t clock00 = get_clock();
	barelog_log("Program starts at %u.", clock00);

	uint32_t clock01 = get_clock();
	(void) clock01;

	char buff[50] = {0};
	barelog_log("e_read begins.");
	barelog_flush(2);
	barelog_clean(2);
	e_read(&e_emem_config, tmp, 0, 0, (void *)(0x8f000000+BARELOG_SHARED_MEM_MAX), 17);
	barelog_log("e_read ends.%u", get_clock());

	uint32_t clock1 = get_clock();
	(void) clock1;

	// Uncomment to test the fulfillment of the events buffer.
	/*for (uint8_t k = 0; k < 1; ++k) {
		for (uint8_t i = 0; i < 200;  ++i) {
			barelog_log("Test %u %u %u", get_clock(), k, i);
			barelog_flush_buffer();
			barelog_clean_buffer();
			__asm__ __volatile__ ("nop");
		}
		__asm__ __volatile__ ("nop");
	}*/

	uint32_t clock2 = get_clock();
	(void) clock2;

	// Uncomment to access various clocks data.
	/*
	barelog_log("Results %u %u %u %u", clock00, clock01, clock1, clock2);
	if (barelog_flush_buffer() != BARELOG_SUCCESS) {
		exit(EXIT_FAILURE);
	}
	barelog_clean_buffer();
	 */

	snprintf(buff, 50, "%s from core %u", tmp, (my_row*4 + my_col));

	barelog_log("e_write begins.");
	barelog_flush(1);
	barelog_clean(1);
	e_write((void*) &e_emem_config, buff, 0, 0, (void *)(0x8f000000+BARELOG_SHARED_MEM_MAX + (my_row*4+my_col)*50), 50);

	barelog_log("e_write ends.");
	barelog_log("Program ends at %u", get_clock());
	barelog_flush(6); // Voluntary flushing too many events to check everything went well.


	exit(EXIT_SUCCESS);
}
