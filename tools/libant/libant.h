/*
 * tools/libant/libant.h - Text processing functions
 *
 * Written 2012 by Werner Almesberger
 * Copyright 2012 Werner Almesberger
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 */

#ifndef LIBANT_H
#define LIBANT_H

#include <stdio.h>


struct image;

struct font;

struct edit {
	enum edit_type {
		edit_string,
		edit_font,
		edit_img,
		edit_spc,
		edit_xoff,
		edit_xpos,
		edit_yoff,
		edit_ypos,
		edit_nl,
	} type;
	union {
		char *s;
		int n;
	} u;
	struct edit *next;
};

/*
 * Markup:
 *
 * <FONT fontname>
 * <IMG imagefile>
 * <SPC offset>
 * <X+offset> <X-offset> <X=pos>
 * <Y+offset> ...
 *
 * Empty lines are ignored. The newline of a line containing only markup is
 * also ignored.
 *
 * Newline leaves one blank row between text lines.
 */

struct image *load_image_file(FILE *file, const char **error);
struct image *load_image(const char *name, const char **error);
void free_image(struct image *img);

int draw_image(void *canvas, int width, int height,
    const struct image *img, int x, int y);

struct font *make_font(struct image *img, const char **error);
void free_font(struct font *font);

int draw_char(void *canvas, int width, int height,
    const struct font *font, char c, int x, int y);
int char_height(const struct font *font, char c);

struct edit *text2edit(const char *s, const char **error);
char *edit2text(const struct edit *e);
void free_edits(struct edit *e);

void add_font_dir(const char *name);

void *apply_edits(int width, int height, const struct edit *e,
    const char **error);

int dump_binary(FILE *file, const void *canvas, int width, int height);
int dump_xbm(FILE *file, const void *canvas, int width, int height);
int dump_ascii(FILE *file, const void *canvas, int width, int height);

#endif /* !LIBANT_H */
