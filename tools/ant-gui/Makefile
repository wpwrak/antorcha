#
# tools/ant-gui/Makefile - Build the Antorcha GUI
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = ant-gui

CFLAGS = -g -Wall -I.. $(shell sdl-config --cflags) \
	 $(if $(BEN), -static)

LDLIBS = -L../libant -lant $(shell sdl-config --libs) -lSDL_gfx

OBJS = ant-gui.o

.PHONY:		all ben clean spotless

all:		$(MAIN)

ben:
		$(MAKE) BEN=y CC=mipsel-openwrt-linux-gcc

$(MAIN):	$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(MAIN)
