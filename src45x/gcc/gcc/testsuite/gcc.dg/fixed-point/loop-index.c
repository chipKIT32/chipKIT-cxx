/* { dg-do run } */
/* { dg-options "-std=gnu99" } */

/* C99 6.8.5.2: The for statement.
   Based on the test from ../dfp/.  */

#include "longlong-support.h"

extern void abort (void);

#define FUNC(TYPE,NAME,PF) \
void \
NAME (void) \
{ \
  TYPE d; \
  int i; \
  for (d = 0.1 ## PF, i = 0; d <= 0.8 ## PF; d += 0.1 ## PF) \
    i++; \
  if (i != 8) \
    abort(); \
}

FUNC (short _Fract, sf, hr)
FUNC (_Fract, f, r)
FUNC (long _Fract, lf, lr)
LLFRACT_TEST (FUNC (long long _Fract, llf, llr))
FUNC (unsigned short _Fract, usf, uhr)
FUNC (unsigned _Fract, uf, ur)
FUNC (unsigned long _Fract, ulf, ulr)
ULLFRACT_TEST (FUNC (unsigned long long _Fract, ullf, ullr))
FUNC (_Sat short _Fract, Ssf, hr)
FUNC (_Sat _Fract, Sf, r)
FUNC (_Sat long _Fract, Slf, lr)
LLFRACT_TEST (FUNC (_Sat long long _Fract, Sllf, llr))
FUNC (_Sat unsigned short _Fract, Susf, uhr)
FUNC (_Sat unsigned _Fract, Suf, ur)
FUNC (_Sat unsigned long _Fract, Sulf, ulr)
ULLFRACT_TEST (FUNC (_Sat unsigned long long _Fract, Sullf, ullr))
FUNC (short _Accum, sa, hk)
FUNC (_Accum, a, k)
FUNC (long _Accum, la, lk)
LLACCUM_TEST (FUNC (long long _Accum, lla, llk))
FUNC (unsigned short _Accum, usa, uhk)
FUNC (unsigned _Accum, ua, uk)
FUNC (unsigned long _Accum, ula, ulk)
ULLACCUM_TEST (FUNC (unsigned long long _Accum, ulla, ullk))
FUNC (_Sat short _Accum, Ssa, hk)
FUNC (_Sat _Accum, Sa, k)
FUNC (_Sat long _Accum, Sla, lk)
LLACCUM_TEST (FUNC (_Sat long long _Accum, Slla, llk))
FUNC (_Sat unsigned short _Accum, Susa, uhk)
FUNC (_Sat unsigned _Accum, Sua, uk)
FUNC (_Sat unsigned long _Accum, Sula, ulk)
ULLACCUM_TEST (FUNC (_Sat unsigned long long _Accum, Sulla, ullk))

int
main ()
{
  sf ();
  f ();
  lf ();
  LLFRACT_TEST (llf ());
  usf ();
  uf ();
  ulf ();
  ULLFRACT_TEST (ullf ());
  Ssf ();
  Sf ();
  Slf ();
  LLFRACT_TEST (Sllf ());
  Susf ();
  Suf ();
  Sulf ();
  ULLFRACT_TEST (Sullf ());
  sa ();
  a ();
  la ();
  LLACCUM_TEST (lla ());
  usa ();
  ua ();
  ula ();
  ULLACCUM_TEST (ulla ());
  Ssa ();
  Sa ();
  Sla ();
  LLACCUM_TEST (Slla ());
  Susa ();
  Sua ();
  Sula ();
  ULLACCUM_TEST (Sulla ());

  return 0;
}
