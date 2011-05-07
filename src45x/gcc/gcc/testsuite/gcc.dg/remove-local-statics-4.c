/* Verify that we don't eliminate a global static variable.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "global_static" } } */

static int global_static;

int
test1 (int x)
{
  global_static = x;

  return global_static + x;
}
