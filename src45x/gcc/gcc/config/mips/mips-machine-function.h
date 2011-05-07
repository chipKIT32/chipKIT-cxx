
/* Prototypes of target machine for GNU compiler.  MIPS version.
   Copyright (C) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2001, 2002, 2003, 2004, 2005, 2007, 2008
   Free Software Foundation, Inc.
   Contributed by A. Lichnewsky (lich@inria.inria.fr).
   Changed by Michael Meissner	(meissner@osf.org).
   64-bit r4000 support by Ian Lance Taylor (ian@cygnus.com) and
   Brendan Eich (brendan@microunity.com).

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

#ifndef GCC_MIPS_MACHINE_FUNCTION_H
#define GCC_MIPS_MACHINE_FUNCTION_H

#include "hwint.h"

#undef TRUE
#undef FALSE

#ifdef __cplusplus
  /* Obsolete.  */
# define TRUE true
# define FALSE false
#else /* !__cplusplus */
# undef bool
# undef true
# undef false

# define bool unsigned char
# define true 1
# define false 0

  /* Obsolete.  */
# define TRUE true
# define FALSE false
#endif /* !__cplusplus */

/* Is the current function an interrupt? If so, how is context handled? */
enum mips_function_type_tag { NON_INTERRUPT, SOFTWARE_CONTEXT_SAVE, SRS_CONTEXT_SAVE,
                              AUTO_CONTEXT_SAVE, DEFAULT_CONTEXT_SAVE, UNKNOWN_CONTEXT_SAVE
                            };

/* Information about a function's frame layout.  */
struct mips_frame_info GTY(()) {
  /* The size of the frame in bytes.  */
  HOST_WIDE_INT total_size;

  /* The number of bytes allocated to variables.  */
  HOST_WIDE_INT var_size;

  /* The number of bytes allocated to outgoing function arguments.  */
  HOST_WIDE_INT args_size;

  /* The number of bytes allocated to the .cprestore slot, or 0 if there
     is no such slot.  */
  HOST_WIDE_INT cprestore_size;

  /* Bit X is set if the function saves or restores GPR X.  */
  unsigned int mask;
  unsigned int savedgpr;

  /* Likewise FPR X.  */
  unsigned int fmask;

  /* Likewise doubleword accumulator X ($acX).  */
  unsigned int acc_mask;

  /* The number of GPRs, FPRs, doubleword accumulators and COP0
     registers saved.  */
  unsigned int num_gp;
  unsigned int num_fp;
  unsigned int num_acc;
  unsigned int num_cop0_regs;

  /* The offset of the topmost GPR, FPR, accumulator and COP0-register
     save slots from the top of the frame, or zero if no such slots are
     needed.  */
  HOST_WIDE_INT gp_save_offset;
  HOST_WIDE_INT fp_save_offset;
  HOST_WIDE_INT acc_save_offset;
  HOST_WIDE_INT cop0_save_offset;

  /* Likewise, but giving offsets from the bottom of the frame.  */
  HOST_WIDE_INT gp_sp_offset;
  HOST_WIDE_INT fp_sp_offset;
  HOST_WIDE_INT acc_sp_offset;
  HOST_WIDE_INT cop0_sp_offset;

  /* Similar, but the value passed to _mcount.  */
  HOST_WIDE_INT ra_fp_offset;

  /* The offset of arg_pointer_rtx from frame_pointer_rtx.  */
  HOST_WIDE_INT arg_pointer_offset;

  /* The offset of hard_frame_pointer_rtx from frame_pointer_rtx.  */
  HOST_WIDE_INT hard_frame_pointer_offset;

  /* TODO: remove? */
  /* true if interrupt context is saved */
  bool has_interrupt_context;
  /* true if hi and lo registers are saved */
  bool has_hilo_context;
};

struct GTY(()) machine_function  {
  /* The register returned by mips16_gp_pseudo_reg; see there for details.  */
  rtx mips16_gp_pseudo_rtx;

  /* The number of extra stack bytes taken up by register varargs.
     This area is allocated by the callee at the very top of the frame.  */
  int varargs_size;

  /* The current frame information, calculated by mips_compute_frame_info.  */
  struct mips_frame_info frame;

  /* The register to use as the function's global pointer, or INVALID_REGNUM
     if the function doesn't need one.  */
  unsigned int global_pointer;

  /* How many instructions it takes to load a label into $AT, or 0 if
     this property hasn't yet been calculated.  */
  unsigned int load_label_length;

  /* True if mips_adjust_insn_length should ignore an instruction's
     hazard attribute.  */
  bool ignore_hazard_length_p;

  /* True if the whole function is suitable for .set noreorder and
     .set nomacro.  */
  bool all_noreorder_p;

  /* True if the function has "inflexible" and "flexible" references
     to the global pointer.  See mips_cfun_has_inflexible_gp_ref_p
     and mips_cfun_has_flexible_gp_ref_p for details.  */
  bool has_inflexible_gp_insn_p;
  bool has_flexible_gp_insn_p;

  /* True if the function's prologue must load the global pointer
     value into pic_offset_table_rtx and store the same value in
     the function's cprestore slot (if any).  Even if this value
     is currently false, we may decide to set it to true later;
     see mips_must_initialize_gp_p () for details.  */
  bool must_initialize_gp_p;

  /* True if the current function must restore $gp after any potential
     clobber.  This value is only meaningful during the first post-epilogue
     split_insns pass; see mips_must_initialize_gp_p () for details.  */
  bool must_restore_gp_when_clobbered_p;

  /* True if we have emitted an instruction to initialize
     mips16_gp_pseudo_rtx.  */
  bool initialized_mips16_gp_pseudo_p;

  /* True if this is an interrupt handler.  */
  bool interrupt_handler_p;
  enum mips_function_type_tag current_function_type;
  int interrupt_priority;

  /* True if this is an interrupt handler that uses shadow registers.  */
  bool use_shadow_register_set_p;

  /* True if this is an interrupt handler that should keep interrupts
     masked.  */
  bool keep_interrupts_masked_p;

  /* True if this is an interrupt handler that should use DERET
     instead of ERET.  */
  bool use_debug_exception_return_p;
};


#endif /*  GCC_MIPS_MACHINE_FUNCTION_H */

