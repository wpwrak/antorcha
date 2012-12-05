CC = mipsel-openwrt-linux-gcc
CFLAGS = -g -Wall -I.. -I.
OBJS = ben.o mmc.o mmc-hw.o

.PHONY:		all ben clean spotless

vpath %.c ..

all:		ben

ben:		$(OBJS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f ben
