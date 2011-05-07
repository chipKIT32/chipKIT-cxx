/* ColdFire has restricted addressing modes for float operands.  */
/* { dg-do compile { target m68k-*-* } }  */
/* { dg-options "-O0 -mcpu=547x -mhard-float" }  */

double Foo (unsigned a)
{
  unsigned local_data[16384] __attribute__((unused));
  double d;

  d = a;

  return d;
}
