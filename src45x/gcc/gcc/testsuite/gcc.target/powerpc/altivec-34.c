/* altivec-34.c */
/* { dg-do run { target powerpc*-*-* } } */
/* { dg-options "-O0 -maltivec" } */

#include <stdlib.h>

/* #4053 (Align large local Altivec arrays at 128 bits)  */

long l1;
int main(void)
{
  long a1[5], a2[6], a3[4];
  char c1[4], c2[17];
  if (((long)(&a1[0])) & 0xf)
    abort ();
  if (((long)(&a2[0])) & 0xf)
    abort ();
  if (((long)(&a3[0])) & 0xf)
    abort ();
  if (((long)(&c1[0])) & 0xf)
    abort ();
  if (((long)(&c2[0])) & 0xf)
    abort ();
  exit (0);
}


