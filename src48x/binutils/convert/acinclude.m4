dnl See whether we need to use fopen-bin.h rather than fopen-same.h.
AC_DEFUN([CONV_BINARY_FOPEN],
[AC_REQUIRE([AC_CANONICAL_HOST])
case "${host}" in
changequote(,)dnl
*-*-msdos* | *-*-go32* | *-*-mingw32* | *-*-cygwin* | *-*-windows)
changequote([,])dnl
  AC_DEFINE([USE_BINARY_FOPEN], 1, [Use b modifier when opening binary files?]) ;;
esac])dnl

dnl See whether we need a declaration for a function.
AC_DEFUN([CONV_NEED_DECLARATION],
[AC_MSG_CHECKING([whether $1 must be declared])
AC_CACHE_VAL(bfd_cv_decl_needed_$1,
[AC_TRY_COMPILE([
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#else
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif],
[char *(*pfn) = (char *(*)) $1],
bfd_cv_decl_needed_$1=no, bfd_cv_decl_needed_$1=yes)])
AC_MSG_RESULT($bfd_cv_decl_needed_$1)
if test $bfd_cv_decl_needed_$1 = yes; then
  bfd_tr_decl=NEED_DECLARATION_`echo $1 | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`
  AC_DEFINE_UNQUOTED($bfd_tr_decl, 1, [Define if a declaration is needed.])
fi
])dnl
