#
# Makefile - Makefile of the Whirl firmware 
#
# Written 2012 by Werner Almesberger
# Copyright 2012 by Werner Almesberger
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

SHELL = /bin/bash

NAME = whirl

CFLAGS = -g -mmcu=$(CHIP) \
	 -DBOOT_ADDR=$(BOOT_ADDR) \
	 -Wall -Wextra -Wshadow -Wno-unused-parameter \
	 -Wmissing-prototypes -Wmissing-declarations -Wstrict-prototypes \
	 -I../../ben-wpan/atusb/fw/include/
#	 -Wall -Wextra -Wshadow -Werror -Wno-unused-parameter \

CHIP = atmega168
HOST = jlime
BOOT_ADDR = 0x3800

AVR_PREFIX = $(BIN_PATH) avr-
CC = $(AVR_PREFIX)gcc
OBJCOPY = $(AVR_PREFIX)objcopy
#OBJDUMP = $(AVR_PREFIX)objdump
SIZE = $(AVR_PREFIX)size

OBJS = $(NAME).o $(COMMON_OBJS)
BOOT_OBJS = boot.o $(COMMON_OBJS)
COMMON_OBJS =

# ----- Verbosity control -----------------------------------------------------

CC_normal	:= $(CC)
BUILD_normal	:=
DEPEND_normal	:= $(CPP) $(CFLAGS) -MM -MG

CC_quiet	= @echo "  CC       " $@ && $(CC_normal)
BUILD_quiet	= @echo "  BUILD    " $@ && $(BUILD_normal)
DEPEND_quiet	= @$(DEPEND_normal)

ifeq ($(V),1)
    CC		= $(CC_normal)
    BUILD	= $(BUILD_normal)
    DEPEND	= $(DEPEND_normal)
else
    CC		= $(CC_quiet)
    BUILD	= $(BUILD_quiet)
    DEPEND	= $(DEPEND_quiet)
endif

# ----- Rules -----------------------------------------------------------------

.PHONY:		all clean nosecrets upload prog version.c
.PHONY:		prog-app prog-read

all:		$(NAME).bin boot.bin

$(NAME).elf:	$(OBJS)
		$(MAKE) version.o
		$(CC) $(CFLAGS) -o $@ $(OBJS) version.o

boot.elf:	$(BOOT_OBJS)
		$(CC) $(CFLAGS) -o $@ $(BOOT_OBJS) \
		  -Wl,--section-start=.text=$(BOOT_ADDR)

%.bin:		%.elf
		$(BUILD) $(OBJCOPY) -j .text -j .data -O binary $< $@
		@echo "build #`cat .version`, `ls -l $@`"

%.hex:		%.elf
		$(BUILD) $(OBJCOPY) -j .text -j .data -O ihex $< $@
		$(SIZE) $@

# ----- Cleanup ---------------------------------------------------------------

clean:
		rm -f $(NAME).bin $(NAME).elf
		rm -f $(OBJS) $(OBJS:.o=.d)
		rm -f boot.hex boot.elf
		rm -f $(BOOT_OBJS) $(BOOT_OBJS:.o=.d)
		rm -f version.c version.d version.o

# ----- Build version ---------------------------------------------------------

version.c:
		@if [ -f .version ]; then \
		    v=`cat .version`; \
		    expr $$v + 1 >.version; \
		else \
		    echo 0 >.version; \
		fi
		@[ -s .version ] || echo 0 >.version
		@echo '/* MACHINE-GENERATED. DO NOT EDIT ! */' >version.c
		@echo '#include "version.h"' >>version.c
		@echo "const char *build_date = \"`date`\";" >>version.c
		@echo "const uint16_t build_number = `cat .version`;" \
		  >>version.c

# ----- Dependencies ----------------------------------------------------------

MKDEP =									\
	$(DEPEND) $< |							\
	  sed 								\
	    -e 's|^$(basename $(notdir $<)).o:|$@:|'			\
	    -e '/^\(.*:\)\? */{p;s///;s/ *\\\?$$/ /;s/  */:\n/g;H;}'	\
	    -e '$${g;p;}'						\
	    -e d >$(basename $@).d;					\
	  [ "$${PIPESTATUS[*]}" = "0 0" ] ||				\
	  { rm -f $(basename $@).d; exit 1; }

%.o:            %.c
		$(CC) $(CFLAGS) -Os -c $<
		$(MKDEP)

-include $(OBJS:.o=.d)

# ----- Programming and device control ----------------------------------------

upload:		$(NAME).bin boot.hex
		scp $(NAME).bin boot.hex ../../common/avrdude-antorcha.conf \
		  $(HOST):

# lfuse: external clock, slow start-up
# hfuse: 4 kB boot loader, reset into boot loader
# lock: allow everything but SPM to the boot loader
#       Note: when trying to program 0xef, we get back 0x2f, failing
#	      verification. So we just program 0x2f.

prog-app:
		ssh $(HOST) avrdude -F -p $(CHIP) \
		  -L avrdude-antorcha.conf -c nanonote_antorcha -e \
		  -U flash:w:antorcha.bin:r
#		  -U lfuse:w:0x60:m

prog:
		ssh $(HOST) avrdude -F -p $(CHIP) \
		  -L avrdude-antorcha.conf -c nanonote_antorcha -e \
		  -U flash:w:boot.hex:i \
		  -U efuse:w:0x00:m \
		  -U lfuse:w:0xe2:m
#		  -U lfuse:w:0x60:m \
#		  -U hfuse:w:0xd8:m \
#		  -U lock:w:0x2f:m

prog-read:
		ssh $(HOST) avrdude -F -p $(CHIP) -c nanonote_antorcha \
		  -U flash:r:mcu.bin:r
