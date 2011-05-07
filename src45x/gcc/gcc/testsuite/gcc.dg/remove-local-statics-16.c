/* Verify that we do not consider an structure variable for local static
   elimination.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics -fdump-tree-remlocstatic-details" } */

int foo (void)
{
  static struct {
    int x;
    int y;
  } a;

  a.x = 0;

  return a.y;
}

/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "remlocstatic" } } */
/* { dg-final { cleanup-tree-dump "remlocstatic" } } */
