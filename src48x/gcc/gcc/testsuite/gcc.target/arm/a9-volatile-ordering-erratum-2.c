/* { dg-do compile { target arm_dmb } } */
/* { dg-options "-O2 -mthumb -mfix-cortex-a9-volatile-hazards" } */
/* { dg-require-effective-target arm_thumb2_ok } */

volatile int x;
volatile int y;

int foo (int c)
{
  return x < y + c ? x : y;
}

/* { dg-final { scan-assembler "dmbgt" } } */
/* { dg-final { scan-assembler "dmble" } } */
