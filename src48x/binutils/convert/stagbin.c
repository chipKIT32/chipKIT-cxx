/* stag.c -- Stag Programmer Binary Output

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

#define SOH 		1
#define CHECKLENGTH	1024	/* # bytes between intermediate checksums */
#define BUFSIZE		8192	/* max bytes per block */

static FILE		*stagfp;
static unsigned long	stagaddr = ~0;
static int 		staglen;
static unsigned char	*stagbuf;
static int		chksum;

static void
sbinputc (unsigned char c)
{
    chksum += c;
    putc (c, stagfp);
}

static void
sbinlong (unsigned i)
{
    sbinputc (i >> 24);
    sbinputc (i >> 16);
    sbinputc (i >> 8);
    sbinputc (i);
}

static void
sbinheader (unsigned len, unsigned offs)
{
    sbinputc (SOH);
    chksum = 0;
    sbinlong (len);
    sbinlong (offs);
}

static void
stagFlush ()
{
    int i;

    if (staglen > 0) {
	sbinheader (staglen, stagaddr);
	for (i = 0; i < staglen; i++) {
	    if (i > 0 && i % CHECKLENGTH == 0)
	      fputc (-chksum, stagfp);	/* intermediate checksum */
	    putc (stagbuf[i], stagfp);
	    chksum += stagbuf[i];
	}
	fputc (-chksum, stagfp);	/* final checksum */
	staglen = 0;
    }
}

void
StagStart (FILE *fp, unsigned long ep)
{
    stagbuf = (char *) xmalloc (BUFSIZE);
    stagfp = fp;
}


void
StagOutput (unsigned long addr, unsigned char byte)
{
    if (addr != stagaddr + staglen || staglen == BUFSIZE) {
	stagFlush ();
	stagaddr = addr;
    }
    stagbuf[staglen++] = byte;
}


void
StagEnd (unsigned long ep)
{
    stagFlush ();

    /* terminating null block */
    sbinheader (0, 0);		/* header */
    fputc (0, stagfp);		/* checksum */
    free (stagbuf);
}

