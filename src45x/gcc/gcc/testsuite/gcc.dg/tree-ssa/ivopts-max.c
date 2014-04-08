/* { dg-do compile { target { lp64 } } } */
/* { dg-options "-O2 -fdump-tree-ivopts -fno-tree-vectorize -fno-tree-loop-ivcanon" } */

/* # (exit test == false)  : 0xffffffff */
/* iv_period (j)           : 0xffffffff */
/* may_eliminate (j, i < 0x200000000) should return true */

void
do_while_maxuint_plus_1 (unsigned char *p)
{
  unsigned long int i = 0;
  unsigned int j = 0;
  do
    {
      p[j] = '\0';
      j++;
      i += 2;
    }
  while (i < 0x200000000);
}

/* { dg-final { scan-tree-dump-times "PHI" 1 "ivopts"} } */
/* { dg-final { scan-tree-dump-times "PHI.*, j_" 1 "ivopts"} } */
/* { dg-final { scan-tree-dump-times "j_.* != 0" 1 "ivopts"} } */
/* { dg-final { scan-tree-dump-not "i_" "ivopts"} } */
/* { dg-final { cleanup-tree-dump "ivopts" } } */
