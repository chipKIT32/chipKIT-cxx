/* Verify that we do not eliminate a static local variable when its
   address is taken.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

int *
test1 (int x)
{
  static int thestatic;

  thestatic = x;

  return &thestatic + x;
}
