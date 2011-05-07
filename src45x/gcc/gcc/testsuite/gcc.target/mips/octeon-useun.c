/* Check the mov_u[ls][dw] patterns.  */
/* { dg-options "-march=octeon -O2 -mabi=64 -mocteon-useun -meb" } */
struct __attribute__((packed)) sl { unsigned long x; };
struct __attribute__((packed)) si { unsigned int x; };
unsigned long f1 (struct sl *s) { return s[0].x; };
unsigned int f2 (struct si *s) { return s[1].x; };
void f3 (struct sl *s, unsigned long x) { s[10].x = x; }
void f4 (struct si *s, unsigned int x) { s[11].x = x; }
void f5 (struct sl *s) { s[100].x = 0; }
void f6 (struct si *s) { s[101].x = 0; }
/* { dg-final { scan-assembler "\tjr?\t\\\$31\n\tuld\t\\\$2,0\\(\\\$4\\)\n" } } */
/* { dg-final { scan-assembler "\tulw\t\\\$2,4\\(\\\$4\\)\n" } } */
/* { dg-final { scan-assembler "\tjr?\t\\\$31\n\tusd\t\\\$5,80\\(\\\$4\\)\n" } } */
/* { dg-final { scan-assembler "\tjr?\t\\\$31\n\tusw\t\\\$5,44\\(\\\$4\\)\n" } } */
/* { dg-final { scan-assembler "\tjr?\t\\\$31\n\tusd\t\\\$0,800\\(\\\$4\\)\n" } } */
/* { dg-final { scan-assembler "\tjr?\t\\\$31\n\tusw\t\\\$0,404\\(\\\$4\\)\n" } } */
