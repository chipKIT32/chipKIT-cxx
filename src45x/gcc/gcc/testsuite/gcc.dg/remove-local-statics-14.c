/* Verify that we do eliminate a static local variable whose last use is
   in a statement containing a call expression.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler-not "thestatic" } } */

int
test2 (int x)
{
  if (x < 0)
    return 0;
  else
    return test1 (x - 1);
}

int
test1 (int x)
{
  static int thestatic;
  int y;

  thestatic = x;

  y = test2 (thestatic - 1);

  return y + x;
}
  
