/* { dg-do compile { target arm_dmb } } */
/* { dg-options "-O2 -marm -mfix-cortex-a9-volatile-hazards" } */

volatile int x;

int foo (int c)
{
  int r = 0;
  if (c == 10)
    r = x;
  return r;
}

/* { dg-final { scan-assembler "dmb\[\\t \]*sy" { xfail { arm_thumb2 } } } } */
/* { dg-final { scan-assembler-not "dmbeq" { xfail { arm_thumb2 } } } } */
