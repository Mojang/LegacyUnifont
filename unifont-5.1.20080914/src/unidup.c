/*
   unidup - check for duplicate code points in sorted unifont.hex file.

   Synopsis: unidup < unifont_file.hex

             [Hopefully there won't be any output!]

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

#define MAXBUF 256


int main () {

   int ix, iy;
   char inbuf[MAXBUF];

   ix = -1;

   while (fgets(inbuf, MAXBUF-1, stdin) != NULL) {
      sscanf(inbuf, "%X", &iy);
      if (ix == iy) fprintf(stderr, "Duplicate code point: %04X\n", ix);
      else ix = iy;
   }
   exit(0);
}
