/*
 * tools/libtxt/edit.c - Editing and rendering
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
#include <sys/types.h>

#include "util.h"
#include "libtxt.h"


#define	DEFAULT_FONT	"5x7"


/* ----- Render a list of editing instructions ----------------------------- */


struct font_dir {
	const char *name;
	struct font_dir *next;
} *font_path = NULL, **last_font_dir = &font_path;


void add_font_dir(const char *name)
{
	struct font_dir *dir;

	dir = alloc_type(struct font_dir);
	dir->name = strdup(name);
	if (!dir->name)
		abort();
	dir->next = NULL;
	*last_font_dir = dir;
	last_font_dir = &dir->next;
}


static struct image *find_font_image(const char *name, const char **error)
{
	struct image *img;
	const char *err;
	const struct font_dir *dir;
	FILE *file;
	char *path;

	img = load_image(name, error);
	if (img)
		return img;
	if (error)
		err = *error;
	for (dir = font_path; dir; dir = dir->next) {
		asprintf(&path, "%s/%s.xbm", dir->name, name);
		file = fopen(path, "r");
		if (!file)
			continue;
		img = load_image_file(file, error);
		fclose(file);
		return img;
	}
	if (error)
		*error = err;
	return NULL;
}


static struct font *load_font(const char *name, const char **error)
{
	struct image *img;

	img = find_font_image(name, error);
	if (!img)
		return NULL;
	return make_font(img, error);
}


static int do_edit(uint8_t *canvas, int width, int height,
    const struct edit *e, const char **error)
{
	struct image *img;
	struct font *font = NULL;
	int x = 0, y = 0, max = 0;
	int spc = 1;
	const char *s;
	int xo, yo;

	while (e) {
		switch (e->type) {
		case edit_string:
			if (!font) {
				font = load_font(DEFAULT_FONT, error);
				if (!font)
					goto fail;
			}
			for (s = e->u.s; *s; s++) {
				xo = draw_char(canvas, width, height,
				    font, *s, x, y);
				yo = char_height(font, *s);
				if (yo > max)
					max = yo;
				x += xo+spc;
			}
			break;
		case edit_font:
			free_font(font);
			font = load_font(e->u.s, error);
			if (!font)
				goto fail;
			break;
		case edit_img:
			img = load_image(e->u.s, error);
			if (!img)
				return 0;
			xo = draw_image(canvas, width, height, img, x, y);
			free_image(img);
			x += xo;
			break;
		case edit_spc:
			spc = e->u.n;
			break;
		case edit_xoff:
			x += e->u.n;
			break;
		case edit_xpos:
			x = e->u.n;
			break;
		case edit_yoff:
			y += e->u.n;
			break;
		case edit_ypos:
			y = e->u.n;
			break;
		case edit_nl:
			x = 0;
			y += max+1;
			max = 0;
			break;
		default:
			abort();
		}
		e = e->next;
	}
	return 1;

fail:
	free_font(font);
	return 0;
}


void *apply_edits(int width, int height, const struct edit *e,
    const char **error)
{
	uint8_t *canvas;

	canvas = calloc(1, (width*height+7) >> 3);
	if (!canvas)
		abort();
	if (do_edit(canvas, width, height, e, error))
		return canvas;
	free(canvas);
	return NULL;
}


/* ----- Compile editing instructions -------------------------------------- */


static char *alloc_string_n(const char *s, size_t len)
{
	char *t;

	t = alloc_size(len+1);
	memcpy(t, s, len);
	t[len] = 0;
	return t;
}


static void add_string(struct edit ***last, const char *start, size_t len)
{
	struct edit *e;

	e = alloc_type(struct edit);
	e->type = edit_string;
	e->u.s = alloc_string_n(start, len);
	**last = e;
	*last = &e->next;
}


static int parse_coord(struct edit *e, const char *s,
    enum edit_type off, enum edit_type pos)
{
	char *end;

	if (!*s)
		return 0;
	e->u.n = strtoul(s+1, &end, 0);
	if (*end != '>')
		return 0;
	switch (*s) {
	case '-':
		e->u.n = -e->u.n;
		/* fall through */
	case '+':
		e->type = off;
		break;
	case '=':
		e->type = pos;
		break;
	default:
		return 0;
	}
	return 1;
}


struct edit *text2edit(const char *s)
{
	struct edit *edits = NULL, *e;
	struct edit **last = &edits;
	const char *start;
	int have_text = 0;
	char *end;

	start = s;
	for (start = s; *s; s++) {
		if (*s != '<' && *s != '\n')
			continue;
		if (s != start) {
			add_string(&last, start, s-start);
			have_text = 1;
		}
		start = s+1;

		if (*s == '\n' && !have_text)
			continue;

		e = alloc_type(struct edit);
		e->type = edit_nl; /* pick something without data */
		e->next = NULL;
		*last = e;
		last = &e->next;

		if (*s == '\n') {
			have_text = 0;
			continue;
		}

		end = strchr(s, '>');
		if (!end)
			goto fail;

		switch (s[1]) {
		case 'F':
			if (strncmp(s, "<FONT ", 6))
				goto fail;
			e->type = edit_font;
			e->u.s = alloc_string_n(s+6, end-s-6);
			break;
		case 'I':
			if (strncmp(s, "<IMG ", 5))
				goto fail;
			e->type = edit_img;
			e->u.s = alloc_string_n(s+5, end-s-5);
			break;
		case 'S':
			if (strncmp(s, "<SPC ", 5))
				goto fail;
			e->type = edit_spc;
			e->u.n = strtoul(s+5, &end, 0);
			if (*end != '>')
				goto fail;
			break;
		case 'X':
			if (!parse_coord(e, s+2, edit_xoff, edit_xpos))
				goto fail;
			break;
		case 'Y':
			if (!parse_coord(e, s+2, edit_yoff, edit_ypos))
				goto fail;
			break;
		default:
			goto fail;
		}
		s = end;
		start = s+1;
	}
	if (s != start)
		add_string(&last, start, s-start);
	return edits;

fail:
	free_edit(e);
	return NULL;
}


/* ----- Free edit list ---------------------------------------------------- */


void free_edit(struct edit *e)
{
	struct edit *next;

	while (e) {
		if (e->type == edit_font || e->type == edit_img)
			free(e->u.s);
		next = e->next;
		free(e);
		e = next;
	}
}
