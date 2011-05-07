/* Wind River GNU/Linux Configuration.
   Copyright (C) 2006, 2007
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

/* Override linux64.h to default to O32.  */
#undef DRIVER_SELF_SPECS
#define DRIVER_SELF_SPECS \
  BASE_DRIVER_SELF_SPECS, \
  LINUX_DRIVER_SELF_SPECS \
  " %{!EB:%{!EL:%(endian_spec)}}" \
  " %{!mabi=*: -mabi=32}"

/* We do not need to provide an explicit big-endian multilib.  */
#undef MULTILIB_DEFAULTS
#define MULTILIB_DEFAULTS \
  { "meb", "mabi=32" }

/* The GLIBC headers are in /usr/include, relative to the sysroot; the
   uClibc headers are in /uclibc/usr/include.  */
#undef SYSROOT_HEADERS_SUFFIX_SPEC
#define SYSROOT_HEADERS_SUFFIX_SPEC		\
  "%{muclibc:/uclibc}" 

/* The various C libraries each have their own subdirectory.  */
#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC				\
  "%{muclibc:%{mel:/uclibc/mel ;			\
               :/uclibc} ;				\
     mel:%{mhard-float:/mel/hard-float ;		\
           :/mel} ;					\
     march=octeon:/octeon ;				\
     march=octeon2:/octeon2 ;				\
     march=vr5500:%{mhard-float:/hard-float/vr5500} ;	\
     march=xlp:%{mhard-float:/hard-float/xlp} ;		\
     mhard-float:/hard-float}"

/* MULTILIB_OSDIRNAMES provides directory names used in two ways:
   relative to $target/lib/ in the GCC installation, and relative to
   lib/ and usr/lib/ in a sysroot.  For the latter, we want names such
   as plain ../lib64, but these cannot be used outside the sysroot
   because different multilibs would be mapped to the same directory.
   Directories are searched both with and without the multilib suffix,
   so it suffices if the directory without the suffix is correct
   within the sysroot while the directory with the suffix doesn't
   exist.  We use STARTFILE_PREFIX_SPEC to achieve the desired
   effect.  */
#undef STARTFILE_PREFIX_SPEC
#define STARTFILE_PREFIX_SPEC				\
  "%{mabi=32: /usr/local/lib/ /lib/ /usr/lib/} 		\
   %{mabi=n32: /usr/local/lib32/ /lib32/ /usr/lib32/}	\
   %{mabi=64: /usr/local/lib64/ /lib64/ /usr/lib64/}"
