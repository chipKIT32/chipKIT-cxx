/* { dg-do compile { target arm_dmb } } */
/* { dg-options "-O2 -mfix-cortex-a9-volatile-hazards" } */

volatile int x;

int foo ()
{
  return x;
}

/* { dg-final { scan-assembler "dmb" } } */
