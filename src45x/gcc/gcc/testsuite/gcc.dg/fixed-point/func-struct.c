/* { dg-do run } */
/* { dg-options "-std=gnu99" } */

/* C99 6.5.2.2 Function calls.
   Test structure passing and return values involving fixed-point types.
   Based on the test from ../dfp/.  */

#include "longlong-support.h"

extern void abort (void);

struct example
{
  short _Fract sf;
  char dummy1;
  _Fract f;
  char dummy2;
  long _Fract lf;
  char dummy3;
#if DO_LLFRACT_TESTS
  long long _Fract llf;
  char dummy4;
#endif
  unsigned short _Fract usf;
  char dummy5;
  unsigned _Fract uf;
  char dummy6;
  unsigned long _Fract ulf;
  char dummy7;
#if DO_ULLFRACT_TESTS
  unsigned long long _Fract ullf;
  char dummy8;
#endif
  _Sat short _Fract Ssf;
  char dummy9;
  _Sat _Fract Sf;
  char dummy10;
  _Sat long _Fract Slf;
  char dummy11;
#if DO_LLFRACT_TESTS
  _Sat long long _Fract Sllf;
  char dummy12;
#endif
  _Sat unsigned short _Fract Susf;
  char dummy13;
  _Sat unsigned _Fract Suf;
  char dummy14;
  _Sat unsigned long _Fract Sulf;
  char dummy15;
#if DO_ULLFRACT_TESTS
  _Sat unsigned long long _Fract Sullf;
  char dummy16;
#endif
  short _Accum sa;
  char dummya1;
  _Accum a;
  char dummya2;
  long _Accum la;
  char dummya3;
#if DO_LLACCUM_TESTS
  long long _Accum lla;
  char dummya4;
#endif
  unsigned short _Accum usa;
  char dummya5;
  unsigned _Accum ua;
  char dummya6;
  unsigned long _Accum ula;
  char dummya7;
#if DO_ULLACCUM_TESTS
  unsigned long long _Accum ulla;
  char dummya8;
#endif
  _Sat short _Accum Ssa;
  char dummya9;
  _Sat _Accum Sa;
  char dummya10;
  _Sat long _Accum Sla;
  char dummya11;
#if DO_LLACCUM_TESTS
  _Sat long long _Accum Slla;
  char dummya12;
#endif
  _Sat unsigned short _Accum Susa;
  char dummya13;
  _Sat unsigned _Accum Sua;
  char dummya14;
  _Sat unsigned long _Accum Sula;
  char dummya15;
#if DO_ULLACCUM_TESTS
  _Sat unsigned long long _Accum Sulla;
  char dummya16;
#endif
} nums = { 0.1hr, 'a',
	   0.2r, 'b',
	   0.3lr, 'c',
#if DO_LLFRACT_TESTS
	   0.4llr, 'd',
#endif
	   0.5uhr, 'e',
	   0.6ur, 'f',
	   0.7ulr, 'g',
#if DO_ULLFRACT_TESTS
	   0.8ullr, 'h',
#endif
	   0.11r, 'i',
	   0.22r, 'j',
	   0.33lr, 'k',
#if DO_LLFRACT_TESTS
	   0.44llr, 'l',
#endif
	   0.55uhr, 'm',
	   0.66ur, 'n',
	   0.77ulr, 'o',
#if DO_ULLFRACT_TESTS
	   0.88ullr, 'p',
#endif
	   0.1hk, 'q',
	   0.2k, 'r',
	   0.3lk, 's',
#if DO_LLACCUM_TESTS
	   0.4llk, 't',
#endif
	   0.5uhk, 'u',
	   0.6uk, 'v',
	   0.7ulk, 'w',
#if DO_ULLACCUM_TESTS
	   0.8ullk, 'x',
#endif
	   0.11k, 'y',
	   0.22k, 'z',
	   0.33lk, '0',
#if DO_LLACCUM_TESTS
	   0.44llk, '1',
#endif
	   0.55uhk, '2',
	   0.66uk, '3',
	   0.77ulk, '4',
#if DO_ULLACCUM_TESTS
	   0.88ullk, '5'
#endif
	 };

/* A handful of functions that return the Nth argument of
   an incoming array.  */

#define FUNC(TYPE,NAME) \
TYPE NAME ## _func (struct example s) \
  { return s. NAME; }

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
#define TEST(TYPE,NAME,VALUE) \
  { \
    if (NAME ## _func (nums) != VALUE) abort (); \
  }

  TEST (short _Fract, sf, 0.1hr)
  TEST (_Fract, f, 0.2r)
  TEST (long _Fract, lf, 0.3lr)
  LLFRACT_TEST (TEST (long long _Fract, llf, 0.4llr))
  TEST (unsigned short _Fract, usf, 0.5uhr)
  TEST (unsigned _Fract, uf, 0.6ur)
  TEST (unsigned long _Fract, ulf, 0.7ulr)
  ULLFRACT_TEST (TEST (unsigned long long _Fract, ullf, 0.8ullr))
  TEST (_Sat short _Fract, Ssf, 0.11hr)
  TEST (_Sat _Fract, Sf, 0.22r)
  TEST (_Sat long _Fract, Slf, 0.33lr)
  LLFRACT_TEST (TEST (_Sat long long _Fract, Sllf, 0.44llr))
  TEST (_Sat unsigned short _Fract, Susf, 0.55uhr)
  TEST (_Sat unsigned _Fract, Suf, 0.66ur)
  TEST (_Sat unsigned long _Fract, Sulf, 0.77ulr)
  ULLFRACT_TEST (TEST (_Sat unsigned long long _Fract, Sullf, 0.88ullr))
  TEST (short _Accum, sa, 0.1hk)
  TEST (_Accum, a, 0.2k)
  TEST (long _Accum, la, 0.3lk)
  LLACCUM_TEST (TEST (long long _Accum, lla, 0.4llk))
  TEST (unsigned short _Accum, usa, 0.5uhk)
  TEST (unsigned _Accum, ua, 0.6uk)
  TEST (unsigned long _Accum, ula, 0.7ulk)
  ULLACCUM_TEST (TEST (unsigned long long _Accum, ulla, 0.8ullk))
  TEST (_Sat short _Accum, Ssa, 0.11hk)
  TEST (_Sat _Accum, Sa, 0.22k)
  TEST (_Sat long _Accum, Sla, 0.33lk)
  LLACCUM_TEST (TEST (_Sat long long _Accum, Slla, 0.44llk))
  TEST (_Sat unsigned short _Accum, Susa, 0.55uhk)
  TEST (_Sat unsigned _Accum, Sua, 0.66uk)
  TEST (_Sat unsigned long _Accum, Sula, 0.77ulk)
  ULLACCUM_TEST (TEST (_Sat unsigned long long _Accum, Sulla, 0.88ullk))

  return 0;
}
