/* Verify that we eliminate a static local variable where its uses
   are dominated by a def.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler-not "thestatic" } } */

int
test1 (int x)
{
  static int thestatic;

  thestatic = x;

  return thestatic + x;
}
