/* { dg-do compile { target { powerpc*-*-* && lp64 } } } */
/* { dg-options "-mcpu=e500mc64" } */

#include "builtin-isel.h"

/* Equality comparisons.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64eq

SIGNED64_PROTO
{
  L = ISEL_BUILTIN (x, y, a, b);
  U = ISEL_BUILTIN (x, y, (unsigned long) a, (unsigned long) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  L = ISEL_BUILTIN (p, q, a, b);
  /* Equality checks explicitly permit unsigned comparison operands.  */
  L = ISEL_BUILTIN ((unsigned long) x, (unsigned long) y, a, b);
  r = ISEL_BUILTIN ((unsigned long) x, (unsigned long) y, p, q);
}

/* less-than, greater-than.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64lt

SIGNED64_PROTO
{
  L = ISEL_BUILTIN (x, y, a, b);
  U = ISEL_BUILTIN (x, y, (unsigned long) a, (unsigned long) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
}

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64gt

SIGNED64_PROTO
{
  L = ISEL_BUILTIN (x, y, a, b);
  U = ISEL_BUILTIN (x, y, (unsigned long) a, (unsigned long) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
}

/* Unsigned variants.  These permit unsigned and pointer operands for
   comparison only.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64ltu

UNSIGNED64_PROTO
{
  L = ISEL_BUILTIN (x, y, a, b);
  U = ISEL_BUILTIN (x, y, (unsigned long) a, (unsigned long) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  L = ISEL_BUILTIN (p, q, a, b);
}

#undef FUNCTION_NAME
#define FUNCTION_NAME isel64gtu

UNSIGNED64_PROTO
{
  L = ISEL_BUILTIN (x, y, a, b);
  U = ISEL_BUILTIN (x, y, (unsigned long) a, (unsigned long) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  L = ISEL_BUILTIN (p, q, a, b);
}

/* Don't use bare isel, as that'll match function names and the like.  */
/* { dg-final { scan-assembler "isel " 25 } } */
