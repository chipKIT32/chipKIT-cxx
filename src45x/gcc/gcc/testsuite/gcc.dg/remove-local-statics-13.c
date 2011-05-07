/* We used to ICE on this test, because the call to BAR appeared to
   define both static variables in FOO.  Verify that we no longer do
   this.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "static1" } } */
/* { dg-final { scan-assembler-not "static2" } } */

int foo(int i) {
  static int static1 = 0;
  static int static2;

  if (static2 = bar(i))
    static1 = 1;
  static2 = static1 + 30;

  return static1 + static2;
}

int bar(int i) {
  if (i) { foo(i-1); return 0; }
  return 1;
}
