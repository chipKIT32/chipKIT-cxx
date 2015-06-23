/* Verify that we eliminate a static local variable where it is defined
   along all paths leading to a use.  */

/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler-not "thestatic" } } */

int
test1 (int x)
{
  static int thestatic;

  if (x < 0)
    thestatic = x;
  else
    thestatic = -x;

  return thestatic + x;
}
