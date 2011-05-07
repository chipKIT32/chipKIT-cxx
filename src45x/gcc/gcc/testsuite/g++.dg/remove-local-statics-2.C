/* Verify that we do not eliminate a static variable in
   main due to its use in Local::Foo.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

int
main (void)
{
   static int thestatic = 0;
   struct Local {
     __attribute__((__noinline__))
     static int Foo () { return thestatic; }
   };

   thestatic = 2;
   thestatic = Local::Foo();

   return thestatic++;
}
