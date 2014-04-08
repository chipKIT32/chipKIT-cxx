/* Test rejection of invalid parameter combinations in isel builtins.  */
/* { dg-do compile { target { powerpc*-*-* && lp64 } } } */
/* { dg-options "-mcpu=e500mc64" } */

#include "builtin-isel.h"


/* Equality comparisons.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64eq

SIGNED64_PROTO
{
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned long) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned long) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((long *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (long *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned long) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned long) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (long *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (long *) q); /* { dg-error "isel intrinsic" } */
}

/* less-than, greater-than.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64lt

SIGNED64_PROTO
{
  /* Unsigned comparison should be done with the *u variants.  */
  ISEL_BUILTIN ((unsigned long) x, (unsigned long) y, a, b); /* { dg-error "isel intrinsic" } */
  /* So should pointer comparison.  */
  ISEL_BUILTIN (p, q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned long) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned long) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((long *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (long *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned long) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned long) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (long *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (long *) q); /* { dg-error "isel intrinsic" } */
}

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64gt

SIGNED64_PROTO
{
  /* Unsigned comparison should be done with the *u variants.  */
  ISEL_BUILTIN ((unsigned long) x, (unsigned long) y, a, b); /* { dg-error "isel intrinsic" } */
  /* So should pointer comparison.  */
  ISEL_BUILTIN (p, q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned long) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned long) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((long *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (long *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned long) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned long) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (long *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (long *) q); /* { dg-error "isel intrinsic" } */
}

/* Unsigned variants.  These permit unsigned and pointer operands for
   comparison only.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64ltu

UNSIGNED64_PROTO
{
  /* Signed comparison should be done with the signed variants.  */
  ISEL_BUILTIN ((long) x, (long) y, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((long) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (long) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((long *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (long *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned long) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned long) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (long *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (long *) q); /* { dg-error "isel intrinsic" } */
}

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64gtu

UNSIGNED64_PROTO
{
  /* Signed comparison should be done with the signed variants.  */
  ISEL_BUILTIN ((long) x, (long) y, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((long) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (long) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((long *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (long *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned long) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned long) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (long *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (long *) q); /* { dg-error "isel intrinsic" } */
}
