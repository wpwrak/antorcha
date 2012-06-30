#
# tools/ant-txt/Makefile - Build the Antorcha text composer
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = ant-txt

CFLAGS = -g -Wall -I.. \
	 $(if $(BEN), -static)

LDLIBS = -L../libtxt -ltxt

OBJS = ant-txt.o

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
