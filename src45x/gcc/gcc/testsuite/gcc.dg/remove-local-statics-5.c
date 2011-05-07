/* Verify that we do not eliminate a static local variable whose uses
   are dominated by a def when the function calls setjmp.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

#include <setjmp.h>

int
foo (int x)
{
  static int thestatic;
  int retval;
  jmp_buf env;

  thestatic = x;

  retval = thestatic + x;

  setjmp (env);

  return retval;
}
