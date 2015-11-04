/* { dg-do run } */
/* { dg-options "-fuse-caller-save -save-temps" } */
/* { dg-skip-if "" { *-*-* }  { "*" } { "-Os" } } */
/* Testing -fuse-caller-save optimization option.  */

static int __attribute__((noinline))
bar (int x)
{
  return x + 3;
}

int __attribute__((noinline))
foo (int y)
{
  return y + bar (y);
}

int
main (void)
{
  return !(foo (5) == 13);
}

/* Check that there are only 2 stack-saves: r31 in main and foo.  */

/* Variant not mips16.  Check that there only 2 sw/sd.  */
/* { dg-final { scan-assembler-times "(?n)s\[wd\]\t\\\$.*,.*\\(\\\$sp\\)" 2 { target { ! mips16 } } } } */

/* Variant not mips16, Subvariant micromips.  Additionally check there's no
   swm.  */
/* { dg-final { scan-assembler-times "(?n)swm\t\\\$.*,.*\\(\\\$sp\\)" 0 {target micromips } } } */

/* Variant mips16.  The save can save 1 or more registers, check that only 1 is
   saved, twice in total.  */
/* { dg-final { scan-assembler-times "(?n)save\t\[0-9\]*,\\\$\[^,\]*\$" 2 { target mips16 } } } */

/* Check that the first caller-save register is unused.  */
/* { dg-final { scan-assembler-not "(\\\$16)" } } */
