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
extern void mchp_handle_configset_pragma(struct cpp_reader *pfile, const char *set);
extern void mchp_handle_config_alt_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_bf1_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_abf1_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_bf2_pragma(struct cpp_reader *pfile);
extern void mchp_handle_config_abf2_pragma(struct cpp_reader *pfile);
extern void mchp_init_cci_pragmas(void);
extern void mchp_init_cci(void *);
extern void mchp_handle_vector_pragma(struct cpp_reader *);
extern void mchp_handle_interrupt_pragma (struct cpp_reader *pfile ATTRIBUTE_UNUSED);
extern void mchp_handle_region_pragma (struct cpp_reader *pfile ATTRIBUTE_UNUSED);
extern void mchp_file_end (void);
extern tree mchp_interrupt_attribute (tree *decl, tree identifier, tree args,
                                        int flags, bool *no_add_attrs);
extern tree mchp_vector_attribute (tree *decl, tree identifier, tree args,
                                     int flags, bool *no_add_attrs);
extern tree mchp_at_vector_attribute (tree *decl, tree identifier,
                                        tree args, int flags, bool *no_add_attrs);
extern tree mchp_address_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_ramfunc_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED);
extern tree mchp_naked_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED);
extern tree mchp_unique_section_attribute (tree *node, tree name ATTRIBUTE_UNUSED,
                          tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                          bool *no_add_attrs);
extern tree mchp_nomicromips_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED);
extern tree mchp_space_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_region_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern void pic32_update_external_memory_info(const char* region_name, 
                            unsigned int region_origin, int region_size);                            
extern tree mchp_unsupported_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_target_error_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_keep_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_coherent_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_persistent_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_crypto_attribute(tree *decl, tree identifier,
                            tree args, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs);
extern tree mchp_shared_attribute(tree *node, tree identifier ATTRIBUTE_UNUSED,
                           tree args, int flags ATTRIBUTE_UNUSED,
                           bool *no_add_attrs);
extern tree mchp_nofpu_attribute (tree *decl, tree identifier, tree args,
                                        int flags, bool *no_add_attrs);
extern tree mchp_frp_attribute(tree *decl, tree identifier ATTRIBUTE_UNUSED,
                            tree args ATTRIBUTE_UNUSED, int flags ATTRIBUTE_UNUSED,
                            bool *no_add_attrs ATTRIBUTE_UNUSED);
extern int mchp_function_nofpu_p (tree type);
extern void mchp_set_current_function (tree fndecl);
extern bool mchp_suppress_prologue (void);
extern bool mchp_suppress_epilogue (void);
extern bool mchp_function_profiling_epilogue (bool usereturn);
extern void mchp_expand_prologue_after_save (void);
extern void mchp_expand_prologue_end (const struct mips_frame_info *frame);

extern HOST_WIDE_INT mchp_expand_prologue_saveregs (HOST_WIDE_INT size, HOST_WIDE_INT step1);
extern void mchp_expand_epilogue_restoreregs (HOST_WIDE_INT step1,
                                              HOST_WIDE_INT step2);
extern HOST_WIDE_INT mchp_compute_frame_info (void);
extern int mchp_register_interrupt_context_p (unsigned regno);

extern bool mchp_subtarget_save_reg_p (unsigned int regno);
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
extern void mchp_subtarget_override_options(void);
extern void mchp_subtarget_override_options1 (void);
extern void mchp_subtarget_override_options2 (void);
extern void mchp_override_options_after_change(void);
extern void mchp_prepare_function_start (tree fndecl);
extern bool mchp_subtarget_mips16_enabled (const_tree decl);
extern bool mchp_subtarget_micromips_enabled (const_tree decl);
extern bool mchp_subtarget_mips32_enabled ();
extern void mchp_subtarget_encode_section_info (tree decl, rtx rtl, int first_seen ATTRIBUTE_UNUSED);

extern void mchp_apply_pragmas(tree decl);
extern void mchp_handle_align_pragma(struct cpp_reader *);
extern void mchp_handle_section_pragma(struct cpp_reader *);
extern void mchp_handle_printf_args_pragma(struct cpp_reader *);
extern void mchp_handle_scanf_args_pragma(struct cpp_reader *);
extern void mchp_handle_inline_pragma(struct cpp_reader *);
extern void mchp_handle_keep_pragma(struct cpp_reader *);
extern void mchp_handle_coherent_pragma(struct cpp_reader *);
extern void mchp_handle_required_pragma(struct cpp_reader *);
extern void mchp_handle_optimize_pragma(struct cpp_reader *pfile);

extern unsigned int mchp_pragma_align;
extern tree mchp_pragma_section;
extern unsigned int mchp_pragma_inline;
extern unsigned int mchp_pragma_keep;
extern unsigned int mchp_pragma_coherent;
extern unsigned int mchp_pragma_printf_args;
extern unsigned int mchp_pragma_scanf_args;

extern void mchp_asm_named_section(const char *pszSectionName,
                            SECTION_FLAGS_INT flags,
                            tree decl ATTRIBUTE_UNUSED);
extern section *mchp_select_section (tree decl, int reloc,
                     unsigned HOST_WIDE_INT align ATTRIBUTE_UNUSED);
extern SECTION_FLAGS_INT mchp_section_type_flags(tree decl, const char *name,
                        int reloc ATTRIBUTE_UNUSED);
extern int set_section_stack(const char *pszSectionName, SECTION_FLAGS_INT pszSectionFlag);
extern const char *mchp_strip_name_encoding (const char *symbol_name);

extern void mchp_init_cci_builtins(void);

extern bool mips_far_type_p (const_tree type);

extern const char * mchp_data_section_asm_op(void);
extern const char * mchp_bss_section_asm_op(void);
extern const char * mchp_sbss_section_asm_op(void);
extern const char * mchp_sdata_section_asm_op(void);
extern const char * mchp_text_section_asm_op(void);
extern const char * mchp_rdata_section_asm_op(void);
extern void mchp_cache_conversion_state(rtx, tree);
extern rtx  pic32_expand_software_reset_libcall(void);

#endif
