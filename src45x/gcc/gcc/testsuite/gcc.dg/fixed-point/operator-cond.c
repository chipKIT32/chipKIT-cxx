/* { dg-do run } */
/* { dg-options "-std=gnu99 -O0" } */

/* C99 6.5.15 Conditional operator.
   Test with fixed-point operands.
   Based on the test from ../dfp/.  */

#include "longlong-support.h"

extern void abort (void);
volatile int yes, no;

#define FUNC(TYPE,NAME,PF) \
volatile TYPE NAME ## a, NAME ## b, NAME ## c; \
void \
init_ ## NAME () \
{ \
  NAME ## b = 0.2 ## PF; \
  NAME ## c = 0.3 ## PF; \
  yes = 1; \
  no = 0; \
}

FUNC (short _Fract, sf, hr)
FUNC (_Fract, f, r)
FUNC (long _Fract, lf, lr)
LLFRACT_TEST (FUNC (long long _Fract, llf, llr))
FUNC (unsigned short _Fract, usf, uhr)
FUNC (unsigned _Fract, uf, ur)
FUNC (unsigned long _Fract, ulf, ulr)
LLFRACT_TEST (FUNC (long long _Fract, ullf, ullr))
FUNC (_Sat short _Fract, Ssf, hr)
FUNC (_Sat _Fract, Sf, r)
FUNC (_Sat long _Fract, Slf, lr)
LLFRACT_TEST (FUNC (_Sat long long _Fract, Sllf, llr))
FUNC (_Sat unsigned short _Fract, Susf, uhr)
FUNC (_Sat unsigned _Fract, Suf, ur)
FUNC (_Sat unsigned long _Fract, Sulf, ulr)
LLFRACT_TEST (FUNC (_Sat long long _Fract, Sullf, ullr))
FUNC (short _Accum, sa, hk)
FUNC (_Accum, a, k)
FUNC (long _Accum, la, lk)
LLACCUM_TEST (FUNC (long long _Accum, lla, llk))
FUNC (unsigned short _Accum, usa, uhk)
FUNC (unsigned _Accum, ua, uk)
FUNC (unsigned long _Accum, ula, ulk)
LLACCUM_TEST (FUNC (long long _Accum, ulla, ullk))
FUNC (_Sat short _Accum, Ssa, hk)
FUNC (_Sat _Accum, Sa, k)
FUNC (_Sat long _Accum, Sla, lk)
LLACCUM_TEST (FUNC (_Sat long long _Accum, Slla, llk))
FUNC (_Sat unsigned short _Accum, Susa, uhk)
FUNC (_Sat unsigned _Accum, Sua, uk)
FUNC (_Sat unsigned long _Accum, Sula, ulk)
LLACCUM_TEST (FUNC (_Sat long long _Accum, Sulla, ullk))

int
main ()
{
#define TEST(NAME) \
  init_ ## NAME (); \
  NAME ## a = yes ? NAME ## b : NAME ## c; \
  if (NAME ## a != NAME ## b) \
    abort (); \
  NAME ## a = no ? NAME ## b : NAME ## c; \
  if (NAME ## a != NAME ## c) \
    abort ();

  TEST(sf)
  TEST(f)
  TEST(lf)
  LLFRACT_TEST (TEST(llf))
  TEST(usf)
  TEST(uf)
  TEST(ulf)
  ULLFRACT_TEST (TEST(ullf))
  TEST(Ssf)
  TEST(Sf)
  TEST(Slf)
  LLFRACT_TEST (TEST(Sllf))
  TEST(Susf)
  TEST(Suf)
  TEST(Sulf)
  ULLFRACT_TEST (TEST(Sullf))
  TEST(sa)
  TEST(a)
  TEST(la)
  LLACCUM_TEST (TEST(lla))
  TEST(usa)
  TEST(ua)
  TEST(ula)
  ULLACCUM_TEST (TEST(ulla))
  TEST(Ssa)
  TEST(Sa)
  TEST(Sla)
  LLACCUM_TEST (TEST(Slla))
  TEST(Susa)
  TEST(Sua)
  TEST(Sula)
  ULLACCUM_TEST (TEST(Sulla))

  return 0;
}
