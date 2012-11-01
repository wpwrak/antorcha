/*
 * tools/ant-gui/ant-gui.c - Antorcha GUI
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

#include "SDL.h"
#include "SDL_gfxPrimitives.h"

#include <libant/libant.h>


#define	XRES	320	/* canvas width */
#define	YRES	240	/* canvas height */

#define	W	80	/* image width */
#define	H	16	/* image height */
#define	GAP	6

#define	NX	((XRES+GAP)/(W+GAP))
#define	NY	((YRES+GAP)/(H+GAP))


#define	FG_SEL_RGBA	0x000000ff
#define	BG_SEL_RGBA	0xffffffff
#define	FG_NORM_RGBA	0xffffffff
#define	BG_NORM_RGBA	0x000000ff
#define	FG_ACT_RGBA	0xff2020ff


static void render(SDL_Surface *s, const uint8_t *img, int x, int y,
    int sel, int act)
{
	int ix, iy;

	uint32_t fg = act ? FG_ACT_RGBA : sel ? FG_SEL_RGBA : FG_NORM_RGBA;
	uint32_t bg = sel ? BG_SEL_RGBA : BG_NORM_RGBA;

	for (iy = 0; iy != H; iy++)
		for (ix = 0; ix != W; ix++)
			if (img[(iy*W+ix) >> 3] & (1 << (ix & 7)))
				pixelColor(s, x+ix, y+iy, fg);
			else
				pixelColor(s, x+ix, y+iy, bg);
}


static void gui(uint8_t *const *imgs, int n)
{
	SDL_Surface *surf;
	SDL_Event event;
	int x = 0, y = 0, y_top = 0;
	int dx, dy, upload;
	int ix, iy, i;
	int active = -1;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL_init: %s\n", SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);

	surf = SDL_SetVideoMode(XRES, YRES, 0, SDL_SWSURFACE);
	if (!surf) {
		fprintf(stderr, "SDL_SetVideoMode: %s\n", SDL_GetError());
		exit(1);
	}

	while (1) {
		dx = dy = upload = 0;
		SDL_WaitEvent(&event);
		switch (event.type) {
		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_UP:
				if (y > 0)
					dy = -1;
				break;
			case SDLK_DOWN:
				if ((y+1)*NX+x < n)
					dy = 1;
				break;
			case SDLK_RIGHT:
				dx = 1;
				break;
			case SDLK_LEFT:
				dx = -1;
				break;
			case SDLK_RETURN:
				upload = 1;
				break;
			case SDLK_q:
				return;
			default:
				break;
			}
			break;
		case SDL_QUIT:
			return;
		default:
			break;
		}

		x = (x+NX+dx) % NX;
		y += dy;
		if (y*NX+x >= n)
			x = n-y*NX-1;
		if (y < y_top)
			y_top = y;
		if (y >= y_top+NY)
			y_top = y-NY+1;

		if (upload) {
			i = y*NX+x;
			active = i;
		}

		SDL_LockSurface(surf);
	        SDL_FillRect(surf, NULL, SDL_MapRGB(surf->format, 0, 0, 0));
		for (iy = 0; iy != NY; iy++)
			for (ix = 0; ix != NX; ix++) {
				i = iy*NX+ix;
				if (i >= n)
					break;
				render(surf, imgs[i],
				    ix*(W+GAP), (iy-y_top)*(H+GAP),
				    ix == x && iy == y, i == active);
			}
		SDL_UnlockSurface(surf);
		SDL_UpdateRect(surf, 0, 0, 0, 0);
	}
}


static void flush_edits(uint8_t ***imgs, int *n, struct edit *edits)
{
	const char *err;

	if (!edits)
		return;
	*imgs = realloc(*imgs, sizeof(uint8_t **)*(*n+1));
	if (!*imgs) {
		perror("realloc");
		exit(1);
	}
	(*imgs)[*n] = apply_edits(W, H, edits, &err);
	if (!(*imgs)[*n]) {
		fprintf(stderr, "%s\n", err);
		exit(1);
	}
	(*n)++;
	free_edits(edits);
}


static void generate(uint8_t ***imgs, int *n, const char *path)
{
	FILE *file;
	char buf[100];
	struct edit *edits = NULL, **last = &edits;
	const char *err;
	char *nl;

	file = fopen(path, "r");
	if (!file) {
		perror(path);
		exit(1);
	}
	while (fgets(buf, sizeof(buf), file)) {
		if (buf[0] == '#')
			continue;
		nl = strchr(buf, '\n');
		if (nl)
			*nl = 0;
		if (!buf[0]) {
			flush_edits(imgs, n, edits);
			edits = NULL;
			last = &edits;
			continue;
		}
		if (edits) {
			*last = malloc(sizeof(struct edit));
			if (!*last)
				abort();
			(*last)->type = edit_nl;
			last = &(*last)->next;
		}
		*last = text2edit(buf, &err);
		if (!*last) {
			fprintf(stderr, "\"%s\": %s\n", buf, err);
			exit(1);
		}
		while (*last)
			last = &(*last)->next;
	}
	fclose(file);
	flush_edits(imgs, n, edits);
}


static void usage(const char *name)
{
	fprintf(stderr, "usage: %s [-F font_dir ...] file ...\n", name);
	exit(1);
}


int main(int argc, char **argv)
{
	uint8_t **imgs = NULL;
	int n = 0;
	int i, c;

	while ((c = getopt(argc, argv, "F:")) != EOF)
		switch (c) {
		case 'F':
			add_font_dir(optarg);
			break;
		default:
			usage(*argv);
		}

	if (optind == argc)
		usage(*argv);

	for (i = optind; i != argc; i++)
		generate(&imgs, &n, argv[i]);
	gui(imgs, n);
	exit(1);
}
