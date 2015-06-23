/* Copyright (C) 1997-2013 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#include "version.h"

/* This is the location of the online document giving instructions for
   reporting bugs.  If you distribute a modified version of GCC,
   please configure with --with-bugurl pointing to a document giving
   instructions for reporting bugs to you, not us.  (You are of course
   welcome to forward us bugs reported to you, if you determine that
   they are not bugs in your modifications.)  */
#ifdef BUGURL
const char bug_report_url[] = BUGURL;
#else
const char bug_report_url[] = "<URL:http://www.microchip.com/support>";
#endif

/* The complete version string, assembled from several pieces.
   BASEVER, DATESTAMP, DEVPHASE, and REVISION are defined by the
   Makefile.  */
#define _BUILD_C32_
#define _BUILD_MCHP_
#ifdef _BUILD_C32_
const char version_string[] = BASEVER " " "MPLAB XC32 Compiler v1.40"; /* XC32 */

#elif defined(_BUILD_C30_)
#define version2(X) #X
#define version(X) version2(X)

char *version_string = BASEVER " " "(XC16, Microchip " version(MCHP_VERSION)
                       ") Build date: " __DATE__;
#else

const char version_string[] = BASEVER " " "MPLAB XC Compiler v1.10";
#endif
const char pkgversion_string[] = PKGVERSION;
