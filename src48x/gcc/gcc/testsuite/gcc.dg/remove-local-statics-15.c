/* Verify that we do not consider an array variable for local static
   elimination.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse2-details  -fdump-tree-dse1-details" } */

int foo (void)
{
  static int a[1];

  a[0] = 0;

  return a[0];
}

/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse1" } } */
/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse2" } } */
/* { dg-final { cleanup-tree-dump "dse1" } } */
/* { dg-final { cleanup-tree-dump "dse2" } } */
