/* Verify that we eliminate a static local variable when it is dead on
   return from a function call that recursively calls the function in
   which the variable is defined.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler-not "thestatic" } } */

int test1 (int);
int test2 (int);

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

  y = thestatic;

  return y + x + test1 (x - 1) + test2 (x - 1);
}
  
