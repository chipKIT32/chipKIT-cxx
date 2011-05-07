/* Simple implementation of memset/memclr for targets that don't provide
   their own.

   Copyright (c) 2010  free software foundation, inc.
   contributed by CodeSourcery.

   this file is free software; you can redistribute it and/or modify it
   under the terms of the gnu general public license as published by the
   free software foundation; either version 3, or (at your option) any
   later version.

   this file is distributed in the hope that it will be useful, but
   without any warranty; without even the implied warranty of
   merchantability or fitness for a particular purpose.  see the gnu
   general public license for more details.

   under section 7 of gpl version 3, you are granted additional
   permissions described in the gcc runtime library exception, version
   3.1, as published by the free software foundation.

   you should have received a copy of the gnu general public license and
   a copy of the gcc runtime library exception along with this program;
   see the files copying3 and copying.runtime respectively.  if not, see
   <http://www.gnu.org/licenses/>.  */


/* Define ALIASNAME as a strong alias for NAME.  */
#define strong_alias(name, aliasname) \
  extern __typeof (name) aliasname __attribute__ ((alias (#name)));

#define LBLOCKSIZE (sizeof(long))
#define UNALIGNED(X)   ((long)X & (LBLOCKSIZE - 1))
#define TOO_SMALL(LEN) ((LEN) < LBLOCKSIZE)

void
__aeabi_memset (void *m, unsigned long n, int c)
{
  char *s = (char *)m;
  int i;
  unsigned long buffer;
  unsigned long *aligned_addr;
  unsigned int d = c & 0xff;	/* To avoid sign extension, copy C to an
				   unsigned variable.  */

  while (UNALIGNED (s))
    {
      if (n--)
        *s++ = (char) c;
      else
        return;
    }

  if (!TOO_SMALL (n))
    {
      /* If we get this far, we know that n is large and s is word-aligned. */
      aligned_addr = (unsigned long *)s;

      /* Store D into each char sized location in BUFFER so that
         we can set large blocks quickly.  */
      buffer = (d << 8) | d;
      buffer |= (buffer << 16);
      for (i = 32; i < LBLOCKSIZE * 8; i <<= 1)
        buffer = (buffer << i) | buffer;

      /* Unroll the loop.  */
      while (n >= LBLOCKSIZE * 4)
        {
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          *aligned_addr++ = buffer;
          n -= 4 * LBLOCKSIZE;
        }

      while (n >= LBLOCKSIZE)
        {
          *aligned_addr++ = buffer;
          n -= LBLOCKSIZE;
        }
      /* Pick up the remainder with a bytewise loop.  */
      s = (char *)aligned_addr;
    }

  while (n--)
    *s++ = (char) c;
}

void
__aeabi_memclr (void *m, unsigned long n)
{
  __aeabi_memset (m, n, 0);
}

strong_alias(__aeabi_memset, __aeabi_memset4)
strong_alias(__aeabi_memset, __aeabi_memset8)
strong_alias(__aeabi_memclr, __aeabi_memclr4)
strong_alias(__aeabi_memclr, __aeabi_memclr8)
