/* { dg-do run } */
/* { dg-options "-O2" } */

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define CHECK(n, c) if (!(c)) { printf ("error %d at line %d", n,  __LINE__); return 1; }
#else
#define CHECK(n, c) if (!(c)) { return 1; }
#endif

#define TEST_OP(T, f, i, o, m, res)             \
  {                                             \
    volatile T a;                               \
    T r;                                        \
    a = i;                                      \
    CHECK (1, a == i);                          \
    r = f (&a, o);                              \
    CHECK (2, a == m);                          \
    CHECK (3, r == res);                        \
    a = i;                                      \
    CHECK (4, a == i);                          \
    (void)f (&a, o);                            \
    CHECK (5, a == m);                          \
  }

#define TEST_OP2(T, f, i, o, o2, m, res)        \
  {                                             \
    volatile T a;                               \
    T r;                                        \
    a = i;                                      \
    CHECK (1, a == i);                          \
    r = f (&a, o, o2);                          \
    CHECK (2, a == m);                          \
    CHECK (3, r == res);                        \
    a = i;                                      \
    CHECK (4, a == i);                          \
    (void)f (&a, o, o2);                        \
    CHECK (5, a == m);                          \
  }

int i0 = 0, i1 = 1, i2 = 2;
long long ll0 = 0, ll1 = 1, ll2 = 2;

int main()
{

  /* Swap.   */
  TEST_OP (int, __builtin_mips_swap_acq, 1, 0, 0, 1);
  TEST_OP (int, __builtin_mips_swap_rel, 1, 0, 0, 1);
  TEST_OP (int, __sync_lock_test_and_set, 1, 0, 0, 1);
  TEST_OP (int, __builtin_mips_swap_acq, 1, i0, 0, 1);
  TEST_OP (int, __builtin_mips_swap_rel, 1, i0, 0, 1);
  TEST_OP (int, __sync_lock_test_and_set, 1, i0, 0, 1);

  /* Failed val swap.  */
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_acq, 1, 0, 2, 1, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_rel, 1, 0, 2, 1, 1);
  TEST_OP2 (int, __sync_val_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_acq, 1, i0, i2, 1, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_rel, 1, i0, i2, 1, 1);
  TEST_OP2 (int, __sync_val_compare_and_swap, 1, i0, i2, 1, 1);

  /* Successful val swap.  */
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_acq, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_rel, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __sync_val_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_acq, 1, i1, i2, 2, 1);
  TEST_OP2 (int, __builtin_mips_val_compare_and_swap_rel, 1, i1, i2, 2, 1);
  TEST_OP2 (int, __sync_val_compare_and_swap, 1, i1, i2, 2, 1);

  /* Failed bool swap.  */
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_acq, 1, 0, 2, 1, 0);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_rel, 1, 0, 2, 1, 0);
  TEST_OP2 (int, __sync_bool_compare_and_swap, 1, 0, 2, 1, 0);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_acq, 1, i0, i2, 1, 0);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_rel, 1, i0, i2, 1, 0);
  TEST_OP2 (int, __sync_bool_compare_and_swap, 1, i0, i2, 1, 0);

  /* Successful bool swap.  */
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_acq, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_rel, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __sync_bool_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_acq, 1, i1, i2, 2, 1);
  TEST_OP2 (int, __builtin_mips_bool_compare_and_swap_rel, 1, i1, i2, 2, 1);
  TEST_OP2 (int, __sync_bool_compare_and_swap, 1, i1, i2, 2, 1);

  /* Add. */
  TEST_OP (int, __sync_add_and_fetch, 1, 1, 2, 2);
  TEST_OP (int, __sync_fetch_and_add, 1, 1, 2, 1);
  TEST_OP (int, __sync_add_and_fetch, 1, i1, 2, 2);
  TEST_OP (int, __sync_fetch_and_add, 1, i1, 2, 1);

  /* Sub.  */
  TEST_OP (int, __sync_sub_and_fetch, 2, 1, 1, 1);
  TEST_OP (int, __sync_fetch_and_sub, 2, 1, 1, 2);
  TEST_OP (int, __sync_sub_and_fetch, 2, i1, 1, 1);
  TEST_OP (int, __sync_fetch_and_sub, 2, i1, 1, 2);

#if _MIPS_SIM == _ABIO32
#else

  /* Swap.   */
  TEST_OP (long long, __builtin_mips_swapd_acq, 1, 0, 0, 1);
  TEST_OP (long long, __builtin_mips_swapd_rel, 1, 0, 0, 1);
  TEST_OP (long long, __sync_lock_test_and_set, 1, 0, 0, 1);
  TEST_OP (long long, __builtin_mips_swapd_acq, 1, ll0, 0, 1);
  TEST_OP (long long, __builtin_mips_swapd_rel, 1, ll0, 0, 1);
  TEST_OP (long long, __sync_lock_test_and_set, 1, ll0, 0, 1);

  /* Failed val swapd.  */
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_acq, 1, 0, 2, 1, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_rel, 1, 0, 2, 1, 1);
  TEST_OP2 (long long, __sync_val_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_acq, 1, ll0, ll2, 1, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_rel, 1, ll0, ll2, 1, 1);
  TEST_OP2 (long long, __sync_val_compare_and_swap, 1, ll0, ll2, 1, 1);

  /* Successful val swapd.  */
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_acq, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_rel, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __sync_val_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_acq, 1, ll1, ll2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_val_compare_and_swapd_rel, 1, ll1, ll2, 2, 1);
  TEST_OP2 (long long, __sync_val_compare_and_swap, 1, ll1, ll2, 2, 1);

  /* Failed bool swapd.  */
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_acq, 1, 0, 2, 1, 0);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_rel, 1, 0, 2, 1, 0);
  TEST_OP2 (long long, __sync_bool_compare_and_swap, 1, 0, 2, 1, 0);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_acq, 1, ll0, ll2, 1, 0);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_rel, 1, ll0, ll2, 1, 0);
  TEST_OP2 (long long, __sync_bool_compare_and_swap, 1, ll0, ll2, 1, 0);

  /* Successful bool swapd.  */
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_acq, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_rel, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __sync_bool_compare_and_swap, 1, 1, 2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_acq, 1, ll1, ll2, 2, 1);
  TEST_OP2 (long long, __builtin_mips_bool_compare_and_swapd_rel, 1, ll1, ll2, 2, 1);
  TEST_OP2 (long long, __sync_bool_compare_and_swap, 1, ll1, ll2, 2, 1);

  /* Add. */
  TEST_OP (long long, __sync_add_and_fetch, 1, 1, 2, 2);
  TEST_OP (long long, __sync_fetch_and_add, 1, 1, 2, 1);
  TEST_OP (long long, __sync_add_and_fetch, 1, ll1, 2, 2);
  TEST_OP (long long, __sync_fetch_and_add, 1, ll1, 2, 1);

  /* Sub.  */
  TEST_OP (long long, __sync_sub_and_fetch, 2, 1, 1, 1);
  TEST_OP (long long, __sync_fetch_and_sub, 2, 1, 1, 2);
  TEST_OP (long long, __sync_sub_and_fetch, 2, ll1, 1, 1);
  TEST_OP (long long, __sync_fetch_and_sub, 2, ll1, 1, 2);

#endif

  return 0;
}
