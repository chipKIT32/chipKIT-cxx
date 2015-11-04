/* { dg-do run } */
/* { dg-options "-std=gnu99" } */

/* C99 6.5.2.2 Function calls.
   Test passing varargs of fixed-point types.
   Based on the test from ../dfp/.  */

#include <stdarg.h>
#include "longlong-support.h"

extern void abort (void);

#define FUNC(TYPE, NAME) \
static TYPE \
vararg_ ## NAME (unsigned arg, ...) \
{ \
  int i; \
  va_list ap; \
  TYPE result; \
  va_start (ap, arg); \
  for (i = 0; i <= arg; i++) \
    result = va_arg (ap, TYPE); \
  va_end (ap); \
  return result; \
}

FUNC (short _Fract, sf)
FUNC (_Fract, f)
FUNC (long _Fract, lf)
LLFRACT_TEST (FUNC (long long _Fract, llf))
FUNC (unsigned short _Fract, usf)
FUNC (unsigned _Fract, uf)
FUNC (unsigned long _Fract, ulf)
ULLFRACT_TEST (FUNC (unsigned long long _Fract, ullf))
FUNC (_Sat short _Fract, Ssf)
FUNC (_Sat _Fract, Sf)
FUNC (_Sat long _Fract, Slf)
LLFRACT_TEST (FUNC (_Sat long long _Fract, Sllf))
FUNC (_Sat unsigned short _Fract, Susf)
FUNC (_Sat unsigned _Fract, Suf)
FUNC (_Sat unsigned long _Fract, Sulf)
ULLFRACT_TEST (FUNC (_Sat unsigned long long _Fract, Sullf))
FUNC (short _Accum, sa)
FUNC (_Accum, a)
FUNC (long _Accum, la)
LLACCUM_TEST (FUNC (long long _Accum, lla))
FUNC (unsigned short _Accum, usa)
FUNC (unsigned _Accum, ua)
FUNC (unsigned long _Accum, ula)
ULLACCUM_TEST (FUNC (unsigned long long _Accum, ulla))
FUNC (_Sat short _Accum, Ssa)
FUNC (_Sat _Accum, Sa)
FUNC (_Sat long _Accum, Sla)
LLACCUM_TEST (FUNC (_Sat long long _Accum, Slla))
FUNC (_Sat unsigned short _Accum, Susa)
FUNC (_Sat unsigned _Accum, Sua)
FUNC (_Sat unsigned long _Accum, Sula)
ULLACCUM_TEST (FUNC (_Sat unsigned long long _Accum, Sulla))

int main()
{
#define TEST(NAME,PF) \
  if (vararg_ ## NAME (0, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.0 ## PF) \
    abort (); \
  if (vararg_ ## NAME (1, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.1 ## PF) \
    abort (); \
  if (vararg_ ## NAME (2, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.2 ## PF) \
    abort (); \
  if (vararg_ ## NAME (3, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.3 ## PF) \
    abort (); \
  if (vararg_ ## NAME (4, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.4 ## PF) \
    abort (); \
  if (vararg_ ## NAME (5, 0.0 ## PF, 0.1 ## PF, 0.2 ## PF, 0.3 ## PF, \
		       0.4 ## PF, 0.5 ## PF) != 0.5 ## PF) \
    abort ();

  TEST(sf, hr)
  TEST(f, r)
  TEST(lf, lr)
  LLFRACT_TEST (TEST(llf, llr))
  TEST(usf, uhr)
  TEST(uf, ur)
  TEST(ulf, ulr)
  ULLFRACT_TEST (TEST(ullf, ullr))
  TEST(Ssf, hr)
  TEST(Sf, r)
  TEST(Slf, lr)
  LLFRACT_TEST (TEST(Sllf, llr))
  TEST(Susf, uhr)
  TEST(Suf, ur)
  TEST(Sulf, ulr)
  ULLFRACT_TEST (TEST(Sullf, ullr))
  TEST(sa, hk)
  TEST(a, k)
  TEST(la, lk)
  LLACCUM_TEST (TEST(lla, llk))
  TEST(usa, uhk)
  TEST(ua, uk)
  TEST(ula, ulk)
  ULLACCUM_TEST (TEST(ulla, ullk))
  TEST(Ssa, hk)
  TEST(Sa, k)
  TEST(Sla, lk)
  LLACCUM_TEST (TEST(Slla, llk))
  TEST(Susa, uhk)
  TEST(Sua, uk)
  TEST(Sula, ulk)
  ULLACCUM_TEST (TEST(Sulla, ullk))

  return 0;
}
