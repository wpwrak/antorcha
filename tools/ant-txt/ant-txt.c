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
#include <unistd.h>
#include <assert.h>

#include <libtxt/libtxt.h>


#define	W	80
#define	H	16


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-F font_dir ...] [text]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	struct edit *edits = NULL, **last = &edits;
	uint8_t *canvas;
	const char *err;
	int i, x, y;
	int c;

	while ((c = getopt(argc, argv, "F:")) != EOF)
		switch (c) {
		case 'F':
			add_font_dir(optarg);
			break;
		default:
			usage(*argv);
		}
	for (i = optind; i != argc; i++) {
		while (*last)
			last = &(*last)->next;
		if (edits) {
			*last = malloc(sizeof(struct edit));
			if (!*last)
				abort();
			(*last)->type = edit_nl;
			last = &(*last)->next;
		}
		*last = text2edit(argv[i], &err);
		if (!*last) {
			fprintf(stderr, "\"%s\": %s\n", argv[i], err);
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
