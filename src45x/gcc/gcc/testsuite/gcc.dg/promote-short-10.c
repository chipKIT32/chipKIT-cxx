/* Verify that we do not promote a short loop index variable when its
   address is taken.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Found 0 candidates" 1 "promoteshort" } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

extern void outside (short *);

void
test1 (int n, int *x)
{
  short i;

  for (i = 0; i < n; i++)
    {
      outside (&i);
    }
}
