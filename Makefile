
CC ?= cc
CFLAGS  += -fPIC

.PHONY:
.PHONY: all clean install

.SUFFIXES:
.SUFFIXES: .c .h

PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
MANDIR = $(PREFIX)/share/man/man1

all: rbatch rbatchd

install: all
	install -d $(BINDIR)
	install -m 750 -s rbatch rbatchd $(BINDIR)
	install -d $(MANDIR)
	install -m 644 rbatch.1 rbatchd.1 $(MANDIR)

rbatch: rbatch.c common.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

rbatchd: rbatchd.c common.o
	$(CC) $(CFLAGS) -o $@ $(LDFLAGS) $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o rbatch rbatchd
