/*
 * tools/ant-txt/ant-txt.c - Antorcha text composer
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <libtxt/libtxt.h>


#define	W	80
#define	H	16


int main(int argc, char **argv)
{
	struct edit *edits = NULL, **last = &edits;
	uint8_t *canvas;
	const char *err;
	int i, x, y;

	for (i = 1; i != argc; i++) {
		while (*last)
			last = &(*last)->next;
		if (edits) {
			*last = malloc(sizeof(struct edit));
			if (!*last)
				abort();
			(*last)->type = edit_nl;
			last = &(*last)->next;
		}
		*last = text2edit(argv[i]);
		if (!*last) {
			fprintf(stderr, "\"%s\": compilation failed\n",
			    argv[i]);
			return 1;
		}
	}

	canvas = apply_edits(W, H, edits, &err);
	if (!canvas) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	for (y = 0; y != H; y++) {
		for (x = 0; x != W; x++)
			if (canvas[(y*W+x) >> 3] & (1 << (x & 7)))
				putchar('#');
			else
				putchar('.');
		putchar('\n');
	}
	return 0;
}
