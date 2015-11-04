/* { dg-do run } */
/* Skip -flto torture options, which leave messages in stdout. */
/* { dg-skip-if "code quality test" { *-*-* } { "-O0" "-flto" } { "" } } */
/* { dg-options "-mframe-header-opt -fdump-rtl-mach -save-temps" } */
/* Testing -mframe-header-opt optimization option.  */

int __attribute__((noinline))
B (int x)
{
  return x + 3;
}

/* We are sure that B is not using its incoming stack frame so we can skip
   its allocation.  */
int __attribute__((noinline))
A (int x)
{
  return B (x) + 2;
}

int
main (void)
{
  int a;
  void *volatile sp1, *volatile sp2;
  register void *sp asm ("$sp");
  sp1 = sp;
  a = A (5);
  sp2 = sp;
  return !(a == 10 && sp1 == sp2);
}

/* -mframe-header-opt is a o32-only optimization, check if it's disabled for other abis.  */
/* { dg-final { scan-rtl-dump-not "Frame size reduced by frame header optimization" "mach" { target { ! o32 } } } } */

/* -mframe-header-opt is disabled for mips16 and micromips, to be fixed.  */
/* This should be replaced with the { target selector1 xfail selector2 } idiom in 4.8.  */
/* { dg-final { if [ check_effective_target_o32 ] { scan-rtl-dump "Frame size reduced by frame header optimization" "mach" { xfail { mips16 || micromips } } } } } */

/* For enabled targets, test that only one stack allocation is present, the one
   in main.  The one in A should have been removed by -mframe-header-opt.  */
/* { dg-final { scan-assembler-times "addiu\t\\\$sp,\\\$sp,-" 1 { target { o32 && { ! { mips16 || micromips } } } } } } */

/* { dg-final { cleanup-rtl-dump "mach" } } */
/* { dg-final { cleanup-saved-temps } } */
