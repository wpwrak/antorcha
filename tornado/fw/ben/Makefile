CC = mipsel-openwrt-linux-gcc
CFLAGS = -g -Wall -I.. -I.
LDFLAGS = -static
OBJS = ben.o mmc.o mmc-hw.o

.PHONY:		all clean spotless

vpath %.c ..

all:		ben

ben:		$(OBJS)

clean:
		rm -f $(OBJS)

spotless:	clean
		rm -f ben
