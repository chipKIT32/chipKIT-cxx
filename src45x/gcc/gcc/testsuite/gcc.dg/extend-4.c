/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ee" } */

unsigned char f(unsigned int a, int c)
{
  unsigned int b = a;
  if (c)
    b = a & 0x10ff;
  return b;
}

/* { dg-final { cleanup-rtl-dump "ee" } } */

