/* Wind River GNU/Linux Configuration.
   Copyright (C) 2006, 2007, 2008
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

/* Use the ARM926EJ-S by default.  */
#undef SUBTARGET_CPU_DEFAULT
#define SUBTARGET_CPU_DEFAULT TARGET_CPU_arm926ejs

/* Add a -tiwmmxt option for convenience in generating multilibs.
   This option generates big-endian IWMMXT code.  */
#undef CC1_SPEC
#define CC1_SPEC "							\
 %{tarm926ej-s:	-mcpu=arm926ej-s ;					\
   tthumb2-v7-a: %{!mcpu=*:%{!march=*:-mcpu=cortex-a8}}			\
                 %{!marm:%{!mno-thumb:-mthumb}} ;			\
   tthumb2-v7-a-neon: %{!mcpu=*:%{!march=*:-mcpu=cortex-a8}}		\
                      %{!marm:%{!mno-thumb:-mthumb}}			\
                      -mfloat-abi=softfp -mfpu=neon }			\
 %{profile:-p}"

/* Since the ARM926EJ-S is the default processor, we do not need to
   provide an explicit multilib for that processor.  */
#undef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS \
  { "tarm926ej-s" }

/* The GLIBC headers are in /usr/include, relative to the sysroot; the
   uClibc headers are in /uclibc/usr/include.  */
#undef SYSROOT_HEADERS_SUFFIX_SPEC
#define SYSROOT_HEADERS_SUFFIX_SPEC		\
  "%{muclibc:/uclibc}" 

/* The various C libraries each have their own subdirectory.  */
#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC					\
  "%{muclibc:/uclibc}%{tthumb2-v7-a:/thumb2-v7-a ;		\
     tthumb2-v7-a-neon:/thumb2-v7-a-neon}%{!tthumb2*:%{mfloat-abi=softfp:/softfp}}"

