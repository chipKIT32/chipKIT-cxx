/* { dg-do compile } */
/* { dg-options "-mcpu=e500mc" } */

#include "builtin-isel.h"

/* We're not being clever with the preprocessor here because DejaGNU
   will get confused.  We do try to use it to eliminate what duplication
   we can.  */

/* We check to see that the resolution permits polymorphic results.  */

/* Equality comparisons.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME iseleq

SIGNED_PROTO
{
  i = ISEL_BUILTIN (x, y, a, b);
  u = ISEL_BUILTIN (x, y, (unsigned int) a, (unsigned int) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  i = ISEL_BUILTIN (p, q, a, b);
  /* Equality checks explicitly permit unsigned comparison operands.  */
  i = ISEL_BUILTIN ((unsigned int) x, (unsigned int) y, a, b);
  r = ISEL_BUILTIN ((unsigned int) x, (unsigned int) y, p, q);
}

/* less-than, greater-than.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME isellt

SIGNED_PROTO
{
  i = ISEL_BUILTIN (x, y, a, b);
  u = ISEL_BUILTIN (x, y, (unsigned int) a, (unsigned int) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
}

#undef FUNCTION_NAME
#define FUNCTION_NAME iselgt

SIGNED_PROTO
{
  i = ISEL_BUILTIN (x, y, a, b);
  u = ISEL_BUILTIN (x, y, (unsigned int) a, (unsigned int) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
}

/* Unsigned variants.  These permit unsigned and pointer operands for
   comparison only.  */

#undef FUNCTION_NAME
#define FUNCTION_NAME iselltu

UNSIGNED_PROTO
{
  i = ISEL_BUILTIN (x, y, a, b);
  u = ISEL_BUILTIN (x, y, (unsigned int) a, (unsigned int) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  i = ISEL_BUILTIN (p, q, a, b);
}

#undef FUNCTION_NAME
#define FUNCTION_NAME iselgtu

UNSIGNED_PROTO
{
  i = ISEL_BUILTIN (x, y, a, b);
  u = ISEL_BUILTIN (x, y, (unsigned int) a, (unsigned int) b);
  r = ISEL_BUILTIN (x, y, p, q);
  r = ISEL_BUILTIN (x, y, (char *) p, (char *) q);
  i = ISEL_BUILTIN (p, q, a, b);
}

/* Don't use bare isel, as that'll match function names and the like.  */
/* { dg-final { scan-assembler-times "isel " 25 } } */
