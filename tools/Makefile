#
# tools/Makefile - Build the Antorcha tool
#
# Written 2012 by Werner Almesberger
# Copyright 2012 Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#


MAIN = antorcha

CFLAGS = -g -I../fw -I../../ben-wpan/tools/include \
	 -I../../ben-wpan/atusb/fw/include \
	 -Wall
LDLIBS = -L../../ben-wpan/tools/lib -latrf -lusb

OBJS = antorcha.o hash.o

.PHONY:		all update ping

all:		$(MAIN)

$(MAIN):	$(OBJS)
		$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

update:
		./antorcha -F ../fw/antorcha.bin

ping:
		./antorcha -P
