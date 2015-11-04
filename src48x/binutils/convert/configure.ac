dnl Process this file with autoconf to produce a configure script.

AC_INIT(convert, 3.0.4)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AC_PREREQ(2.59)
AC_CONFIG_SRCDIR(convert.c)

AC_PROG_CC
AC_PROG_INSTALL

AC_ISC_POSIX

AC_HEADER_STDC
AC_CONFIG_HEADERS([config.h])
AC_CHECK_HEADERS(stddef.h stdlib.h strings.h unistd.h limits.h)
AC_CHECK_HEADERS(fcntl.h sys/file.h)

AC_C_CONST
AC_TYPE_SIZE_T

AC_CHECK_FUNCS(strchr strtok memcpy)
AC_FUNC_ALLOCA

CONV_BINARY_FOPEN

CONV_NEED_DECLARATION(malloc)
CONV_NEED_DECLARATION(realloc)
CONV_NEED_DECLARATION(free)
CONV_NEED_DECLARATION(strtok)

ACX_PKGVERSION([MIPS Convert])
ACX_BUGURL([http://support.codesourcery.com/])

AM_MAINTAINER_MODE

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
