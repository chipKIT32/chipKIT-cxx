/* Do the type-generic tests.  Unlike pr28796-2.c, we test these
   without any fast-math flags.  */

/* { dg-do run } */
/* { dg-skip-if "No Inf/NaN support" { spu-*-* } } */
/* { dg-add-options ieee } */
/* { dg-options "-Wl,--defsym=__cs3_mips_float_type=2 -lcs3-mips-cp1 -lcs3-mips-fpemu" { target mips*-*sde*-* } } */

#include "../tg-tests.h"

int main(void)
{
  return main_tests ();
}
