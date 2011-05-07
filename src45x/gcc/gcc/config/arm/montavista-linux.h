/* MontaVista GNU/Linux Configuration.
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

/* Add -tarmv6 and -tthumb2 options for convenience in generating multilibs.
*/
#undef CC1_SPEC
#define CC1_SPEC "							\
 %{tarmv6:      -march=armv6 -mfloat-abi=softfp ;			\
   tthumb2:     -mthumb -march=armv7-a -mfloat-abi=softfp ;             \
   thard-float: -march=armv6k -mtune=mpcore -mcpu=mpcore -mfpu=vfp      \
                -mfloat-abi=hard -mtp=cp15 ;                            \
   :            -march=armv5t}"

/* The various C libraries each have their own subdirectory.  */
#undef SYSROOT_SUFFIX_SPEC
#define SYSROOT_SUFFIX_SPEC					\
  "%{tarmv6:/armv6 ;			                        \
     tthumb2:/thumb2 ;                                          \
     thard-float:/hard-float}"
