/* Verify that we do not consider an array variable for local static
   elimination.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics -fdump-tree-remlocstatic-details" } */

int foo (void)
{
  static int a[1];

  a[0] = 0;

  return a[0];
}

/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "remlocstatic" } } */
/* { dg-final { cleanup-tree-dump "remlocstatic" } } */
