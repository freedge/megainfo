
CFLAGS ?= -Wall -O3 -pedantic -Werror -std=gnu17
LDFLAGS ?= -O3
all: megainfo

megainfo: megainfo.o

clean:
	rm -f megainfo megainfo.o
