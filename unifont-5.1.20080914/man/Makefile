
MANPAGES = bdfimplode.1 hex2bdf.1 hex2bdf-split.1 hex2sfd.1 \
	hexbraille.1 hexdraw.1 hexmerge.1 johab2ucs2.1 \
	unibmp2hex.1 unicoverage.1 unidup.1 unihex2bmp.1 \
	unipagecount.1 uniunmask.1

#
# $(PREFIX) is defined in the top-level Makefile
#
INSTALLDIR = $(PREFIX)/share/man/man1

all:

install: $(MANPAGES)
	install -m0755 -d $(INSTALLDIR)
	install -m0644 $(MANPAGES) $(INSTALLDIR)

clean:

distclean:

.PHONY: all install clean distclean
