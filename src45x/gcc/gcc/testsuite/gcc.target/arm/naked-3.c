/* { dg-do compile } */
/* { dg-options "-O2 -Wall" } */
/* Check that we do not get warnings about missing return statements
   or bogus looking noreturn functions.  */
int __attribute__((naked))
foo(void)
{
  __asm__ volatile ("mov r0, #1\r\nbx lr\n");
}

int __attribute__((naked,noreturn))
bar(void)
{
  __asm__ volatile ("frob r0\n");
}
