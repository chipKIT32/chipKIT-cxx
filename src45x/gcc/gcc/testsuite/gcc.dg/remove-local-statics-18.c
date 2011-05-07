/* Verify that we do not eliminate the static variable `x' when `func'
   is inlined and its return value become dead.  */

/* { dg-do run } */
/* { dg-output "" } */
/* { dg-options "-O2 -fremove-local-statics" } */

void abort(void);

static int guard;
static int func(int y)
{
  static int x;
  if (guard == 0)
    {
      x = y;
      guard = 1;
    }
  return x + y;
}

int __attribute__((noinline)) call1(int a)
{
  func(a);
  return 0;
}

int __attribute__((noinline)) call2(int a)
{
  return func(a);
}

int global1 = 3;
int global2 = 5;

extern int printf (const char *, ...);

int main()
{
  call1 (global1);
  printf ("call2: %d\n", call2(global2));
#if 0
  if (call2 (global2) != 8)
    abort ();
#endif
  return 0;
}
