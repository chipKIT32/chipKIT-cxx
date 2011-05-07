/* Verify that we eliminate a static local variable when it is live
   on return from a function call that does not recursively call the
   function in which the variable is defined.  */

/* XFAIL'd because we don't utilize the callgraph properly.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler-not "thestatic" { xfail *-*-* } } } */

static int
test2 (int x)
{
  if (x < 0)
    return 0;
  else
    return x + test2 (x - 1);
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
  
