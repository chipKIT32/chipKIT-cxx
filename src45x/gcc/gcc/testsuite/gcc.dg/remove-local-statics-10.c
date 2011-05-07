/* Verify that we do not eliminate a static local variable when it is
   live on return from a function call that recursively calls the
   function in which the variable is defined.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

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

  y = test2 (x - 1);

  y += thestatic;

  return y + x;
}
  
