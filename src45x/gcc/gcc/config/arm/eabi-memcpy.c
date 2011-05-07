/* Simple implementation of memcpy/memmove for targets that don't provide
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

/* Nonzero if either X or Y is not aligned on a "long" boundary.  */
#define UNALIGNED(X, Y) \
  (((long)X & (sizeof (long) - 1)) | ((long)Y & (sizeof (long) - 1)))

/* How many bytes are copied each iteration of the 4X unrolled loop.  */
#define BIGBLOCKSIZE    (sizeof (long) << 2)

/* How many bytes are copied each iteration of the word copy loop.  */
#define LITTLEBLOCKSIZE (sizeof (long))

/* Threshhold for punting to the byte copier.  */
#define TOO_SMALL(LEN)  ((LEN) < BIGBLOCKSIZE)

void
__aeabi_memcpy (void *dst0, const void *src0, unsigned long len)
{
  char *dst = dst0;
  const char *src = src0;
  long *aligned_dst;
  const long *aligned_src;

  /* If the size is small, or either SRC or DST is unaligned,
     then punt into the byte copy loop.  This should be rare.  */
  if (!TOO_SMALL(len) && !UNALIGNED (src, dst))
    {
      aligned_dst = (long *)dst;
      aligned_src = (const long *)src;

      /* Copy 4X long words at a time if possible.  */
      while (len >= BIGBLOCKSIZE)
        {
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          *aligned_dst++ = *aligned_src++;
          len -= BIGBLOCKSIZE;
        }

      /* Copy one long word at a time if possible.  */
      while (len >= LITTLEBLOCKSIZE)
        {
          *aligned_dst++ = *aligned_src++;
          len -= LITTLEBLOCKSIZE;
        }

       /* Pick up any residual with a byte copier.  */
      dst = (char *)aligned_dst;
      src = (const char *)aligned_src;
    }

  while (len--)
    *dst++ = *src++;
}

void
__aeabi_memmove (void *dst_void, const void *src_void, unsigned long len)
{
  char *dst = dst_void;
  const char *src = src_void;

  if (src < dst && dst < src + len)
    {
      /* Destructive overlap...have to copy backwards */
      src += len;
      dst += len;
      while (len--)
	{
	  *--dst = *--src;
	}
    }
  else
    __aeabi_memcpy (dst_void, src_void, len);
}

strong_alias(__aeabi_memcpy, __aeabi_memcpy4)
strong_alias(__aeabi_memcpy, __aeabi_memcpy8)
strong_alias(__aeabi_memmove, __aeabi_memmove4)
strong_alias(__aeabi_memmove, __aeabi_memmove8)
