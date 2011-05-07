/* Test the optimization of loading 0.0 for ARM Neon.  */

/* { dg-do compile } */
/* { dg-require-effective-target arm_neon_ok } */
/* { dg-options "-O2" } */
/* { dg-add-options arm_neon } */

double x;
void bar ()
{
  x = 0.0;
}
/* { dg-final { scan-assembler "vmov\.i32\[ 	\]+\[dD\]\[0-9\]+, #0\n" } } */
/* { dg-final { cleanup-saved-temps } } */
