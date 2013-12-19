/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ee" } */

void f(unsigned char * p, short s, int c, int *z)
{
  if (c)
    *z = 0;
  *p ^= (unsigned char)s;
}

/* { dg-final { scan-rtl-dump-times "sign_extend:" 0 "ee" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump-times "superfluous extension \[0-9\]+ replaced" 1 "ee" { target mips*-*-* } } } */
/* { dg-final { cleanup-rtl-dump "ee" } } */
