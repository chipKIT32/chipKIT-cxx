/* { dg-do compile } */
/* { dg-options "-O2 -mabi=32" } */

int
f1 (unsigned char *p)
{
  unsigned char c = *p;
  unsigned int i = c - 9;
  unsigned char c2 = i;
  return c2 < 20;
}

int
f2 (unsigned short *p)
{
  unsigned short c = *p;
  unsigned int i = c - 9;
  unsigned short c2 = i;
  return c2 < 20;
}

/* { dg-final { scan-assembler-times "\tandi\t" 0} } */
