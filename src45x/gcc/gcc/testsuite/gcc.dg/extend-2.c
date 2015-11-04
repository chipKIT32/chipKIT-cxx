/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ee" } */
/* { dg-require-effective-target ilp32 } */

void f(unsigned char * p, short *s, int c)
{
  short or = 0;
  while (c)
    {
      or = or | s[c];
      c --;
    }
  *p = (unsigned char)or;
}

/* { dg-final { scan-rtl-dump-times "zero_extend" 0 "ee" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump-times "sign_extend" 0 "ee" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump-times "superfluous extension \[0-9\]+ replaced" 2 "ee" { target mips*-*-* } } } */
/* { dg-final { cleanup-rtl-dump "ee" } } */

