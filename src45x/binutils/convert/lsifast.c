/* lsifast.c -- LSI PMON Fast ASCII Output

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

/*#define MAXREC			550*/
#define MAXREC			255
#define BUFSIZE			((MAXREC-12)/4*3)

#define ZEROS "/Z"		/* record contains a count of zeros,
				   actually it's the number of 24-bit
				   records that are all zero */
#define BYTE "/B"		/* record contains a single byte */
#define CHKSUM "/C"		/* checksum to date */
#define CLRSUM "/K"		/* klear (sic) the checksum */
#define ADDR "/A"		/* double length record containing a 32 bit
				   address */
#define END "/E"		/* end of download */

static FILE		*lsifp;
static unsigned long	lsiaddr = ~0;
static int 		lsilen;
static unsigned char	*lsibuf;
static int		chksum;


static int
b2a(int c)
{
/* translate binary to base64 (tx form)
0..25		A-Z
26..51		a-z
52..61		0-9
62		,
63		.
*/

    c &= 0x3f;
    if (c <= 25)  return('A'+c);
    if (c <= 51) return('a'+c-26);
    if (c <= 61) return('0'+c-52);
    if (c == 62) return(',');
    if (c == 63) return('.');
    abort();
}

static void
send12 (char *type, unsigned val)
{
    val &= 0xfff;
    chksum += val;
    fputc (type[0], lsifp);
    fputc (type[1], lsifp);
    fputc (b2a(val>>6), lsifp);
    fputc (b2a(val), lsifp);
}


static void
send24(unsigned long val)
{
    int i;
    val &= 0xffffff;
    chksum += val>>12;
    chksum += val&0xfff;
    for (i = 18; i >= 0; i -= 6) {
	char c = b2a(val >> i);
	putc (c, lsifp);
    }
}


static void
txaddr (unsigned long addr)
{
    send12(ADDR, addr >> 24);
    send24(addr);
}

static void
lsiChecksum ()
{
    fputc ('\n', lsifp);
    send12(CHKSUM, chksum);
    fputc ('\n', lsifp);
    chksum = 0;
}

static void
lsiFlush ()
{
    unsigned char *buf = lsibuf;
    int zcnt = 0;

    if (lsilen == 0)
      return;

    while (lsilen >= 3) {
	unsigned long bdat;

	bdat = (buf[0]<<16)+(buf[1]<<8)+buf[2]; /* convert to single word */
	buf += 3; lsilen -= 3;

	if (bdat == 0) {
	    if (++zcnt < 4096)
	      continue;
	    zcnt--;
	}

	if (zcnt) {
	    send12 (ZEROS, zcnt);
	    zcnt = 0;
	}
	send24(bdat); /* send the data */
    }

    if (zcnt)
      send12 (ZEROS, zcnt);

    while (lsilen != 0) {
	send12 (BYTE, *buf++);
	lsilen--;
    }

    fputc ('\n', lsifp);
}

void
LsiStart (FILE *fp, unsigned long ep)
{
    lsibuf = (char *) xmalloc (BUFSIZE);
    lsifp = fp;
    send12(CLRSUM, 0);
    chksum = 0;
}


void
LsiOutput (unsigned long addr, unsigned char byte)
{
    int newaddr = (addr != lsiaddr + lsilen);
    if (newaddr || lsilen >= BUFSIZE) {
	lsiFlush ();
	if (newaddr)
	  txaddr (addr);
	lsiaddr = addr;
    }
    lsibuf[lsilen++] = byte;
}


void
LsiSym (char *name, unsigned long value)
{
    int len = strlen(name);
    char *buf = alloca (len + 10);
    static int reclen = 0;
    static int first = 1;

    if (first) {
	lsiFlush ();
	lsiChecksum ();
	first = 0;
    }

    if (reclen + 8 >= MAXREC) {
	fputc ('\n', lsifp);
	reclen = 0;
    }
    txaddr (value);
    reclen += 8;

    len += 2;			/* /S */
    strcpy(buf, name);
    strcat(buf, ","); len++;

    /* pad total string length to multiple of 4 */
    while (len & 3) {
	strcat (buf, "X");
	len++;
    }

    if (reclen + len >= MAXREC) {
	fputc ('\n', lsifp);
	reclen = 0;
    }
    fprintf(lsifp, "/S%s", buf);
    reclen += len;
}


void
LsiEnd (unsigned long ep)
{
    lsiFlush ();
    lsiChecksum ();
    txaddr(ep);
    send12(END, 0);
    fputc ('\n', lsifp);
    free (lsibuf);
}

