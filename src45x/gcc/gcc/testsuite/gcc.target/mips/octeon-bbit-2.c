/* { dg-do compile } */
/* { dg-options "-O2 -march=octeon -mbranch-likely" } */
/* { dg-final { scan-assembler "\tbbit\[01\]\t" } } */
/* { dg-final { scan-assembler-not "\tbbit\[01\]l\t" } } */
/* { dg-final { scan-assembler "\tbnel\t" } } */
/* { dg-final { scan-assembler-not "\tbne\t" } } */

NOMIPS16 long int
f (long int n, long int i)
{
  long int s = 0;
  for (; i & 1; i++)
    s += i;
  return s;
}

NOMIPS16 long int
g (long int n, long int i)
{
  long int s = 0;
  for (i = 0; i < n; i++)
    s += i;
  return s;
}
