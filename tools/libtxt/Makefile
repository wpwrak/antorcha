#
# tools/libtxt/Makefile - Build the Antorcha tool
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = libtxt.a

CFLAGS = -g -Wall

OBJS = font.o

.PHONY:		all clean spotless

all:		$(MAIN)

$(MAIN):	$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(MAIN)
