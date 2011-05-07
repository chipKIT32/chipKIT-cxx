/* Sourcery G++ IA32 GNU/Linux Configuration.
   Copyright (C) 2007
   Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* This configuration may be used either with the system glibc (in
   system32 and system64 subdirectories) or with the included glibc
   (in the sgxx-glibc subdirectory).  */

#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC			\
  "%{msgxx-glibc:/sgxx-glibc ;			\
     m64:/system64 ;				\
     mrhel3:/system64 ;				\
     mrh73:/system32-old ;			\
     :/system32}"

#undef SYSROOT_HEADERS_SUFFIX_SPEC
#define SYSROOT_HEADERS_SUFFIX_SPEC SYSROOT_SUFFIX_SPEC

/* See mips/wrs-linux.h for details on this use of
   STARTFILE_PREFIX_SPEC.  */
#undef STARTFILE_PREFIX_SPEC
#define STARTFILE_PREFIX_SPEC				\
  "%{m64: /usr/local/lib64/ /lib64/ /usr/lib64/}	\
   %{!m64: /usr/local/lib/ /lib/ /usr/lib/}"
