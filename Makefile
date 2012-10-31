PREFIX=/usr
BINDIR=$(PREFIX)/bin

CC=gcc
INSTALL=ginstall

all:	modemtest
distclean:	clean

clean:
	rm modemtest


install: all
	$(INSTALL) -D modemtest $(DESTDIR)$(BINDIR)/modemtest

macmask:
	$(CC) modemtest.c -o modemtest
