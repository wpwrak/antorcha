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


#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "util.h"
#include "libtxt.h"


static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!?$%+-*/=@.";


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
	int max_width;
};


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


struct image *load_image_file(FILE *file, const char **error)
{
	struct image *img;
	const char *err;

	img = alloc_type(struct image);
	err = read_xbm_file(file, img);
	if (err) {
		if (error)
			*error = err;
		free_image(img);
		return NULL;
	}
	return img;
}


struct image *load_image(const char *name, const char **error)
{
	FILE *file;
	struct image *img;

	file = fopen(name, "r");
	if (!file) {
		if (error)
			*error = alloc_sprintf("%s: %s", name, strerror(errno));
		return NULL;
	}
	img = load_image_file(file, error);
	fclose(file);
	return img;
}


void free_image(struct image *img)
{
	if (img) {
		free(img->data);
		free(img);
	}
}


/* ----- Font generation --------------------------------------------------- */


static void set_block(struct font *font, int *n,
    int xlast, int x, int y0, int y1)
{
	int width = x-xlast-1;

	if (width > font->max_width)
		font->max_width = width;
	font->sym[*n].x = xlast+1;
	font->sym[*n].y = y0;
	font->sym[*n].w = width;
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
	if (font) {
		free_image(font->img);
		free(font);
	}
}


/* ----- Drawing on a canvas ----------------------------------------------- */


static void do_draw(uint8_t *canvas, int width, int heigth,
    const struct image *img, int ox, int oy, int w, int h, int x, int y)
{
	int ix, iy, xt;

	for (ix = 0; ix != w; ix++) {
		if (x+ix < 0 || x+ix >= width)
			continue;
		for (iy = 0; iy != h; iy++) {
			if (y+iy < 0 || y+iy >= heigth)
				continue;
			xt = ox+ix;
			if (img->data[(oy+iy)*img->span+(xt >> 3)] &
			    (1 << (xt & 7))) {
				xt = x+ix;
				canvas[((y+iy)*width+xt) >> 3] |= 1 << (xt & 7);
			}
		}
	}
}


int draw_image(void *canvas, int width, int height,
    const struct image *img, int x, int y)
{
	do_draw(canvas, width, height, img, 0, 0, img->w, img->h, x, y);
	return img->w;
}


int draw_char(void *canvas, int width, int height,
    const struct font *font, char c, int x, int y)
{
	const char *cp;
	const struct sym *sym;

	if (c == ' ')
		return font->max_width;
	cp = strchr(charset, c);
	if (!cp)
		return 0;
	sym = font->sym+(cp-charset);

	do_draw(canvas, width, height,
	    font->img, sym->x, sym->y, sym->w, sym->h, x, y);

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
