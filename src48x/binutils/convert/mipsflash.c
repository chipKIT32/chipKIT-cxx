/* mipsflash.c -- MIPS eval board USB/parallel flash download format

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

static FILE		*mipsfp;
static unsigned long	mipsaddr = ~0;
static unsigned long	mipsbase = ~0;
static int 		mipslen;
static unsigned char	*mipsbuf;
static int		mipsbigendian;

extern int		promflag;
extern unsigned long	prombase;

/* data chunk size - 16 words */
#define BUFSIZE			(16*4)

/* flash sector size */
#define FLASH_BLOCK_SIZE	0x20000

static void
mipsFlush ()
{
    unsigned char *buf;
    int done;

    if (mipslen == 0)
      return;

    for (buf = mipsbuf, done = 0; done < BUFSIZE; buf += 4, done += 4) {
	unsigned long val = 0;
	int i;

	/* new line every 32 bytes (8 words) */
	if (done != 0 && done % 32 == 0)
	    fputc ('\n', mipsfp);

	/* collect one word from input buffer using appropriate endianness. */
	for (i = 0; i < 4; i++) {
	    unsigned int addr01 = (mipsaddr + done + i) & 0x3;
	    if (mipsbigendian)
		val |= buf[i] << (8 * (3 - addr01));
	    else
		val |= buf[i] << (8 * addr01);
	}

	/* output one word of data */
	fprintf (mipsfp, " %08lx", val);
    }

    fputc ('\n', mipsfp);

    /* reset output buffer to blank */
    memset (mipsbuf, 0xff, BUFSIZE);
    mipslen = 0;
}


void
MipsStart (FILE *fp, unsigned long ep, int bigendian)
{
    mipsbuf = (char *) xmalloc (BUFSIZE);
    mipsfp = fp;
    mipsbigendian = bigendian;

    /* Reset the loader state machine */
    fprintf (fp, "!R\n");
}


void
MipsOutput (unsigned long addr, unsigned char byte)
{
  /* XXX Rework this whole thing to gather up aligned chunks into each
     buffer, don't require strictly contiguous addresses.  */

    unsigned long lastaddr = mipsaddr + mipslen;
    int newsector, newaddr;

    if (promflag)
	/* convert back to PROM physical address */
	addr += prombase;

    newsector = ((addr ^ lastaddr) & ~(FLASH_BLOCK_SIZE - 1)) != 0;
    newaddr = newsector || (addr != lastaddr);

    if (newaddr || newsector || mipslen >= BUFSIZE) {
	mipsFlush ();
	mipsaddr = addr;
    }

    if (mipsbase == ~0) {
      /* first byte - remember base address */
      mipsbase = addr;
      if (mipsbase == 0x1fc00000)
	/* unlock the flash */
	fprintf (mipsfp, ">1fc00xxx @1fc00000 !C\n");
    }

    if (newsector)
      {
	/* started new flash sector - erase it */
	unsigned long base = addr & ~(FLASH_BLOCK_SIZE - 1);
	fprintf (mipsfp, ">%.5xxxx ", base / 0x1000);
	fprintf (mipsfp, "@%.8x !E\n", base);
	if (base != addr)
	  newaddr = 1;
      }

    if (newaddr)
      /* set new write addr */
      fprintf (mipsfp, "@%.8lx\n", addr);

    if (newaddr || newsector || ((addr & 0xfff == 0) && mipslen == 0))
      /* update display */
      fprintf (mipsfp, ">%.8lx\n", addr);

    mipsbuf[mipslen++] = byte;
}

void
MipsEnd (unsigned long ep)
{
    mipsFlush ();

    if (mipsbase == 0x1fc00000) {
	/* Lock the flash */
	fprintf (mipsfp, ">LOCKFLSH\n");
	fprintf (mipsfp, "@1fc00000 !S\n");
	fprintf (mipsfp, "@1fc20000 !S\n");
	fprintf (mipsfp, "@1fc40000 !S\n");
	fprintf (mipsfp, "@1fc60000 !S\n");
	fprintf (mipsfp, "@1fc80000 !S\n");
	fprintf (mipsfp, "@1fca0000 !S\n");
	fprintf (mipsfp, "@1fcc0000 !S\n");
	fprintf (mipsfp, "@1fce0000 !S\n");
    }

    fprintf (mipsfp, ">#DL_DONE\n");
    fprintf (mipsfp, ">FINISHED\n");
    free (mipsbuf);
}

