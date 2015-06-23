/* { dg-do compile { target arm_dmb } } */
/* { dg-options "-O2 -mfix-cortex-a9-volatile-hazards" } */

volatile float x;
volatile double y;

void foo (void)
{
  ++x;
  ++y;
}

/* { dg-final { scan-assembler-times "dmb" 2 } } */
