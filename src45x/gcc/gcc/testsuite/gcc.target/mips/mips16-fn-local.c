/* { dg-options "(-mips16) -fpic -minterlink-mips16" } */
/* { dg-do link } */
MIPS16 static double
fn1(void) { return 0.0; }
MIPS16 static double
fn2(double d) { return 0.0; }
MIPS16 void
main(void) {
  volatile double d;
  d = fn1();
  d = fn2(0.0);
}
