/* Copyright (C) 2012  Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3, or (at your option) any later
version.

GCC is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.
Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_UNWIND_COMPACT_H
#define GCC_UNWIND_COMPACT_H

enum compact_entry_type _Unwind_Search_Compact_eh_hdr (void *,
    const unsigned char *, struct compact_eh_bases *);

_Unwind_Reason_Code __gnu_compact_pr1 (int, _Unwind_Action,
		  _Unwind_Exception_Class, struct _Unwind_Exception *,
		  struct _Unwind_Context *);

_Unwind_Reason_Code __gnu_compact_pr2 (int, _Unwind_Action,
		  _Unwind_Exception_Class, struct _Unwind_Exception *,
		  struct _Unwind_Context *);

_Unwind_Reason_Code __gnu_compact_pr3 (int, _Unwind_Action,
		  _Unwind_Exception_Class, struct _Unwind_Exception *,
		  struct _Unwind_Context *);

#define EH_FRAME_HEADER_VERSION 2
#endif

