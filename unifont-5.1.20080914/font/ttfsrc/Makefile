
SHELL = /bin/sh

BINDIR = ../../bin

#
# Luis' original scripts were invoked with "-lang=ff" on the command
# line.  This was not supported by the version of FontForge in
# the current Debian stable release (Etch), so it was removed.
# However, the latest (as of July 2008) version of Debian (Sid)
# needed the language defined.  Setting FONTFORGE_LANGUAGE gets
# around this conflict between the old and new FontForge command
# line options -- thanks to Anthony Fok for this solution.
#
export FONTFORGE_LANGUAGE = ff

#
# How to build unifont.ttf from GNU Unifont's unifont.hex
# -------------------------------------------------------
# Written by Luis Alejandro Gonzalez Miranda - http://www.lgm.cl/
#

#
# 2008 - Paul Hardy created this Makefile from Luis' original
# howto-build.sh and other bash scripts.  Those original scripts
# don't appear in this archive, but they can be retrieved from
# http://www.lgm.cl/.
#

# First of all, you need a Perl interpreter and FontForge.
#
# I don't remember all the steps, but I think it was as described by
# this script.

# This division is done only so the Simplify and RemoveOverlap
# operations don't use up too much memory, and because
# a .sfd generated from the whole unifont.hex would be too big to
# process all at once.

all: hex2sfd merge sfd2ttf

hex2sfd: unifont.hex $(BINDIR)/hex2sfd
	echo "Separating unifont.hex into 16 parts"
	for i in 0 1 2 3 4 5 6 7 8 9 A B C D E F ; \
	   do \
	   grep ^$$i unifont.hex > $$i.hex ; \
	   $(BINDIR)/hex2sfd < $$i.hex > $$i.sfd ; \
	done
	for i in 0 1 2 3 4 5 6 7 8 9 A B C D E F ; \
	   do \
	   fontforge -c 'Open($$1); SelectAll(); RemoveOverlap(); Simplify(-1,1); Save($$1);' $$i.sfd ; \
	   rm -f $$i.hex ; \
	done

# We join all the generated .sfd files on a single one,
# and generate the final TrueType font

merge:
	echo "Merging all .sfd files"
	fontforge -script all.pe

sfd2ttf:
	echo "Converting .sfd font into .ttf font"
	fontforge -c 'Open($$1); Generate($$2)' unifont.sfd unifont.ttf

#
# This fontforge script reads a BDF font file and generates an SBIT font file.
# Author: written by Qianqian Fang, given to Paul Hardy in 2008.
# Right now nothing calls this; it is just here for experimentation.
# The SBIT font is far smaller than the default outline TrueType font
# and takes far less time to build than the outline font.  However, it
# isn't scalable.  An SBIT font could be created and merged with the
# larger TTF font using fontforge, but I (Paul Hardy) haven't noticed
# any degradation in the screen rendering of just the outline TTF font
# that this Makefile produces as its final product.  This is with
# daily use of this Makefile's default TrueType font.  Note that there
# is no file in this directory called "unifont.bdf" -- copy a BDF font
# here from ../compiled and name it "unifont.bdf".
#
sbit:
	fontforge -c 'New(); Import($$1); Generate($$2, "ttf"); Close()' \
		unifont.bdf unifont-sbit.ttf

clean:
	rm -f [0-9A-F].hex
	rm -f [0-9A-F].sfd

distclean: clean
	rm -f unifont.bdf
	rm -f unifont.hex
	rm -f unifont.sfd
	rm -f unifont.ttf
	rm -f unifont-sbit.ttf

.PHONY: all hex2sfd merge sfd2ttf sbit clean distclean
