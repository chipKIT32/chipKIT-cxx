/* Verify that we do not eliminate a static variable in
   main::Local::Foo.  */

/* { dg-do compile } */
/* { dg-options "-O2 -fremove-local-statics" } */
/* { dg-final { scan-assembler "thestatic" } } */

int
main (void)
{
   static int thestatic = 0;
   struct Local {
     __attribute__((__noinline__))
     static void Foo () { thestatic = 1; }
   };

   thestatic = 2;
   Local::Foo();

   return thestatic++;
}
