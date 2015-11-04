#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifndef HAVE_STRCHR
# define strchr index
# define strrchr rindex
#endif

#if HAVE_STRING_H
# include <string.h>
#else
char *strchr(), *strrchr();
#endif

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#  define memmove(d, s, n) bcopy ((s), (d), (n))
#endif

#ifdef HAVE_UNISTD_H
#include <sys/types.h>
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#else
#ifdef HAVE_SYS_FILE_H
#include <sys/file.h>
#endif
#endif

#ifndef SEEK_SET
#define SEEK_SET 0
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#include <stdio.h>
#include <errno.h>

#if defined(__GNUC__) && !defined(C_ALLOCA)
# undef alloca
# define alloca __builtin_alloca
#else
# if defined(HAVE_ALLOCA_H) && !defined(C_ALLOCA)
#  include <alloca.h>
# else
#  ifndef alloca /* predefined by HP cc +Olibcalls */
#   if !defined (__STDC__) && !defined (__hpux)
char *alloca ();
#   else
void *alloca ();
#   endif /* __STDC__, __hpux */
#  endif /* alloca */
# endif /* HAVE_ALLOCA_H */
#endif

#ifdef NEED_DECLARATION_MALLOC
extern void *malloc (size_t);
#endif
#ifdef NEED_DECLARATION_REALLOC
extern void *realloc (void *, size_t);
#endif
#ifdef NEED_DECLARATION_FREE
extern void free (void *);
#endif
#ifdef NEED_DECLARATION_STRTOK
extern char *strtok (char *, const char *);
#endif

#ifdef USE_BINARY_FOPEN
#define FOPEN_RB	"rb"
#define FOPEN_WB 	"wb"
#define FOPEN_AB 	"ab"
#define FOPEN_RUB 	"r+b"
#define FOPEN_WUB 	"w+b"
#define FOPEN_AUB 	"a+b"
#else
#define FOPEN_RB	"r"
#define FOPEN_WB 	"w"
#define FOPEN_AB 	"a"
#define FOPEN_RUB 	"r+"
#define FOPEN_WUB 	"w+"
#define FOPEN_AUB 	"a+"
#endif

#define FOPEN_RT	"r"
#define FOPEN_WT 	"w"
#define FOPEN_AT 	"a"
#define FOPEN_RUT 	"r+"
#define FOPEN_WUT 	"w+"
#define FOPEN_AUT 	"a+"

/* local utility functions */
extern void * xmalloc (size_t);
