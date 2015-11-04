/* Verify that we do not eliminate a static variable that is "defined"
   by an asm that clobbers memory.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fdump-tree-dse2-details  -fdump-tree-dse1-details" } */

int foo (void)
{
  static int thestatic = 0;

  __asm__ __volatile__ ("" : : : "memory");

  thestatic++;

  return thestatic;
}
/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse1" } } */
/* { dg-final { scan-tree-dump-times "static variables to consider" 0 "dse2" } } */
/* { dg-final { cleanup-tree-dump "dse1" } } */
/* { dg-final { cleanup-tree-dump "dse2" } } */
