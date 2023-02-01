
CFLAGS = -g -O2 -Wall

INSTALLBINDIR = ../bin

#
# $(PREFIX) is defined in top-level directory, so don't do a
# 'make install' within this directory -- do it from above.
#
INSTALLDIR = $(PREFIX)/bin

#
# Programs written in C.
#
CPROGS = unibmp2hex unicoverage unidup unihex2bmp \
	 unipagecount uniunmask

#
# Programs written in Perl.
#
PPROGS = bdfimplode hex2bdf hex2bdf-split hex2sfd hexbraille hexdraw hexmerge \
	 johab2ucs2

PROGS = $(CPROGS) $(PPROGS)


all: $(CPROGS)
	install -m0755 -d $(INSTALLBINDIR)
	install -s -m0755 $(CPROGS) $(INSTALLBINDIR)
	install    -m0755 $(PPROGS) $(INSTALLBINDIR)
	rm -f $(CPROGS)

install:
	if [ x$(PREFIX) = x ] ; \
	then \
	   echo "Fatal error: PREFIX directory is not defined." ; \
	   echo "Run 'make install' from top-level directory" ; \
	   exit 1 ; \
	fi
	install -m0755 -d $(INSTALLDIR)
	set -e ; \
	   cd $(INSTALLBINDIR) ; \
	   install -s -m0755 $(CPROGS) $(INSTALLDIR)
	set -e ; \
	   cd ../bin ; \
	   install    -m0755 $(PPROGS) $(INSTALLDIR)

#
# The *.exe is for cygwin; it does nothing elsewhere.
#
clean:
	rm -f $(CPROGS)
	rm -f *.exe

distclean: clean


