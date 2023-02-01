#
# Makefile to build a version of GNU Unifont with the
# unifoundry.com GNU Unifont utilities.  This Makefile
# converts unifont.hex and related files into a final
# GNU Unifont font file.
#
# This software is released under version 2.0 of the GNU Public License,
# or (at your option) a later version of the GPL.
#
# Paul Hardy, 6 July 2008
#
# commands will be interpreted with the Bourne Shell
#
SHELL = /bin/sh
#
# assembly date of this version
#
VERSION = 5.1.20080907
#
# Path to local unifont-bin utilities.  If your system doesn't
# use GNU's version of "make", use the first BINDIR definition.
# If using GNU's "make", use the second BINDIR definition.
#
# BINDIR = ../bin
BINDIR = $(CURDIR)/../bin
#
# The top-level directory for installing fonts on the system,
# and the installation directories for PCF and TrueType fonts.
#
PKGDEST = $(DESTDIR)/usr/share/unifont
FONTDEST = $(DESTDIR)/usr/share/fonts
PCFDEST = $(FONTDEST)/X11/misc
TTFDEST = $(FONTDEST)/truetype/unifont
#
# destination directory for compiled fonts
#
BDFDIR = compiled
#
# Directory with original unifont.hex files
#
HEXDIR = hexsrc
#
# These are the files for building GNU Unifont with the Qianqian Fang's
# Wen Quan Yi CJK ideographs.  This version provides complete coverage
# of the Unicode 5.1 Basic Multilingual Plane.
#
UNIFILES = $(HEXDIR)/blanks.hex $(HEXDIR)/rc-base.hex $(HEXDIR)/wqy-cjk.hex \
	$(HEXDIR)/rc-hangul.hex $(HEXDIR)/rc-priv.hex
#
# These are the files for building GNU Unifont with Roman Czyborra's
# original Japanese public domain CJK ideographs.  This version is not
# complete, as it is missing thousands of CJK ideographs.
#
UNIORIGFILES = $(HEXDIR)/blanks.hex $(HEXDIR)/rc-base.hex \
	$(HEXDIR)/rc-cjk.hex $(HEXDIR)/rc-hangul.hex $(HEXDIR)/rc-priv.hex
#
# Location of  TTF source directory, where TTF font is built.
#
TTFSRC = ttfsrc

#	egrep '^*:0+$$' < $(HEXDIR)/substitutes.hex \
#			> $(TTFSRC)/spaces.hex

all: pcf bmp-final bmp-ref ttf
	cp -p index.html compiled/index.html

#
# Build the aggregate .hex font files
#
hex: distclean $(BINDIR)/uniunmask
	if [ ! -d $(BDFDIR) ] ; then \
	   mkdir -p $(BDFDIR) ; \
	fi
	cat $(UNIORIGFILES) | sort >$(BDFDIR)/unifontfull-jp-$(VERSION).hex
	cat $(UNIFILES) | sort >$(BDFDIR)/unifontfull-$(VERSION).hex
	cp $(HEXDIR)/masks.hex $(BDFDIR)
	cp $(HEXDIR)/substitutes.hex $(BDFDIR)
	cd $(BDFDIR) ; \
	$(BINDIR)/uniunmask < unifontfull-jp-$(VERSION).hex \
	          > unifont-jp-$(VERSION).hex
	cd $(BDFDIR) ; \
	$(BINDIR)/uniunmask < unifontfull-$(VERSION).hex \
	          > unifont-$(VERSION).hex
	rm -f $(BDFDIR)/masks.hex
	rm -f $(BDFDIR)/substitutes.hex

#
# Build a BDF font file from the normal .hex file (i.e., the .hex file
# with complete Basic Multilingual Plane coverage with the combining
# circles and other non-printing graphics removed).
#
bdf: hex $(BINDIR)/hex2bdf
	$(BINDIR)/hex2bdf <$(BDFDIR)/unifont-$(VERSION).hex \
		>$(BDFDIR)/unifont-$(VERSION).bdf
	gzip <$(BDFDIR)/unifont-$(VERSION).bdf >$(BDFDIR)/unifont-$(VERSION).bdf.gz

#
#
# Build a BDF font file from the normal .hex file (i.e., the .hex file
# with complete Basic Multilingual Plane coverage with the combining
# circles and other non-printing graphics removed).
#
pcf: bdf
	bdftopcf <$(BDFDIR)/unifont-$(VERSION).bdf \
			>$(BDFDIR)/unifont-$(VERSION).pcf
	gzip -9 $(BDFDIR)/unifont-$(VERSION).pcf

#
# Print coverage of scripts in Basic Multilingual Plane in .txt file.
#
coverage: $(BDFDIR)/unifontfull-$(VERSION).hex $(BINDIR)/unicoverage
	$(BINDIR)/unicoverage < $(BDFDIR)/unifontfull-$(VERSION).hex \
		    > $(BDFDIR)/coverage.txt

#
# Print HTML page coverage in Basic Multilingual Plane in .txt file.
#
pagecount: $(BDFDIR)/unifontfull-$(VERSION).hex $(BINDIR)/unipagecount
	$(BINDIR)/unipagecount -l < $(BDFDIR)/unifontfull-$(VERSION).hex \
		        > $(BDFDIR)/pagecount.html

#
# Create the .bmp (Windows Bitmap) graphics versions of the glyphs.
# The $(BDFDIR)/bmp directory will contain files from the normal
# font file used for rendering.  The $(BDFDIR)/bmp-full directory
# will contain files from the full font file, including non-printing
# graphics (combining circles, noncharacters, etc.).
#
bmp-final: hex $(BINDIR)/unihex2bmp
	if [ ! -d $(BDFDIR)/bmp-final ] ; then \
	   mkdir -p $(BDFDIR)/bmp-final ; \
	fi
	for i in 0 1 2 3 4 5 6 7 8 9 A B C D E F; do \
	   for j in 0 1 2 3 4 5 6 7 8 9 A B C D E F; do \
	      $(BINDIR)/unihex2bmp -f -p$$i$$j \
	         -i$(BDFDIR)/unifont-$(VERSION).hex \
	         -o$(BDFDIR)/bmp-final/uni$$i$$j.bmp ; \
	   done ; \
	done ; \
	echo "Done creating $(BDFDIR)/bmp-final"

bmp-ref: hex $(BINDIR)/unihex2bmp
	if [ ! -d $(BDFDIR)/bmp-ref ] ; then \
	   mkdir -p $(BDFDIR)/bmp-ref ; \
	fi
	for i in 0 1 2 3 4 5 6 7 8 9 A B C D E F; do \
	   for j in 0 1 2 3 4 5 6 7 8 9 A B C D E F ; do \
	      $(BINDIR)/unihex2bmp -f -p$$i$$j \
	         -i$(BDFDIR)/unifontfull-$(VERSION).hex \
	         -o$(BDFDIR)/bmp-ref/uni$$i$$j.bmp ; \
	   done ; \
	done ; \
	echo "Done creating $(BDFDIR)/bmp-ref"

#
# Note that $(TTFSRC) must exist, because it contains some source files
# Perform a "make && make clean" because ALL of the files would consume
# over 200 Megabytes if left around.  The .sfd file is about 100 Megabytes,
# and it is created from merging "[0-F].sfd", which take up another 100 MB.
#

ttf:
	cp $(BDFDIR)/unifont-$(VERSION).hex $(TTFSRC)/unifont.hex
	cd $(TTFSRC) ; make && make clean
	mv $(TTFSRC)/unifont.sfd $(BDFDIR)/unifont-$(VERSION).sfd 
	mv $(TTFSRC)/unifont.ttf $(BDFDIR)/unifont-$(VERSION).ttf 
	gzip -9 $(BDFDIR)/unifont-$(VERSION).sfd 

#
# This is set up for Debian.  Solaris places fonts in
# "/usr/share/fonts/TrueType".  Other unices use other directories.
# The original font format was BDF, but Debian doesn't use that and
# xfs should only need TrueType, so the BDF font isn't installed.
#
# The TrueType font is about 16 Megabytes.  To avoid duplication,
# the fonts are installed as symbolic links back to the original package.
# Alternatively, the fonts can be copied to the destination directory
# with "install -m0644" or moved there (but "mv" is a destructive
# one-time operation).
#
# After installing the new fonts, register them with X Window System using:
#
#      xset fp rehash
#
install: precompiled/unifont.ttf precompiled/unifont.pcf.gz
	if [ x$(CURDIR) = x ] ; \
	then \
	   echo "Fatal Error: CURDIR not defined -- define in Makefile." ; \
	   exit 1 ; \
	fi
	install -m0755 -d $(PKGDEST)
	install -m0755 -d $(PCFDEST)
	install -m0755 -d $(TTFDEST)
	install -m0644 -p $(CURDIR)/precompiled/unifont.hex     $(PKGDEST)/unifont.hex
	install -m0644 -p $(CURDIR)/precompiled/unifontfull.hex $(PKGDEST)/unifontfull.hex
	install -m0644 -p $(CURDIR)/precompiled/combining.dat   $(PKGDEST)/combining.dat
	install -m0644 -p $(CURDIR)/precompiled/README          $(PKGDEST)/README
	install -m0644 -p $(CURDIR)/precompiled/unifont.pcf.gz  $(PCFDEST)/unifont.pcf.gz
	install -m0644 -p $(CURDIR)/precompiled/unifont.ttf     $(TTFDEST)/unifont.ttf

clean:
	rm -rf $(BDFDIR)/bmp-final
	rm -rf $(BDFDIR)/bmp-ref
	rm -f $(BDFDIR)/unifontfull-jp-$(VERSION).hex
	rm -f $(BDFDIR)/unifontfull-$(VERSION).hex
	rm -f $(BDFDIR)/unifont-jp-$(VERSION).hex
	rm -f $(BDFDIR)/unifont-$(VERSION).hex
	rm -f $(BDFDIR)/unifont-$(VERSION).bdf
	rm -f $(BDFDIR)/unifont-$(VERSION).bdf.gz

distclean:
	rm -rf $(BDFDIR)
	$(MAKE) -C ttfsrc distclean

.PHONY: all install clean distclean hex bdf pcf coverage pagecount bmp-final bmp-ref ttf 
