/* Verify that we do not consider an structure variable for local static
   elimination.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse2-details  -fdump-tree-dse1-details" } */

int foo (void)
{
  static struct {
    int x;
    int y;
  } a;

  a.x = 0;

  return a.y;
}

/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse1" } } */
/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse2" } } */
/* { dg-final { cleanup-tree-dump "dse1" } } */
/* { dg-final { cleanup-tree-dump "dse2" } } */
