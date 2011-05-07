/* Verify that we do promote a short loop index variable when it has
   a non-unit-increment and -funsafe-loop-optimizations is in effect.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -funsafe-loop-optimizations -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Promoting 1 variables" 1 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

void
test1 (short n, int *x)
{
  short i;

  for (i = 0; i < n; i+=2)
    {
      x[i] = 0;
    }
}
