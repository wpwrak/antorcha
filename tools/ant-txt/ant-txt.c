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
#include <sys/types.h>

#include <libant/libant.h>


#define	W	80
#define	H	16


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-b|-x] [-F font_dir ...] [text]\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	struct edit *edits = NULL, **last = &edits;
	uint8_t *canvas;
	const char *err;
	int binary = 0, xbm = 0;
	int i, c, res;

	while ((c = getopt(argc, argv, "bF:x")) != EOF)
		switch (c) {
		case 'b':
			binary = 1;
			break;
		case 'x':
			xbm = 1;
			break;
		case 'F':
			add_font_dir(optarg);
			break;
		default:
			usage(*argv);
		}
	if (binary && xbm)
		usage(*argv);
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
	if (binary)
		res = dump_binary(stdout, canvas, W, H);
	else if (xbm)
		res = dump_xbm(stdout, canvas, W, H);
	else
		res = dump_ascii(stdout, canvas, W, H);
	if (res < 0) {
		perror("write");
		exit(1);
	}
	if (fflush(stdout) == EOF) {
		perror("fflush");
		exit(1);
	}
	return 0;
}
