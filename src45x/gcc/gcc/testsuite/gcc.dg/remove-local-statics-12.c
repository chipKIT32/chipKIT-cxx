/* Verify that we do not eliminate a static variable when it is declared
   in a function that has nested functions.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

int test1 (int x)
{
  static int thestatic;

  int nested_test1 (int x)
  {
    return x + thestatic;
  }

  thestatic = x;

  return thestatic + x + nested_test1 (x);
}
