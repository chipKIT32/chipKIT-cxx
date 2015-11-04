/* Compact EH unwinding support for MIPS.
   Copyright (C) 2012  Free Software Foundation, Inc.

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

#ifdef MD_HAVE_COMPACT_EH

#define DWARF_SP_REGNO 29

#if _MIPS_SIM == _ABIO32
#define MIPS_EH_STACK_ALIGN 8
#else
#define MIPS_EH_STACK_ALIGN 16
#endif

#define VRF_0 32

static int
record_push (_Unwind_FrameState *fs, int reg, int offset)
{
  int idx = DWARF_REG_TO_UNWIND_COLUMN (reg);

  offset -= dwarf_reg_size_table[idx];
  fs->regs.reg[idx].how = REG_SAVED_OFFSET;
  fs->regs.reg[idx].loc.offset = offset;
  return offset;
}

static _Unwind_Reason_Code
md_unwind_compact (struct _Unwind_Context *context ATTRIBUTE_UNUSED, _Unwind_FrameState *fs,
		   const unsigned char **pp)
{
  unsigned char op;
  _uleb128_t val;
  int push_offset;
  int i;
  int n;
  const unsigned char *p = *pp;

  push_offset = 0;
  fs->regs.cfa_how = CFA_REG_OFFSET;
  fs->regs.cfa_reg = STACK_POINTER_REGNUM;
  fs->regs.cfa_offset = 0;
  fs->retaddr_column = 31;

  while (1)
    {
      op = *(p++);
      if (op < 0x40)
	{
	  /* Increment stack pointer.  */
	  fs->regs.cfa_offset += (op + 1) * MIPS_EH_STACK_ALIGN;
	}
      else if (op < 0x48)
	{
	  /* Push VR[16] to VR[16+x] and VR[31] */
	  push_offset = record_push (fs, 31, push_offset);
	  for (i = op & 7; i >= 0; i--)
	    push_offset = record_push (fs, 16 + i, push_offset);
	}
      else if (op < 0x50)
	{
	  /* Push VR[16] to VR[16+x], VR[30] and VR[31] */
	  push_offset = record_push (fs, 31, push_offset);
	  push_offset = record_push (fs, 30, push_offset);
	  for (i = op & 7; i >= 0; i--)
	    push_offset = record_push (fs, 16 + i, push_offset);
	}
      else if (op < 0x58)
	{
	  /* Restore stack ponter from frame pointer */
	  fs->regs.cfa_reg = (op & 7) + 16;
	  fs->regs.cfa_offset = 0;
	}
      else if (op == 0x58)
       	{
	  /* Large SP increment.  */
	  p = read_uleb128 (p, &val);
	  fs->regs.cfa_offset += (val + 129) * MIPS_EH_STACK_ALIGN;
	}
      else if (op == 0x59)
	{
	  /* Push VR[x] to VR[x+y] */
	  op = *(p++);
	  n = op >> 3;
	  for (i = op & 7; i >= 0; i--)
	    push_offset = record_push (fs, n + i, push_offset);
	}
      else if (op == 0x5a)
	{
	  /* Push VRF[x] to VRF[x+y] */
	  op = *(p++);
	  n = (op >> 3) + VRF_0;
	  for (i = op & 7; i >= 0; i--)
	    push_offset = record_push (fs, n + i, push_offset);
	}
      else if (op == 0x5c)
	{
	  /* Finish.  */
	  *pp = p;
	  return _URC_NO_REASON;
	}
      else if (op == 0x5d)
	{
	  /* No unwind.  */
	  return _URC_END_OF_STACK;
	}
      else if (op == 0x5e)
	{
	  /* Restore SP from VR[30] */
	  fs->regs.cfa_reg = 30;
	  fs->regs.cfa_offset = 0;
	}
      else if (op == 0x5f)
	{
	  /* NOP */
	}
      else if (op >= 0x60 && op < 0x6c)
	{
	  /* Push VRF[20] to VRF[20 + x] */
	  for (i = op & 0xf; i >= 0; i--)
	    push_offset = record_push (fs, VRF_0 + 20 + i, push_offset);
	}
      else if (op >= 0x6c && op < 0x70)
	{
	  /* MIPS16 push VR[16], VR[17], VR[18+x]-VR[23], VR[31] */
	  push_offset = record_push (fs, 31, push_offset);
	  for (i = 23; i >= 18 + (op & 3); i--)
	    push_offset = record_push (fs, i, push_offset);
	  push_offset = record_push (fs, 17, push_offset);
	  push_offset = record_push (fs, 16, push_offset);
	}
      else
	{
	  return _URC_FATAL_PHASE1_ERROR;
	}
    }
}

#endif /* MD_HAVE_COMPACT_EH */
