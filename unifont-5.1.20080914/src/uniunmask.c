/*
   uniunmask - program to XOR out selected bits and substitute entire glyphs
               in a unifont.hex file

   Synopsis: uniunmask [-iin_file.hex] [-oout_file.hex]

   This program requires the files "masks.hex" and "substitutes.hex" to
   reside in the current directory.  "masks.hex" is a .hex font file with
   bits set to 1 to be XORed with an input font file.  "substitutes.hex"
   is a .hex font file with replacement glyphs for corresponding code points;
   this is mainly used to replaces space characters having graphic glyphs
   with strings of zeroes.  If a code point exists in both "substitutes.hex"
   and "masks.hex", the entry in "masks.hex" will be ignored.


   Author: Paul Hardy, unifoundry <at> unifoundry.com, 6 January 2008
   
   
    Copyright (C) 2008 Paul Hardy

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

#define MAXBUF 1000




int main(int argc, char *argv[]) {

   unsigned i;                /* loop variable                     */
   unsigned k0, k1;           /* temp Unicode char variables       */
   char inbuf[MAXBUF];        /* input buffer                      */
   unsigned thischar;         /* the current character             */
   unsigned char thischarbyte; /* unsigned char lowest byte of Unicode char */
   unsigned char charbits[32][4];  /* bitmap for one character, 4 bytes/row */
   unsigned charint[16];           /* bitmap as one row per entry */
   unsigned char maskbits[32][4];  /* mask for charbits[][] */
   unsigned maskint[16];           /* mask for charint[] */
   char *infile="", *outfile="";  /* names of input and output files   */
   FILE *infp, *outfp;      /* file pointers of input and output files */
   FILE *maskfp;            /* pointer to "masks.hex" file             */
   FILE *subfp;             /* pointer to "substitutes.hex" file       */
   char maskbuf[MAXBUF];    /* to hold next masked character           */
   char subbuf[MAXBUF];     /* to hold next substitute character       */
   int maskeof=0;           /* to signal no more masks to read         */
   int subeof=0;            /* to signal no more substitutes to read   */
   unsigned maskchar;       /* current mask character in buffer        */
   unsigned subchar;        /* current substitute character in buffer  */
   void hex2bit();          /* convert hex string to bitmap            */

   if ((maskfp = fopen("masks.hex", "r")) == NULL) {
      fprintf(stderr,"\nError: Can't open \"masks.hex\" for input.\n\n");
      exit(1);
   }

   if ((subfp = fopen("substitutes.hex", "r")) == NULL) {
      fprintf(stderr,"\nError: Can't open \"substitutes.hex\" for input.\n\n");
      exit(1);
   }

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
               default:   /* if unrecognized option, print list and exit */
                  fprintf(stderr, "\nSyntax:\n\n");
                  fprintf(stderr, "   %s -p<Unicode_Page> ", argv[0]);
                  fprintf(stderr, "-i<Input_File> -o<Output_File> -w\n\n");
                  fprintf(stderr, "\nExample:\n\n");
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
      Get first mask code point.
   */
   if (fgets(maskbuf, MAXBUF-1, maskfp) == NULL) {
      maskeof = 1;
   }
   else {
      sscanf(maskbuf, "%x", &maskchar); /* get next mask code point */
   }
   /*
      Get first substitute code point.
   */
   if (fgets(subbuf, MAXBUF-1, subfp) == NULL) {
      subeof = 1;
   }
   else {
      sscanf(subbuf, "%x", &subchar); /* get next substitute code point */
   }
   /*
      Read in the glyphs in the file
   */
   while (fgets(inbuf, MAXBUF-1, infp) != NULL) {
      sscanf(inbuf, "%x", &thischar);
      thischarbyte = (unsigned char)(thischar & 0xff);
      /*
         Advance to the right position in the masks.hex file.
         It's okay if we go to EOF.
      */
      while (!maskeof && thischar > maskchar) {
         if (fgets(maskbuf, MAXBUF-1, maskfp) == NULL) {
            maskeof = 1;
         }
         else {
            sscanf(maskbuf, "%x", &maskchar); /* get next mask code point */
         }
      }
      /*
         Advance to the right position in the substitute.hex file.
         It's okay if we go to EOF.

         Note that ALL entries in substitute.hex are printed, unlike
         all entries in masks.hex.  This guarantees that we handle all
         space characters properly for a font regardless of whether or
         not it is in the input .hex file.

         If substitute code point has no hex string, delete the input
         character from the output.
      */
      while (!subeof && thischar > subchar) {
         if (fgets(subbuf, MAXBUF-1, subfp) == NULL) {
            subeof = 1;
         }
         else {
            sscanf(subbuf, "%x", &subchar); /* get next substitute code point */
            if (subchar < thischar) {
               fprintf(outfp, "%s", subbuf); /* print every sub code point */
            }
         }
      }
      if (thischar == subchar) {
         if (strlen(subbuf) >= 37) {
            fprintf(outfp, "%s", subbuf); /* print substitute character */
         }
         /* otherwise, do nothing -- delete input code point from output */
      }
      else if (maskeof || thischar < maskchar) {
         fprintf(outfp, "%s", inbuf); /* print original character */
      }
      /*
         If the character should have a combining circle, look for it
         and generate a new hex string without it.
      */
      else {
         for (k0=0; inbuf[k0] != ':'; k0++);
         k0++;
         hex2bit(&inbuf[k0], charbits); /* convert hex string to 32*4 bitmap */
         for (k1=0; maskbuf[k1] != ':'; k1++);
         k1++;
         hex2bit(&maskbuf[k1], maskbits);
         for (i=0; i<16; i++) {
            /* combine two bytes of a glyph row into one int */
            charint[i] = (charbits[i+8][1] << 8) | charbits[i+8][2];
            /* combine two bytes of a mask row into one int */
            maskint[i] = (maskbits[i+8][1] << 8) | maskbits[i+8][2];
            charint[i] ^= maskint[i]; /* turn off bits that were 1 in mask */
         }
         /*
            Now print the glyph hex string with the combining circle removed.
         */
         fprintf(outfp, "%04X:", thischar);
         if (strlen(inbuf) > 63) { /* full-width character */
            for (i=0; i<16; i++) {
               thischarbyte = (charint[i] >> 8) & 0xff;
               fprintf(outfp, "%02X", thischarbyte);
               thischarbyte = charint[i] & 0xff;
               fprintf(outfp, "%02X", thischarbyte);
            }
            fprintf(outfp, "\n");
         }
         else { /* half-width character */
            for (i=0; i<16; i++) {
               thischarbyte = (charint[i] >> 8) & 0xff;
               fprintf(outfp, "%02X", thischarbyte);
            }
            fprintf(outfp, "\n");
         }
      }
   }
   exit(0);
}

/*
   Convert the portion of a hex string after the ':' into a character bitmap.

   If string is >= 128 characters, it will fill all 4 bytes per row.
   If string is >= 64 characters and < 128, it will fill 2 bytes per row.
   Otherwise, it will fill 1 byte per row.
*/
void hex2bit(char *instring, unsigned char character[32][4]) {

   int i;  /* current row in bitmap character */
   int j;  /* current character in input string */
   int k;  /* current byte in bitmap character */
   int width;  /* number of output bytes to fill: 1, 2, or 4 */

   for (i=0; i<32; i++)  /* erase previous character */
      character[i][0] = character[i][1] = character[i][2] = character[i][3] = 0; 
   j=0;  /* current location is at beginning of instring */

   // width = (strlen(instring) - 1) >> 4; /* 16 hex digits per 8 bytes */

   if (strlen(instring) <= 34)  /* 32 + possible '\r', '\n' */
      width = 0;
   else if (strlen(instring) <= 66)  /* 64 + possible '\r', '\n' */
      width = 1;
   else
      width = 4;

   if (width > 1) width = 1;  /* width > 1 not fully implemented yet */

   k = (width > 1) ? 0 : 1;  /* if width < 3, start at index 1 else at 0 */

   for (i=8; i<24; i++) {  /* 16 rows per input character, rows 8..23 */
      sscanf(&instring[j], "%2hhx", &character[i][k]);
      j += 2;
      if (width > 0) { /* add next pair of hex digits to this row */
         sscanf(&instring[j], "%2hhx", &character[i][k+1]);
         j += 2;
      }
      if (width > 1) { /* add 2 next pairs of hex digits to this row */
         sscanf(&instring[j], "%2hhx", &character[i][k+2]);
         j += 2;
         sscanf(&instring[j], "%2hhx", &character[i][k+3]);
         j += 2;
      }
   }

   return;
}

/*
   init() -- read the file "combining.dat" and populate the combining[]
             array with a list of code points that contain combining circles.
*/
int init(unsigned char combining[17*65536]) {

   int i, j;
   char inbuf[MAXBUF];
   FILE *combiningfp;

   if ((combiningfp = fopen("combining.dat", "r")) == NULL) {
      fprintf(stderr, "\nError: data file \"combining.dat\" not found.\n\n");
      exit(0);
   }

   while (fgets(inbuf, MAXBUF-1, combiningfp) != NULL) {
      if ((inbuf[0] >= '0' && inbuf[0] <= '9') ||
          (inbuf[0] >= 'A' && inbuf[0] <= 'F') ||
          (inbuf[0] >= 'a' && inbuf[0] <= 'f')) {
         j = 0;  /* in case second hex number isn't there */
         sscanf(inbuf, "%x-%x", &i, &j);
         do {
            combining[i] = 1;
            i++;
         } while (i <= j);
      }
   }
   fclose(combiningfp);

   return(0);
}
