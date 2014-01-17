#if __LLFRACT_FBIT__ <= (__SIZEOF_LONG_LONG__ * 8)
#define LLFRACT_TEST(X) X
#define DO_LLFRACT_TESTS 1
#else
#define LLFRACT_TEST(X)
#define DO_LLFRACT_TESTS 0
#endif

#if __ULLFRACT_FBIT__ <= (__SIZEOF_LONG_LONG__ * 8)
#define ULLFRACT_TEST(X) X
#define DO_ULLFRACT_TESTS 1
#else
#define ULLFRACT_TEST(X)
#define DO_ULLFRACT_TESTS 0
#endif

#if (__LLACCUM_IBIT__ + __LLACCUM_FBIT__) <= (__SIZEOF_LONG_LONG__ * 8)
#define LLACCUM_TEST(X) X
#define DO_LLACCUM_TESTS 1
#else
#define LLACCUM_TEST(X)
#define DO_LLACCUM_TESTS 0
#endif

#if (__ULLACCUM_IBIT__ + __ULLACCUM_FBIT__) <= (__SIZEOF_LONG_LONG__ * 8)
#define ULLACCUM_TEST(X) X
#define DO_ULLACCUM_TESTS 1
#else
#define ULLACCUM_TEST(X)
#define DO_ULLACCUM_TESTS 0
#endif
