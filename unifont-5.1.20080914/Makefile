#
# unifoundry.com utilities for the GNU Unifont
#
# Typing "make && make install" will make and
# install the binary programs and man pages.
# To build the font from scratch, use
# "cd font ; make"
#
SHELL = /bin/sh

#
# To install under /usr/local, choose the first PREFIX definition.
# For Debian, or to install binaries under /usr/bin and man pages
# under /usr/share/man/man1, choose the second PREFIX definition.
#
# PREFIX=/usr/local
PREFIX = $(DESTDIR)/usr

DIRS = bin man font

all: bindir
	echo "Make is done."

bindir:
	set -e ; $(MAKE) -C src

mandir:
	set -e ; $(MAKE) -C man

fontdir:
	set -e ; $(MAKE) -C font

install: bin
	$(MAKE) -C src install PREFIX=$(PREFIX)
	$(MAKE) -C man install PREFIX=$(PREFIX)
	$(MAKE) -C font install PREFIX=$(PREFIX) DESTDIR=$(DESTDIR)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C man clean
	$(MAKE) -C font clean

distclean:
	$(MAKE) -C src distclean
	$(MAKE) -C man distclean
	$(MAKE) -C font distclean
	rm -rf bin

.PHONY: all install clean distclean
