/* Verify that we do promote a short loop index variable when the loop
   in which it is used has a bound of wider type and
   -funsafe-loop-optimizations is in effect.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -funsafe-loop-optimizations -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Promoting 1 variables" 1 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

void
test1 (int n, int *x)
{
  short i;

  for (i = 0; i < n; i++)
    {
      x[i] = 0;
    }
}
