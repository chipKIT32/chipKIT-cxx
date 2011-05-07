/* Verify that we do not eliminate a static variable that is "defined"
   by an asm that clobbers memory.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics -fdump-tree-remlocstatic-details" } */

int foo (void)
{
  static int foo = 0;

  __asm__ __volatile__ ("" : : : "memory");

  foo++;

  return foo;
}

/* { dg-final { scan-tree-dump-times "removed 0 static variables" 1 "remlocstatic" } } */
/* { dg-final { cleanup-tree-dump "remlocstatic" } } */
