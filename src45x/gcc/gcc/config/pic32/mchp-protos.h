/* Subroutines used for Microchip PIC32 code generation.
   Copyright (C) 1989, 1990, 1991, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007
   Free Software Foundation, Inc.
   Contributed by J. Grosbach, james.grosbach@microchip.com
   Changes by J. Kajita, jason.kajita@microchip.com and
   G. Loegel, george.loegel@microchip.com

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

#ifndef GCC_MCHP_PROTOS_H
#define GCC_MCHP_PROTOS_H

#include "config/mips/mips-machine-function.h"
/* A function to save or store a register.  The first argument is the
   register and the second is the stack slot.  */
typedef void (*mchp_save_restore_fn) (rtx, rtx);

extern void mchp_handle_config_pragma(struct cpp_reader *);
extern void mchp_handle_interrupt_pragma (struct cpp_reader *pfile ATTRIBUTE_UNUSED);
extern void mchp_file_end (void);
extern tree mchp_interrupt_attribute (tree *decl, tree identifier, tree args,
                                        int flags, bool *no_add_attrs);
extern tree mchp_vector_attribute (tree *decl, tree identifier, tree args,
                                     int flags, bool *no_add_attrs);
extern tree mchp_at_vector_attribute (tree *decl, tree identifier,
                                        tree args, int flags, bool *no_add_attrs);

extern void mchp_set_current_function (tree fndecl);
extern bool mchp_suppress_prologue (void);
extern bool mchp_suppress_epilogue (void);
extern void mchp_expand_prologue_after_save (void);
extern void mchp_expand_prologue_end (const struct mips_frame_info *frame);
extern HOST_WIDE_INT mchp_expand_prologue_saveregs (HOST_WIDE_INT size, HOST_WIDE_INT step1);
extern void mchp_expand_epilogue_restoreregs (HOST_WIDE_INT step1,
                                              HOST_WIDE_INT step2);
extern HOST_WIDE_INT mchp_compute_frame_info (void);
extern int mchp_register_interrupt_context_p (unsigned regno);
extern void
  mchp_output_function_prologue (FILE *file, HOST_WIDE_INT tsize, HOST_WIDE_INT size ATTRIBUTE_UNUSED);
extern void mchp_output_function_end_prologue (FILE *file);

extern void
  mchp_expand_epilogue_restore (HOST_WIDE_INT sp_offset);

extern void mchp_expand_epilogue_return(rtx ret_insn);
extern void mchp_output_function_begin_epilogue(FILE *file ATTRIBUTE_UNUSED);
extern void
  mchp_output_function_epilogue (FILE *file ATTRIBUTE_UNUSED,
                                 HOST_WIDE_INT size ATTRIBUTE_UNUSED);
extern void mchp_target_insert_attributes (tree decl, tree *attr_ptr);

extern bool mchp_function_ok_for_sibcall(void);

extern void mchp_subtarget_override_options1 (void);
extern void mchp_subtarget_override_options2 (void);
extern void mchp_prepare_function_start (tree fndecl);
extern bool mchp_subtarget_mips16_enabled (const_tree decl);

extern int mchp_load_configuration_definition (const char *fname);
extern void mchp_handle_configuration_setting (const char *name, const char *value_name);
extern void
mchp_add_vector_dispatch_entry (const char *target_name, int vector_number);

extern int mchp_check_for_conversion(rtx);
extern const char *mchp_strip_name_encoding (const char *symbol_name);
#endif

