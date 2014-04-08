/* Subroutines for the gcc driver.
   Copyright (C) 2008 Free Software Foundation, Inc.

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

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"
#include "version.h"

#define MCHP_DONT_DEFINE_RESOURCES
#include "../../../../c30_resource/src/xc32/resource_info.h"
#undef MCHP_DONT_DEFINE_RESOURCES

#ifndef str
#define xstr(s) str(s)
#define str(s) #s
#endif

const char* mchp_get_small_startfile_isa (char *id, char** matched_id, const char* mchp_resource_file_in)
{
}

