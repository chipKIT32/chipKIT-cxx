/* Stack unwinding support through the first exception frame.
   Copyright (C) 2007 Cavium Networks.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING.  If not, write to
the Free Software Foundation, 51 Franklin Street, Fifth Floor,
Boston, MA 02110-1301, USA.  */

#define MD_FALLBACK_FRAME_STATE_FOR octeon_elf_fallback_frame_state

/* Check whether this is the cvmx_interrupt_stage2 frame.  If the
   function call was dispatched via k0 assume we are in
   cvmx_interrupt_stage2.  In this case the sp in point to the saved
   register array.  */

static _Unwind_Reason_Code
octeon_elf_fallback_frame_state (struct _Unwind_Context *context,
				 _Unwind_FrameState *fs)
{
  unsigned i;
  unsigned *pc = context->ra;

  /* Look for "jalr k0".  */
  if (pc[-2] != 0x0340f809)
    return _URC_END_OF_STACK;
  
  for (i = 0; i < 32; i++)
    {
      fs->regs.reg[i].how = REG_SAVED_OFFSET;
      fs->regs.reg[i].loc.offset = 8 * i;
    }

  /* Keep the next frame's sp.  This way we have a CFA that points
     exactly to the register array.  */
  fs->regs.cfa_how = CFA_REG_OFFSET;
  fs->regs.cfa_reg = STACK_POINTER_REGNUM;
  fs->regs.cfa_offset = 0;

  /* DEPC is saved as the 35. register.  */
  fs->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].how = REG_SAVED_OFFSET;
  fs->regs.reg[DWARF_ALT_FRAME_RETURN_COLUMN].loc.offset = 8 * 35;
  fs->retaddr_column = DWARF_ALT_FRAME_RETURN_COLUMN;

  return _URC_NO_REASON;
}
