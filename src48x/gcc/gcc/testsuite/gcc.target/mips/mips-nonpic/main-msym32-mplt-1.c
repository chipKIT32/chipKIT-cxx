/* Test support for PLTs using 32-bit symbols.  */
/* { dg-do run } */ 
/* { dg-options "-msym32 -mplt" } */

#include <stdio.h>

int
main ()
{
  printf ("Hello world!\n");
  return 0;
}
