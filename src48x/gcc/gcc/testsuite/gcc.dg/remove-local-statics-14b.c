/* Verify that we do not eliminate a static local variable if the function
   containing it is inlined.  */

/* { dg-do compile } */
/* { dg-options "-O2" } */
/* { dg-final { scan-assembler "thestatic" } } */

int
test2 (int x)
{
  if (x < 0)
    return 0;
  else
    return test1 (x - 1);
}

inline int
test1 (int x)
{
  static int thestatic;
  int y;

  thestatic = x;

  y = test2 (thestatic - 1);

  return y + x;
}
  
