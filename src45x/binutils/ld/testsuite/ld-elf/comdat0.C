#include "comdat.H"
#include <stdio.h>

void test::print ()
{
  printf ("(%p) x = %d\n", (void *)this, x);
}

test *maketest (int xx)
{
  return new test (xx);
}
