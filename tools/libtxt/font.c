/*
 * tools/libtxt/font.c - Font operations
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */


#define _GNU_SOURCE /* for vasprintf */
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "libtxt.h"


static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?$%+-*/=@";


#define	CHARS	(sizeof(charset)-1)


struct sym {
	int x, y;
	int w, h;
};

struct image {
	int w, h;
	int span;	/* bytes per row */
	uint8_t *data;
};

struct font {
	struct image *img;
	struct sym sym[CHARS];
};


/* ----- Helper functions -------------------------------------------------- */


static const char *alloc_sprintf(const char *fmt, ...)
{
	va_list ap;
	char *tmp, *res;
	int n;

	va_start(ap, fmt);
	n = vasprintf(&tmp, fmt, ap);
	va_end(ap);
	if (n < 0)
		abort();
	res = malloc(n+1);
	if (!res)
		abort();
	memcpy(res, tmp, n+1);
	return res;
}


/* ----- XBM image --------------------------------------------------------- */


static const char *read_xbm_file(FILE *file, struct image *img)
{
	int n;
	unsigned v;
	char c;
	int bytes = 0;

	img->data = NULL;

	n = fscanf(file, "#define %*[^_]_width %d\n", &img->w);
	if (n < 0)
		return alloc_sprintf("reading width: %s", strerror(errno));
	if (n != 1)
		return alloc_sprintf("width not found (%d)", n);

	n = fscanf(file, "#define %*[^_]_height %d\n", &img->h);
	if (n < 0)
		return alloc_sprintf("reading height: %s", strerror(errno));
	if (n != 1)
		return alloc_sprintf("height not found");

	n = fscanf(file, "%*[^{]{\n");
	if (n < 0)
		return alloc_sprintf("finding data: %s", strerror(errno));

	while (1) {
		n = fscanf(file, " 0x%x%c", &v, &c);
		if (n < 0)
			return alloc_sprintf("reading data: %s",
			    strerror(errno));
		if (n != 2)
			return alloc_sprintf("data not found (%d)", n);

		bytes++;
		img->data = realloc(img->data, bytes);
		if (!img->data)
			abort();
		img->data[bytes-1] = v;

		if (c == ',')
			continue;
		if (c == '}')
			break;
		return alloc_sprintf("invalid data syntax");
	}
	img->span = (img->w+7) >> 3;
	if (bytes != img->h*img->span)
		return alloc_sprintf("%d bytes for %dx%d bitmap",
		    bytes, img->h, img->w);
	return NULL;
}


struct image *load_image(const char *name, const char **error)
{
	FILE *file;
	struct image *img;
	const char *err;

	file = fopen(name, "r");
	if (!file) {
		if (error)
			*error = alloc_sprintf("%s: %s", name, strerror(errno));
		return NULL;
	}

	img = malloc(sizeof(struct image));
	if (!img)
		abort();
	err = read_xbm_file(file, img);
	if (err) {
		if (error)
			*error = err;
		free_image(img);
		return NULL;
	}
	return img;
}


void free_image(struct image *img)
{
	free(img->data);
	free(img);
}


/* ----- Font generation --------------------------------------------------- */


static void set_block(struct font *font, int *n,
    int xlast, int x, int y0, int y1)
{
	font->sym[*n].x = xlast+1;
	font->sym[*n].y = y0;
	font->sym[*n].w = x-xlast-1;
	font->sym[*n].h = y1-y0+1;
	(*n)++;
}


static void analyze_block(struct font *font, int *n, int y0, int y1)
{
	const struct image *img = font->img;
	int x, y, last = -1;

	for (x = 0; x != img->w; x++) {
		for (y = y0; y <= y1; y++)
			if (img->data[y*img->span+(x >> 3)] & (1 << (x & 7)))
				break;
		if (y <= y1)
			continue;
		if (x != last+1)
			set_block(font, n, last, x, y0, y1);
		last = x;
	}
	if (x != last+1)
		set_block(font, n, last, x, y0, y1);
}


static const char *analyze_font(struct font *font)
{
	const struct image *img = font->img;
	int x, y, last = -1;
	int n = 0;
	
	for (y = 0; y != img->h; y++) {
		for (x = 0; x != img->span; x++)
			if (img->data[y*img->span+x])
				break;
		if (x != img->span)
			continue;
		if (y != last+1)
			analyze_block(font, &n, last+1, y-1);
		last = y;
	}
	if (y != last+1)
		analyze_block(font, &n, last+1, y-1);
	if (n != CHARS)
		return alloc_sprintf("found %d instead of %d characters",
		    n, CHARS);
	return NULL;
}


struct font *make_font(struct image *img, const char **error)
{
	struct font *font;
	const char *err;

	font = calloc(1, sizeof(struct font));
	if (!font)
		abort();
	font->img = img;
	err = analyze_font(font);
	if (err) {
		if (error)
			*error = err;
		free_font(font);
		return NULL;
	}
	return font;
}


void free_font(struct font *font)
{
	free_image(font->img);
	free(font);
}


/* ----- Drawing on a canvas ----------------------------------------------- */


int draw_char(void *canvas, int width, int height,
    const struct font *font, char c, int x, int y)
{
	const struct image *img = font->img;
	uint8_t *p = canvas;
	const char *cp;
	const struct sym *sym;
	int ix, iy, xt;

	cp = strchr(charset, c);
	if (!cp)
		return 0;
	sym = font->sym+(cp-charset);

	for (ix = 0; ix != sym->w; ix++) {
		if (x+ix >= width)
			continue;
		for (iy = 0; iy != sym->h; iy++) {
			if (y+iy >= height)
				continue;
			xt = sym->x+ix;
			if (img->data[(sym->y+iy)*img->span+(xt >> 3)] &
			    (1 << (xt & 7))) {
				xt = x+ix;
				p[((y+iy)*width+xt) >> 3] |= 1 << (xt & 7);
			}
		}
	}
	return sym->w;
}


int char_height(const struct font *font, char c)
{
	const char *cp;
	const struct sym *sym;

	cp = strchr(charset, c);
	if (!cp)
		return 0;
	sym = font->sym+(cp-charset);
	return sym->h;
}


/* ----- Testing ----------------------------------------------------------- */


#define	W	64
#define	H	20


int main(int argc, char **argv)
{
	struct font *font;
	struct image *img;
	const char *error;
	uint8_t canvas[W*H/8] = { 0 };
	char *s;
	int x, xo, y;

	img = load_image(argv[1], &error);
	if (!img) {
		fprintf(stderr, "%s\n", error);
		return 1;
	}
	font = make_font(img, &error);
	if (!font) {
		fprintf(stderr, "%s\n", error);
		return 1;
	}

	x = 0;
	for (s = argv[2]; *s; s++) {
		xo = draw_char(canvas, W, H, font, *s, x, 0);
		if (xo)
			x += xo+1;
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
