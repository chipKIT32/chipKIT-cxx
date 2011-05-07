/* Verify that we promote a short loop index variable.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Promoting 1 variables" 1 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

void
test1 (short n, int *x)
{
  short i;

  for (i = 0; i < n; i++)
    x[i] = 0;
}
