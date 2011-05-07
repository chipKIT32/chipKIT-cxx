/* Configuration file for ARM nucleus targets.
   Copyright (C) 2010
   Free Software Foundation, Inc.
   Contributed by Nathan Sidwell (nathan@codesourcery.com)

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#if NUCLEUS_SHARED_EXEC
/* Executables are really shared objects, so default to building a .so
 */
#undef LINK_SPEC
#define LINK_SPEC "%{!static:%{!shared:-shared -z defs}} " BPABI_LINK_SPEC
#undef STARTFILE_SPEC
#define STARTFILE_SPEC "crti%O%s %{!static:crtbeginS%O%s;:crtbegin%O%s}"
#undef ENDFILE_SPEC
#define ENDFILE_SPEC "%{!static:crtendS%O%s;:crtend%O%s} crtn%O%s"
#define NUCLEUS_SHARED_CPP_BUILTINS()		\
  do						\
    {						\
      builtin_define_std ("shared");		\
    }						\
  while (0)
#else
#define NUCLEUS_SHARED_CPP_BUILTINS() do {} while (0)
#endif
/* On nucleaus EABI, we want both the BPABI builtins and a
   nucleaus builtin.  */
#undef  TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS() 		\
  do 						\
    {						\
      TARGET_BPABI_CPP_BUILTINS();		\
      builtin_define_std ("nucleus");		\
      NUCLEUS_SHARED_CPP_BUILTINS();		\
    }						\
  while (0)

/* wide chars are 16 bit */
#undef WCHAR_TYPE
#define WCHAR_TYPE "short int"
#define WCHAR_TYPE_SIZE 16

/* Use --as-needed -lgcc_s for eh support.  */
#ifdef HAVE_LD_AS_NEEDED
#define USE_LD_AS_NEEDED 1
#endif
