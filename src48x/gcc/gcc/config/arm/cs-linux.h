/* Sourcery CodeBench GNU/Linux Configuration.
   Copyright (C) 2012.
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

/* The setting in linux-elf.h doesn't taking into account that
   --with-cpu changes the effective defaults, at least as far as
   multilibs are concerned, and we end up losing the hard-float
   multilib.  Just clearing it works fine.  */
#undef MULTILIB_DEFAULTS
