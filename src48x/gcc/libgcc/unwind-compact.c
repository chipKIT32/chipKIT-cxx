/* Copyright (C) 2012  Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GCC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

#include "tconfig.h"
#include "tsystem.h"
#include "coretypes.h"
#include "tm.h"
#include "dwarf2.h"
#include "unwind.h"
#define NO_BASE_OF_ENCODED_VALUE
#include "unwind-pe.h"
#include "unwind-dw2-fde.h"
#include "unwind-compact.h"

#ifdef MD_HAVE_COMPACT_EH
/* Search a compact (v2) eh_frame_hdr table.  */

struct compact_eh_index {
    sword fn;
    sword data;
} __attribute__((packed, aligned(4)));

static _Unwind_Ptr
decode_index_ptr (const sword *p)
{
  return (*p & ~1) + (_Unwind_Ptr)p;
}

enum compact_entry_type
_Unwind_Search_Compact_eh_hdr (void *pc, const unsigned char *hdr,
    struct compact_eh_bases *bases)
{
  int lo, hi, mid;
  _Unwind_Ptr p;
  _Unwind_Ptr nrec;
  const struct compact_eh_index *ind;

  bases->eh_encoding = hdr[1];
  read_encoded_value_with_base (DW_EH_PE_absptr | DW_EH_PE_udata4, 0,
				hdr + 4, &nrec);
  if (nrec == 0)
    return CET_not_found;

  ind = (const struct compact_eh_index *)(hdr + 8);

  lo = 0;
  hi = nrec - 1;
  while (1)
    {
      mid = (lo + hi) / 2;
      p = decode_index_ptr (&ind[mid].fn);
      if ((_Unwind_Ptr)pc < p)
	{
	  if (mid == 0)
	    return CET_not_found;
	  hi = mid - 1;
	}
      else
	{
	  if (mid == hi)
	    break;
	  p = decode_index_ptr (&ind[mid + 1].fn);
	  if ((_Unwind_Ptr)pc < p)
	    break;
	  lo = mid + 1;
	}
    }

  p = decode_index_ptr (&ind[mid].fn);
  bases->func = (void *)p;
  if ((ind[mid].fn & 1) == 0)
    {
      bases->entry = &ind[mid].data;
      /* If we hit the terminating CANTUNWIND entry then assume
         we're looking in the wrong table.  */
      if (mid == (int)nrec - 1 && ind[mid].data == 0x015d5d01)
	return CET_not_found;

      return CET_inline;
    }
  else
    {
      p = decode_index_ptr (&ind[mid].data);
      bases->entry = (const void *)p;
      if (ind[mid].data & 1)
	return CET_FDE;
      else
	return CET_outline;
    }
}

#endif /* MD_HAVE_COMPACT_EH */
