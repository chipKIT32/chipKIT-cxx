/* Verify that we do not eliminate a static local variable when its uses
   are not dominated by a def.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "first_time" } } */

int
test1 (int x)
{
  static int first_time;

  if (x == 1)
    first_time = 1;
  else if (x > 0)
    first_time = 2;

  return first_time + x;
}
