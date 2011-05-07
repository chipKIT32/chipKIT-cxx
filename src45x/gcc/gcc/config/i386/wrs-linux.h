/* Wind River IA32 GNU/Linux Configuration.
   Copyright (C) 2009
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

#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC			\
  "%{msystem-glibc:/system-glibc}"

#undef SYSROOT_HEADERS_SUFFIX_SPEC
#define SYSROOT_HEADERS_SUFFIX_SPEC SYSROOT_SUFFIX_SPEC

/* See mips/wrs-linux.h for details on this use of
   STARTFILE_PREFIX_SPEC.  */
#undef STARTFILE_PREFIX_SPEC
#define STARTFILE_PREFIX_SPEC				\
  "%{m64: /usr/local/lib64/ /lib64/ /usr/lib64/}	\
   %{!m64: /usr/local/lib/ /lib/ /usr/lib/}"
