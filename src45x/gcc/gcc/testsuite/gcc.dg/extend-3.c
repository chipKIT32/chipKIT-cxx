/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-ee" } */

unsigned int f(unsigned char byte)
{
  return byte << 25;
}

/* { dg-final { scan-rtl-dump-times "zero_extend:" 0 "ee" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump "superfluous extension \[0-9\]+ replaced" "ee" { target mips64 } } } */
/* { dg-final { cleanup-rtl-dump "ee" } } */

