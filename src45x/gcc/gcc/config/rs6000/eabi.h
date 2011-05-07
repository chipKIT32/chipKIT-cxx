/* Core target definitions for GNU compiler
   for IBM RS/6000 PowerPC targeted to embedded ELF systems.
   Copyright (C) 1995, 1996, 2000, 2003, 2004, 2007 Free Software Foundation, Inc.
   Contributed by Cygnus Support.

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

/* Add -meabi to target flags.  */
#undef TARGET_DEFAULT
#define TARGET_DEFAULT (MASK_POWERPC | MASK_NEW_MNEMONICS | MASK_EABI)

#undef TARGET_VERSION
#define TARGET_VERSION fprintf (stderr, " (PowerPC Embedded)");

#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()          \
  do                                      \
    {                                     \
      builtin_define_std ("PPC");         \
      builtin_define ("__embedded__");    \
      builtin_assert ("system=embedded"); \
      builtin_assert ("cpu=powerpc");     \
      builtin_assert ("machine=powerpc"); \
      TARGET_OS_SYSV_CPP_BUILTINS ();     \
    }                                     \
  while (0)

/* Add -te500v1 and -te500v2 options for convenience in generating
   multilibs.  */
#undef CC1_EXTRA_SPEC
#define CC1_EXTRA_SPEC \
  "%{te500v1: -mcpu=8540 -mfloat-gprs=single -mspe=yes -mabi=spe} " \
  "%{te500v2: -mcpu=8548 -mfloat-gprs=double -mspe=yes -mabi=spe} " \
  "%{te600: -mcpu=7400 -maltivec -mabi=altivec}"		    \
  "%{te500mc: -mcpu=e500mc -maix-struct-return}"

#undef ASM_DEFAULT_SPEC
#define ASM_DEFAULT_SPEC			\
  "%{te500v1:-mppc -mspe -me500 ;		\
     te500v2:-mppc -mspe -me500 ;		\
     te600:-mppc -maltivec ;			\
     te500mc:-mppc -me500mc ;			\
     :-mppc%{m64:64}}"
