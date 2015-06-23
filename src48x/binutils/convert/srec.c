/* srec.c -- Motorola S-Record Output

   Copyright (c) 1993-2003, 2008 Free Software Foundation, Inc.
   Contributed by MIPS Technologies, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; If not, write to the Free Software
   Foundation, 51 Franklin Street - Fifth Floor, Boston, MA
   02110-1301, USA.  */

#include "sysdep.h"

#define BUFSIZE		32

static FILE		*srecfp;
static unsigned long	srecaddr = ~0;
static int 		sreclen;
static unsigned char	srecbuf[BUFSIZE];
static int		addrsize;


static void
srecFlush ()
{
    unsigned char cksum = 0;
    int n;

    if (sreclen == 0)
      return;

    fprintf (srecfp, "S%1d%02X", addrsize-1, sreclen + addrsize + 1);
    cksum += sreclen + addrsize + 1;

    for (n = (addrsize - 1) * 8; n >= 0; n -= 8) {
	unsigned char ab = srecaddr >> n;
	fprintf (srecfp, "%02X", ab);
	cksum += ab;
    }

    for (n = 0; n < sreclen; n++) {
	fprintf (srecfp, "%02X", srecbuf[n]);
	cksum += srecbuf[n];
    }

    fprintf (srecfp, "%02X\n", ~cksum & 0xff);
    sreclen = 0;
}


void
Srec3Start (FILE *fp, unsigned long ep)
{
    srecfp = fp;
    addrsize = 4;
}

void
Srec2Start (FILE *fp, unsigned long ep)
{
    srecfp = fp;
    addrsize = 3;
}

void
Srec1Start (FILE *fp, unsigned long ep)
{
    srecfp = fp;
    addrsize = 2;
}


void
SrecOutput (unsigned long addr, unsigned char byte)
{
    if (addr != srecaddr + sreclen || sreclen == BUFSIZE) {
	srecFlush ();
	srecaddr = addr;
    }
    srecbuf[sreclen++] = byte;
}


void
SrecSym (char *name, unsigned long value)
{
    int len = strlen(name) + 8 + 2;
    fprintf (srecfp, "S4%02X%08X%s,00\n", len, value, name);
}


void
SrecEnd (unsigned long ep)
{
    unsigned char cksum = 0;
    int n;

    srecFlush ();

    fprintf (srecfp, "S%1d%02X", 11 - addrsize, addrsize + 1);
    cksum += addrsize + 1;

    for (n = (addrsize - 1) * 8; n >= 0; n -= 8) {
	unsigned char ab = ep >> n;
	fprintf (srecfp, "%02X", ab);
	cksum += ab;
    }

    fprintf (srecfp, "%02X\n", ~cksum & 0xff);
}
