/* Test rejection of invalid parameter combinations in isel builtins.  */
/* { dg-do compile } */
/* { dg-options "-mcpu=e500mc" } */

#include "builtin-isel.h"

/* We're not being clever with the preprocessor here because DejaGNU
   will get confused.  We do try to use it to eliminate what duplication
   we can.  */

/* We check basic resolution of each builtin.  We also check to see that
   the resolution permits polymorphic results.  Argument type mismatches
   and result type mismatches are not permitted, except where noted.  */

/* Equality comparisons.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME iseleq

SIGNED_PROTO
{
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned int) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned int) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((int *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (int *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned int) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned int) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (int *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (int *) q); /* { dg-error "isel intrinsic" } */
}

/* less-than, greater-than.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isellt

SIGNED_PROTO
{
  /* Unsigned comparison should be done with the *u variants.  */
  ISEL_BUILTIN ((unsigned int) x, (unsigned int) y, a, b); /* { dg-error "isel intrinsic" } */
  /* So should pointer comparison.  */
  ISEL_BUILTIN (p, q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned int) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned int) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((int *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (int *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned int) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned int) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (int *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (int *) q); /* { dg-error "isel intrinsic" } */
}

#undef FUNCTION_NAME
#define FUNCTION_NAME iselgt

SIGNED_PROTO
{
  /* Unsigned comparison should be done with the *u variants.  */
  ISEL_BUILTIN ((unsigned int) x, (unsigned int) y, a, b); /* { dg-error "isel intrinsic" } */
  /* So should pointer comparison.  */
  ISEL_BUILTIN (p, q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((unsigned int) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (unsigned int) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((int *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (int *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned int) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned int) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (int *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (int *) q); /* { dg-error "isel intrinsic" } */
}

/* Unsigned variants.  These permit unsigned and pointer operands for
   comparison only.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME iselltu

UNSIGNED_PROTO
{
  /* Signed comparison should be done with the signed variants.  */
  ISEL_BUILTIN ((int) x, (int) y, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((int) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (int) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((int *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (int *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned int) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned int) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (int *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (int *) q); /* { dg-error "isel intrinsic" } */
}

#undef FUNCTION_NAME
#define FUNCTION_NAME iselgtu

UNSIGNED_PROTO
{
  /* Signed comparison should be done with the signed variants.  */
  ISEL_BUILTIN ((int) x, (int) y, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in argument type.  */
  ISEL_BUILTIN ((int) x, y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, (int) y, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN ((int *) p, q, a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (p, (int *) q, a, b); /* { dg-error "isel intrinsic" } */
  /* Mismatches in return type.  */
  ISEL_BUILTIN (x, y, (unsigned int) a, b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, a, (unsigned int) b); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, (int *) p, q); /* { dg-error "isel intrinsic" } */
  ISEL_BUILTIN (x, y, p, (int *) q); /* { dg-error "isel intrinsic" } */
}
