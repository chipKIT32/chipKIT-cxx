/* Verify that we do not promote a short loop index variable when it is
   being passed as a function parameter.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fpromote-loop-indices -fdump-tree-promoteshort" } */
/* { dg-final { scan-tree-dump-times "Promoting 0 variables" 1 "promoteshort" { xfail m68k*-*-* fido*-*-* i?86-*-* x86_64-*-* mips*-*-* sh*-*-* } } } */
/* { dg-final { cleanup-tree-dump "promoteshort" } } */

extern void outside (short);

void
test1 (short n)
{
  short i;

  for (i = 0; i < n; i++)
    outside (i);
}
