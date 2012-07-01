#
# tools/ant-cl/Makefile - Build the Antorcha command-line tool
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = ant-cl

CFLAGS = -g -I../../fw -I../../../ben-wpan/tools/include \
	 -I../../../ben-wpan/atusb/fw/include \
	 -Wall \
	 $(if $(NOGFX),, -DGFX $(shell sdl-config --cflags)) \
	 $(if $(BEN), -static)

LDLIBS = -L../../../ben-wpan/tools/lib -latrf  \
	 $(if $(NOGFX),, $(shell sdl-config --libs) -lSDL_gfx) \
	 $(if $(BEN),, -lusb)

OBJS = ant-cl.o hash.o $(if $(NOGFX),, plot.o)

.PHONY:		all ben clean spotless update ping off

all:		$(MAIN)

ben:
		$(MAKE) NOGFX=y BEN=y CC=mipsel-openwrt-linux-gcc

$(MAIN):	$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(MAIN)

update:
		./ant-cl -F ../../fw/antorcha.bin

ping:
		./ant-cl -P

off:
		./ant-cl /dev/null
