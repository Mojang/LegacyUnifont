/*
   unibmp2hex - program to turn a .bmp or .wbmp glyph matrix into a
                GNU Unifont hex glyph set of 256 characters

   Synopsis: unibmp2hex [-iin_file.bmp] [-oout_file.hex] [-phex_page_num] [-w]


   Author: Paul Hardy, unifoundry <at> unifoundry.com, December 2007
   
   
    Copyright (C) 2007-2008 Paul Hardy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUF 256


unsigned hexdigit[16][4]; /* 32 bit representation of 16x8 0..F bitmap   */

unsigned uniplane=0;        /* Unicode plane number, 0..0xff ff ff       */
unsigned planeset=0;        /* =1: use plane specified with -p parameter */
unsigned flip=0;            /* =1 if we're transposing glyph matrix      */
unsigned forcewide=0;       /* =1 to set each glyph to 16 pixels wide    */

/* The six Unicode plane digits, from left-most (0) to right-most (5)    */
unsigned unidigit[6][4];



int main(int argc, char *argv[]) {

   int i, j, k;               /* loop variables                       */
   unsigned char inchar;       /* temporary input character */
   char header[MAXBUF];        /* input buffer for bitmap file header */
   int wbmp=0; /* =0 for Windows Bitmap (.bmp); 1 for Wireless Bitmap (.wbmp) */
   int fatal; /* =1 if a fatal error occurred */
   int match; /* =1 if we're still matching a pattern, 0 if no match */
   int empty1, empty2; /* =1 if bytes tested are all zeroes */
   unsigned char thischar1[16], thischar2[16]; /* bytes of hex char */
   int thisrow; /* index to point into thischar1[] and thischar2[] */
   int tmpsum;  /* temporary sum to see if a character is blank */

   unsigned char bitmap[17*32][18*32/8]; /* final bitmap */
   char wide[65536]={65536 * 0}; /* 1 = force double width code point */

   char *infile="", *outfile="";  /* names of input and output files */
   FILE *infp, *outfp;      /* file pointers of input and output files */

   if (argc > 1) {
      for (i = 1; i < argc; i++) {
         if (argv[i][0] == '-') {  /* this is an option argument */
            switch (argv[i][1]) {
               case 'i':  /* name of input file */
                  infile = &argv[i][2];
                  break;
               case 'o':  /* name of output file */
                  outfile = &argv[i][2];
                  break;
               case 'p':  /* specify a Unicode plane */
                  sscanf(&argv[i][2], "%x", &uniplane); /* Get Unicode plane */
                  planeset = 1; /* Use specified range, not what's in bitmap */
                  break;
               case 'w': /* force wide (16 pixels) for each glyph */
                  forcewide = 1;
                  break;
               default:   /* if unrecognized option, print list and exit */
                  fprintf(stderr, "\nSyntax:\n\n");
                  fprintf(stderr, "   %s -p<Unicode_Page> ", argv[0]);
                  fprintf(stderr, "-i<Input_File> -o<Output_File> -w\n\n");
                  fprintf(stderr, "   -w specifies .wbmp output instead of ");
                  fprintf(stderr, "default Windows .bmp output.\n\n");
                  fprintf(stderr, "   -p is followed by 1 to 6 ");
                  fprintf(stderr, "Unicode plane hex digits ");
                  fprintf(stderr, "(default is Page 0).\n\n");
                  fprintf(stderr, "\nExample:\n\n");
                  fprintf(stderr, "   %s -p83 -iunifont.hex -ou83.bmp\n\n\n",
                         argv[0]);
                  exit(1);
            }
         }
      }
   }
   /*
      Make sure we can open any I/O files that were specified before
      doing anything else.
   */
   if (strlen(infile) > 0) {
      if ((infp = fopen(infile, "r")) == NULL) {
         fprintf(stderr, "Error: can't open %s for input.\n", infile);
         exit(1);
      }
   }
   else {
      infp = stdin;
   }
   if (strlen(outfile) > 0) {
      if ((outfp = fopen(outfile, "w")) == NULL) {
         fprintf(stderr, "Error: can't open %s for output.\n", outfile);
         exit(1);
      }
   }
   else {
      outfp = stdout;
   }
   /*
      Initialize selected code points for double width (16x16).
      0 ==> 8 pixels wide; 1 ==> 16 pixels wide.
   */
   for (i = 0x1100; i <= 0x11FF; i++) wide[i] = 1; /* Hangul Jamo */
   for (i = 0x2580; i <= 0x259F; i++) wide[i] = 1; /* Block Elements */
   for (i = 0x2E80; i <= 0xA4CF; i++) wide[i] = 1; /* CJK */

   wide[0x303F] = 0; /* CJK half-space fill */

   for (i = 0xFF01; i <= 0xFF60; i++) wide[i] = 1; /* Fullwidth ASCII */
   for (i = 0xFFE0; i <= 0xFFE6; i++) wide[i] = 1; /* Fullwidth Symbol */


   /*
      Determine whether or not the file is a Microsoft Windows Bitmap file.
      If it starts with 'B', 'M', assume it's a Windows Bitmap file.
      Otherwise, assume it's a Wireless Bitmap file.

      WARNING: There isn't much in the way of error checking here --
      if you give it a file that wasn't first created by hex2bmp.c,
      all bets are off.
   */
   fatal = 0;  /* assume everything is okay with reading input file */
   if ((header[0] = fgetc(infp)) != EOF) {
      if ((header[1] = fgetc(infp)) != EOF) {
         if (header[0] == 'B' && header[1] == 'M') {
            wbmp = 0; /* Not a Wireless Bitmap -- it's a Windows Bitmap */
         }
         else {
            wbmp = 1; /* Assume it's a Wireless Bitmap */
         }
      }
      else
         fatal = 1;
   }
   else
      fatal = 1;

   if (fatal) {
      fprintf(stderr, "Fatal error; end of input file.\n\n");
      exit(1);
   }
   /*
      If this is a Wireless Bitmap (.wbmp) format file,
      skip the header and point to the start of the bitmap itself.
   */
   if (wbmp) {
      for (i=2; i<6; i++)
         header[i] = fgetc(infp);
      /*
         Now read the bitmap.
      */
      for (i=0; i < 32*17; i++) {
         for (j=0; j < 32*18/8; j++) {
            inchar = fgetc(infp);
            bitmap[i][j] = ~inchar;  /* invert bits for proper color */
         }
      }
   }
   /*
      Otherwise, this must be a Windows Bitmap file, because we check
      for that first.  Skip past the header, but save it for possible
      future use.
   */
   else {
      for (i=2; i<0x3e; i++)
         header[i] = fgetc(infp);
      /*
         Now read the bitmap.
      */
      for (i = 32*17-1; i >= 0; i--) {
         for (j=0; j < 32*18/8; j++) {
            inchar = fgetc(infp);
            bitmap[i][j] = ~inchar; /* invert bits for proper color */
         }
      }
   }
   /*
      We've read the entire file.  Now close the input file pointer.
   */
   fclose(infp);
  /*
      We now have the header portion in the header[] array,
      and have the bitmap portion from top-to-bottom in the bitmap[] array.
   */
   /*
      If no Unicode range (U+nnnnnn00 through U+nnnnnnFF) was specified
      with a -p parameter, determine the range from the digits in the
      bitmap itself.

      Store bitmaps for the hex digit patterns that this file uses.
   */
   if (!planeset) {  /* If Unicode range not specified with -p parameter */
      for (i = 0x0; i <= 0xF; i++) {  /* hex digit pattern we're storing */
         for (j = 0; j < 4; j++) {
            hexdigit[i][j]   =
               ((unsigned)bitmap[32 * (i+1) + 4 * j + 8    ][6] << 24 ) |
               ((unsigned)bitmap[32 * (i+1) + 4 * j + 8 + 1][6] << 16 ) |
               ((unsigned)bitmap[32 * (i+1) + 4 * j + 8 + 2][6] <<  8 ) |
               ((unsigned)bitmap[32 * (i+1) + 4 * j + 8 + 3][6]       );
         }
      }
      /*
         Read the Unicode plane digits into arrays for comparison, to
         determine the upper four hex digits of the glyph addresses.
      */
      for (i = 0; i < 4; i++) {
         for (j = 0; j < 4; j++) {
            unidigit[i][j] =
               ((unsigned)bitmap[32 * 0 + 4 * j + 8 + 1][i + 3] << 24 ) |
               ((unsigned)bitmap[32 * 0 + 4 * j + 8 + 2][i + 3] << 16 ) |
               ((unsigned)bitmap[32 * 0 + 4 * j + 8 + 3][i + 3] <<  8 ) |
               ((unsigned)bitmap[32 * 0 + 4 * j + 8 + 4][i + 3]       );
         }
      }
   
      tmpsum = 0;
      for (i = 4; i < 6; i++) {
         for (j = 0; j < 4; j++) {
            unidigit[i][j] =
               ((unsigned)bitmap[32 * 1 + 4 * j + 8    ][i] << 24 ) |
               ((unsigned)bitmap[32 * 1 + 4 * j + 8 + 1][i] << 16 ) |
               ((unsigned)bitmap[32 * 1 + 4 * j + 8 + 2][i] <<  8 ) |
               ((unsigned)bitmap[32 * 1 + 4 * j + 8 + 3][i]       );
            tmpsum |= unidigit[i][j];
         }
      }
      if (tmpsum == 0) {  /* the glyph matrix is transposed */
         flip = 1;  /* note transposed order for processing glyphs in matrix */
         /*
            Get 5th and 6th hex digits by shifting first column header left by
            1.5 columns, thereby shifting the hex digit right after the leading
            "U+nnnn" page number.
         */
         for (i = 0x08; i < 0x18; i++) {
            bitmap[i][7] = (bitmap[i][8] << 4) | ((bitmap[i][ 9] >> 4) & 0xf);
            bitmap[i][8] = (bitmap[i][9] << 4) | ((bitmap[i][10] >> 4) & 0xf);
         }
         for (i = 4; i < 6; i++) {
            for (j = 0; j < 4; j++) {
               unidigit[i][j] =
                  ((unsigned)bitmap[4 * j + 8 + 1][i + 3] << 24 ) |
                  ((unsigned)bitmap[4 * j + 8 + 2][i + 3] << 16 ) |
                  ((unsigned)bitmap[4 * j + 8 + 3][i + 3] <<  8 ) |
                  ((unsigned)bitmap[4 * j + 8 + 4][i + 3]       );
            }
         }
      }
   
      /*
         Now determine the Unicode plane by comparing unidigit[0..5] to
         the hexdigit[0x0..0xF] array.
      */
      uniplane = 0;
      for (i=0; i<6; i++) { /* go through one bitmap digit at a time */
         match = 0; /* haven't found pattern yet */
         for (j = 0x0; !match && j <= 0xF; j++) {
            if (unidigit[i][0] == hexdigit[j][0] &&
                unidigit[i][1] == hexdigit[j][1] &&
                unidigit[i][2] == hexdigit[j][2] &&
                unidigit[i][3] == hexdigit[j][3]) { /* we found the digit */
               uniplane |= j;
               match = 1;
            }
         }
         uniplane <<= 4;
      }
      uniplane >>= 4;
   }
   /*
      Now read each glyph and print it as hex.
   */
   for (i = 0x0; i <= 0xf; i++) {
      for (j = 0x0; j <= 0xf; j++) {
         for (k = 0; k < 16; k++) {
            if (flip) {  /* transpose glyph matrix */
               thischar1[k] = bitmap[32*(j+1) + k + 7][4 * (i+2) + 1];
               thischar2[k] = bitmap[32*(j+1) + k + 7][4 * (i+2) + 2];
            }
            else {
               thischar1[k] = bitmap[32*(i+1) + k + 7][4 * (j+2) + 1];
               thischar2[k] = bitmap[32*(i+1) + k + 7][4 * (j+2) + 2];
            }
         }
         /*
            If the second half of the 16*16 character is all zeroes, this
            character is only 8 bits wide, so print a half-width character.
         */
         empty1 = empty2 = 1;
         for (k=0; (empty1 || empty2) && k < 16; k++) {
            if (thischar1[k] != 0) empty1 = 0;
            if (thischar2[k] != 0) empty2 = 0;
            }
         /*
            Only print this glyph if it isn't blank.
         */
         if (!empty1 || !empty2) {
            /*
               If the second half is empty, this is a half-width character.
               Only print the first half.
            */
            /*
               Original GNU Unifont format is four hexadecimal digit character
               code followed by a colon followed by a hex string.  Add support
               for codes beyond the Basic Multilingual Plane.
            */
            if (uniplane > 0xff)
               fprintf(outfp, "%06X%X%X:", uniplane, i, j);
            else
               fprintf(outfp, "%02X%X%X:", uniplane, i, j);
            for (thisrow=0; thisrow<16; thisrow++) {
               /*
                  If second half is empty and we're not forcing this
                  code point to double width, print as single width
               */
               if (!forcewide &&
                   empty2 && !wide[(uniplane << 8) | (i << 4) | j])
                  fprintf(outfp,
                          "%02X",
                          thischar1[thisrow]);
               else
                  fprintf(outfp,
                          "%02X%02X",
                          thischar1[thisrow], thischar2[thisrow]);
            }
            fprintf(outfp, "\n");
         }
      }
   }
   exit(0);
}
