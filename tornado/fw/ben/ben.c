/*
 * ben/ben.c - Cross-platform testing on the Ben Nanonote
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "../mmc.h"
#include "ben-io.h"


#define SOC_BASE	0x10000000
#define PAGE_SIZE	4096


void *ben_mem;


static void io_setup(void)
{
	int fd;

	fd = open("/dev/mem", O_RDWR | O_SYNC);
	if (fd < 0) {
		perror("/dev/mem");
		exit(1);
	}
	ben_mem = mmap(NULL, PAGE_SIZE*3*16, PROT_READ | PROT_WRITE,
	    MAP_SHARED, fd, SOC_BASE);
	if (ben_mem == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	PDDATS = CARD_nPWR;
	PDDIRS = CARD_nPWR;
	PDDIRC = CARD_CMD | CARD_CLK | 
            CARD_DAT0 | CARD_DAT1 | CARD_DAT2 | CARD_DAT3;
	PDFUNC = CARD_nPWR | CARD_CMD | CARD_CLK |
	    CARD_DAT0 | CARD_DAT1 | CARD_DAT2 | CARD_DAT3;
}


int main(void)
{
	int i;

	io_setup();

	if (!mmc_init()) {
		fprintf(stderr, "mmc_init failed\n");
		exit(1);
	}
	if (!mmc_begin_read(0)) {
		fprintf(stderr, "mmc_begin_read failed\n");
		exit(1);
	}
	for (i = 0; i != MMC_BLOCK; i++)
		printf("%02x%c", mmc_read(), (i & 15) == 15 ? '\n' : ' ');
	mmc_end_read();

	return 0;
}
