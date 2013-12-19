/* Common definitions for builtin isel testing.  */

#define SIGNED_ARGLIST (int x, int y, int a, int b, void *p, void *q)
#define UNSIGNED_ARGLIST (unsigned int x, unsigned int y, \
			  int a, int b, void *p, void *q)

#define SIGNED_PROTO void FUNCTION_NAME SIGNED_ARGLIST
#define UNSIGNED_PROTO void FUNCTION_NAME UNSIGNED_ARGLIST

#define SIGNED64_ARGLIST (long x, long y, long a, long b, void *p, void *q)
#define UNSIGNED64_ARGLIST (unsigned long x, unsigned long y, \
			    long a, long b, void *p, void *q)

#define SIGNED64_PROTO void FUNCTION_NAME SIGNED64_ARGLIST
#define UNSIGNED64_PROTO void FUNCTION_NAME UNSIGNED64_ARGLIST

#define CONCAT2(X,Y) X##Y
#define CONCAT(X,Y) CONCAT2(X, Y)
#define ISEL_BUILTIN CONCAT(__builtin_, FUNCTION_NAME)

volatile int i;
volatile unsigned int u;
volatile void *r;
volatile long L;
volatile unsigned long U;
