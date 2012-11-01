#
# tools/libant/Makefile - Build the Antorcha library
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


LIB = libant.a

# _GNU_SOURCE for vasprintf
CFLAGS = -g -Wall -D_GNU_SOURCE

OBJS = edit.o font.o dump.o

.PHONY:		all ben clean spotless

all:		$(LIB)

ben:
		$(MAKE) CC=mipsel-openwrt-linux-gcc

$(LIB):		$(OBJS)
		$(AR) cr $@ $^

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f $(LIB)
