prefix = /usr/local
libdir = $(prefix)/lib
includedir = $(prefix)/include
DESTDIR =
BLDDIR = build

CFLAGS = -g -Wall -O2
LDFLAGS =
ALL_CFLAGS = -fPIC $(CFLAGS)
ALL_LDFLAGS = -lm $(LDFLAGS)

INSTALL = install
CC = gcc

all: testbloomfilter
testbloomfilter: test_dabloom.c
	@echo " LD " $@
	$(CC) -o $@ $(ALL_CFLAGS) test_dabloom.c $(libdir)/libdablooms.a $(ALL_LDFLAGS)
clean:
	rm -rf testbloomfilter
