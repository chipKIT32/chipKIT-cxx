/* Definitions of target machine for GNU compiler.  dsPIC30.
   Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999
   Free Software Foundation, Inc.

   Contributed by John Elliott (john.elliott@microchip.com)

   This file is part of GNU CC.

   GNU CC is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GNU CC is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GNU CC; see the file COPYING.  If not, write to
   the Free Software Foundation, 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef GCC_PIC30_PROTOS_H
#define GCC_PIC30_PROTOS_H

struct cpp_reader;

extern void pic30_apply_pragmas(tree decl);
extern int pic30_device_has_gie(void);

extern enum machine_mode machine_Pmode;

extern tree pic30_identKeep[2];
extern tree pic30_identSave[2];
extern tree mchp_identError[2];
extern tree pic30_identUnsupported[2];
extern tree pic30_identUnsafe[2];
extern tree pic30_identSecure[2];
extern tree pic30_identBoot[2];
extern tree pic30_identExternal[2];
extern tree pic30_identSpace[2];

extern unsigned int mchp_pragma_align;
extern tree mchp_pragma_section;
extern unsigned int mchp_pragma_keep;
extern unsigned int mchp_pragma_printf_args;
extern unsigned int mchp_pragma_scanf_args;
extern unsigned int mchp_pragma_inline;

int pic30_frame_pointer_needed_p(int size); 
tree pic30_extended_pointer_integer_type(enum machine_mode);
int pic30_md_mustsave(rtx reg);
tree pic30_read_externals(enum pic30_special_trees kind);
tree pic30_write_externals(enum pic30_special_trees kind);

tree pic30_build_variant_type_copy(tree type, int type_quals);

void mchp_handle_required_pragma(struct cpp_reader *);
void mchp_handle_optimize_pragma(struct cpp_reader *);
void pic30_handle_code_pragma(struct cpp_reader *pfile);
void pic30_handle_idata_pragma(struct cpp_reader *pfile);
void pic30_handle_udata_pragma(struct cpp_reader *pfile);
void mchp_handle_config_pragma(struct cpp_reader *pfile);
void mchp_handle_inline_pragma(struct cpp_reader *pfile);

extern const char * pic30_section_name(rtx op);
extern const char *pic30_strip_name_encoding_helper(const char *symbol_name);
extern const char *pic30_strip_name_encoding(const char *symbol_name);
extern bool pic30_check_section_flags(SECTION_FLAGS_INT, SECTION_FLAGS_INT);
extern const char *pic30_strip_name_encoding_helper(const char *symbol_name);
extern bool pic30_legitimate_address_p(enum machine_mode, rtx, bool);
extern enum machine_mode pic30_addr_space_pointer_mode(addr_space_t);
extern enum machine_mode pic30_addr_space_address_mode(addr_space_t);
extern bool pic30_addr_space_valid_pointer_mode(enum machine_mode,addr_space_t);
extern bool pic30_addr_space_legitimate_address_p(enum machine_mode, rtx,
                                                  bool, addr_space_t);
extern rtx pic30_addr_space_legitimize_address(rtx, rtx, enum machine_mode,
                                               addr_space_t);
extern bool pic30_addr_space_subset_p(addr_space_t, addr_space_t);
extern rtx pic30_addr_space_convert(rtx, tree, tree);


extern char *pic30_section_base(rtx x, int operand, rtx *);
extern int pic30_eds_target(void);
extern int pic30_ecore_target(void);
extern int pic30_address_of_external(rtx op0, rtx op1);
extern void pic30_function_pre(FILE *f, tree decl, char *fnname);
extern int pic30_validate_symbolic_address_operand(enum machine_mode, rtx);
extern int pic30_data_space_operand_p(enum machine_mode mode,rtx op,int strict);
extern void pic30_emit_fillupper(tree decl, int set);
extern int pic30_managed_psv;
extern tree pic30_gimplify_va_arg_expr(tree valist, tree type,
                                       gimple_seq pre_p, gimple_seq post_p);
extern int pic30_noreturn_function(tree decl);
extern char *pic30_boot_secure_access(rtx call, int *slot, int *set_psv);
extern rtx pic30_convert_mode(enum machine_mode newmode,
                              enum machine_mode oldmode, rtx x, int unsignedp);
extern int pic30_parse_target_rid(tree qual, int *x);
extern int pic30_non_postinc_operand(rtx op, enum machine_mode mode);
extern int pic30_general_operand(rtx op, enum machine_mode mode);
extern bool pic30_convert_pointer(rtx to, rtx from, int unsignedp);
extern void pic30_layout_decl(tree);
extern void pic30_layout_type(tree);
extern enum machine_mode pic30_pointer_mode(tree,tree);
extern int pic30_mode1MinMax_operand(rtx op, enum machine_mode mode,
                                     int nMin,int nMax);
extern int pic30_mode1i_operand(rtx op, enum machine_mode);
extern rtx pic30_legitimize_address(rtx x, rtx oldx, enum machine_mode mode);
extern rtx pic30_force_reg(enum machine_mode mode, rtx x);
extern int pic30_prog_operand(rtx op, enum machine_mode mode);
extern int pic30_psvpage_operand(rtx op, enum machine_mode mode);
extern int pic30_psv_reg_operand(rtx op, enum machine_mode mode);
extern int pic30_psv_operand(rtx op, enum machine_mode mode);
extern int pic30_ext_operand(rtx op, enum machine_mode mode);
extern int pic30_pmp_operand(rtx op, enum machine_mode mode);
extern int pic30_mem_df_operand(rtx op, enum machine_mode mode);
extern int pic30_mem_eds_operand(rtx op, enum machine_mode mode);
extern int pic30_mem_peds_operand(rtx op, enum machine_mode mode);
extern int pic30_eds_operand(rtx op, enum machine_mode mode);
extern int pic30_valid_operator(rtx op, enum machine_mode mode);
extern int pic30_volatile_operand(rtx opnd, enum machine_mode mode);
extern int pic30_stack_dereference(rtx op, enum machine_mode mode);
extern int pic30_strict_register_operand(rtx op, enum machine_mode mode);
extern int pic30_register_operand(rtx op, enum machine_mode mode);
extern int pic30_awb_operand(rtx op, enum machine_mode mode);
extern int pic30_mac_input_operand(rtx op, enum machine_mode mode);
extern int pic30_xprefetch_operand(rtx op, enum machine_mode mode);
extern int pic30_yprefetch_operand(rtx op, enum machine_mode mode);
extern int pic30_accumulator_operand(rtx op, enum machine_mode mode);
extern int pic30_immediate_1bit_operand(rtx op, enum machine_mode mode);
extern int pic30_invalid_address_operand(rtx op, enum machine_mode mode);
extern const char *pic30_set_constant_section(const char *name,
                                              SECTION_FLAGS_INT flags,
                                              tree decl);
extern const char *pic30_get_constant_section(void);
extern int pic30_asm_function_p(int);
extern int pic30_interrupt_function_p(tree);
extern int pic30_dat_alignment(tree, int);
extern void mchp_cache_conversion_state(rtx, tree);
extern int pic30_check_for_conversion(rtx);
extern const char *pic30_strip_name_encoding(const char *);
extern int pic30_pp_modify_valid(rtx);
extern void pic30_override_options(void);
extern void pic30_override_options_after_change(void);
extern void pic30_sdb_end_prologue(unsigned int);
extern void pic30_start_source_file(unsigned int, const char *);
extern void pic30_initial_elimination_offset(int, int, HOST_WIDE_INT *);
extern char *pic30_text_section_asm_op(void);
extern char *pic30_data_section_asm_op(void);
extern char *pic30_const_section_asm_op(void);
extern char *pic30_interrupt_preprologue(void);
extern void pic30_handle_interrupt_pragma(struct cpp_reader *);
extern void pic30_handle_code_pragma(struct cpp_reader *);
extern void pic30_handle_idata_pragma(struct cpp_reader *);
extern void pic30_handle_udata_pragma(struct cpp_reader *);
extern void pic30_handle_large_arrays_pragma(struct cpp_reader *);
extern void mchp_handle_align_pragma(struct cpp_reader *);
extern void mchp_handle_section_pragma(struct cpp_reader *);
extern void mchp_handle_printf_args_pragma(struct cpp_reader *);
extern void mchp_handle_scanf_args_pragma(struct cpp_reader *);
extern void mchp_handle_keep_pragma(struct cpp_reader *);
extern int pic30_hard_regno_mode_ok(int, enum machine_mode);
extern void pic30_expand_prologue(void);
extern void pic30_expand_epilogue(void);
extern int pic30_null_epilogue_p(void);
extern int pic30_which_bit(int);
extern int pic30_one_bit_set_p(int);
extern int pic30_libcall(const char *);
extern char *pic30_text_section_asm_op(void);
extern char *pic30_data_section_asm_op(void);
extern char *pic30_ndata_section_asm_op(void);
extern char *pic30_const_section_asm_op(void);
extern char *pic30_dconst_section_asm_op(void);
extern char *pic30_ndconst_section_asm_op(void);
extern void pic30_asm_file_start(void);
extern void pic30_asm_output_ascii(FILE *, char *, int);
extern void pic30_asm_file_end(void);
extern void pic30_asm_output_common(FILE *, char *, int, int);
extern void pic30_asm_output_aligned_common(FILE *, tree, char *, int, int);
extern void pic30_asm_output_local(FILE *, char *, int, int);
extern void pic30_asm_output_aligned_decl_local(FILE *, tree, char *, int, int);
extern void pic30_asm_output_align(FILE *file, int log);
extern void pic30_function_profiler(FILE *,int);

extern void pic30_set_function_return(int bState);
extern int pic30_get_function_return(void);
extern void pic30_identify_used_regs(rtx, int*);
extern int pic30_trace_all_addresses(void);


#ifdef TREE_CODE
#if 0
extern int pic30_codespace_decl_p(tree);
#endif
extern int pic30_far_decl_p(tree);
extern int pic30_near_decl_p(tree);
extern void pic30_insert_attributes(tree, tree *, tree *);
extern void pic30_encode_section_info(tree, rtx, int);
#if 0
extern void pic30_asm_output_section_name(FILE *, tree, char *, int);
#endif
extern void pic30_asm_declare_function_name(FILE *, char *, tree);
extern void pic30_asm_declare_object_name(FILE *, char *, tree);
extern section *pic30_select_section(tree, int, unsigned HOST_WIDE_INT);
extern int pic30_shadow_function_p(tree);
extern int pic30_data_alignment(tree type, int align);
#endif /* TREE_CODE */


#if defined(RTX_CODE) && defined(TREE_CODE)
extern struct rtx_def *
pic30_function_arg(CUMULATIVE_ARGS *, enum machine_mode, tree, int);
extern void
pic30_function_arg_advance(CUMULATIVE_ARGS *,enum machine_mode,tree,int);
extern void
pic30_init_cumulative_args(CUMULATIVE_ARGS *,tree,rtx);
extern rtx
pic30_expand_builtin PARAMS ((tree, rtx, rtx, enum machine_mode, int));
#endif /* TREE_CODE and RTX_CODE*/


#ifdef RTX_CODE
extern int  pic30_dead_or_set_p(rtx first, rtx reg);
extern void  pic30_final_prescan_insn(rtx, rtx*, int);
extern int  pic30_emit_move_sequence(rtx *, enum machine_mode);
extern void  pic30_print_operand(FILE *, rtx, int);
extern void  pic30_print_operand_address(FILE *, rtx);
extern void  pic30_notice_update_cc(rtx, rtx);
extern int   pic30_check_legit_addr(enum machine_mode, rtx, int);
extern struct rtx_def * pic30_return_addr_rtx(int, rtx);
extern char *  pic30_conditional_branch(enum rtx_code, rtx);
extern const char * pic30_compare(rtx *);
extern void  pic30_asm_output_short(FILE *, rtx);
extern int  pic30_I_operand(rtx, enum machine_mode);
extern int  pic30_O_operand(rtx, enum machine_mode);
extern int  pic30_P_operand(rtx, enum machine_mode);
extern int  pic30_q_constraint(rtx);
extern int  pic30_Q_constraint(rtx);
extern int  pic30_Q_operand(rtx, enum machine_mode);
extern int  pic30_R_constraint(rtx);
extern int  pic30_R_operand(rtx, enum machine_mode);
extern int  pic30_S_constraint(rtx);
extern int  pic30_S_constraint_ecore(rtx, int ecore_okay);
extern int  pic30_S_operand(rtx, enum machine_mode);
extern int  pic30_T_constraint(rtx);
extern int  pic30_UT_operand(rtx, enum machine_mode);
extern int  pic30_T_operand(rtx, enum machine_mode);
extern int  pic30_U_constraint(rtx, enum machine_mode);
extern int  pic30_J_operand(rtx, enum machine_mode);
extern int  pic30_OJM_operand(rtx, enum machine_mode);
extern int  pic30_JM_operand(rtx, enum machine_mode);
extern int  pic30_JN_operand(rtx, enum machine_mode);
extern int  pic30_PN_operand(rtx, enum machine_mode);
extern int  pic30_M_operand(rtx, enum machine_mode);
extern int  pic30_rR_or_zero_operand(rtx, enum machine_mode);
extern int  pic30_rR_or_JN_operand(rtx, enum machine_mode);
extern int  pic30_rR_or_JN_APSV_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_P_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_R_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_R_APSV_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_zero_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_imm_operand(rtx, enum machine_mode);
extern int  pic30_valid_call_address_operand(rtx, enum machine_mode);
extern int  pic30_call_address_operand(rtx, enum machine_mode);
extern int  pic30_symbolic_address_operand(rtx, enum machine_mode);
extern int  pic30_imm8_operand(rtx, enum machine_mode);
extern int  pic30_imm2to15_operand(rtx, enum machine_mode);
extern int  pic30_imm16plus_operand(rtx, enum machine_mode);
extern int  pic30_inc_imm_operand(rtx, enum machine_mode);
extern int  pic30_mode1_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode1_operand(rtx, enum machine_mode);
extern int  pic30_mode1J_operand(rtx, enum machine_mode);
extern int  pic30_mode1J_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode1JN_operand(rtx, enum machine_mode);
extern int  pic30_mode1JN_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode1P_operand(rtx, enum machine_mode);
extern int  pic30_mode1P_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode1PN_operand(rtx, enum machine_mode);
extern int  pic30_mode1PN_APSV_operand(rtx, enum machine_mode);
extern int  pic30_near_mode1PN_operand(rtx, enum machine_mode);
extern int  pic30_near_mode1PN_APSV_operand(rtx, enum machine_mode);
extern int  pic30_indirect_mem_operand(rtx, enum machine_mode);
extern int  pic30_APSV_indirect_mem_operand(rtx, enum machine_mode);
extern int  pic30_indirect_mem_operand_modify(rtx,enum machine_mode);
extern int  pic30_APSV_indirect_mem_operand_modify(rtx,enum machine_mode);
extern int  pic30_mode2_operand(rtx, enum machine_mode);
extern int  pic30_D_mode2_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode2_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode2_or_near_operand(rtx, enum machine_mode);
extern int  pic30_mode2res_operand(rtx, enum machine_mode);
extern int  pic30_mode2mres_operand(rtx, enum machine_mode);
extern int  pic30_mode2mres_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode2_or_P_operand(rtx, enum machine_mode);
extern int  pic30_mode2_or_P_APSV_operand(rtx, enum machine_mode);
extern int  pic30_mode3_operand_helper(rtx, enum machine_mode);
extern int  pic30_mode3_operand(rtx, enum machine_mode);
extern int  pic30_modek_possible_operand(rtx, enum machine_mode);
extern int  pic30_modek_operand(rtx, enum machine_mode);
extern int  pic30_math_operand(rtx, enum machine_mode);
extern int  pic30_math_APSV_operand(rtx, enum machine_mode);
extern int  pic30_moveb_operand(rtx, enum machine_mode);
extern int  pic30_moveb_APSV_operand(rtx, enum machine_mode);
extern int  pic30_move_operand(rtx, enum machine_mode);
extern int  pic30_move_APSV_operand(rtx, enum machine_mode);
extern int  pic30_move2_operand(rtx, enum machine_mode);
extern int  pic30_move2_APSV_operand(rtx, enum machine_mode);
extern int  pic30_code_operand(rtx, enum machine_mode);
extern int  pic30_data_operand(rtx, enum machine_mode);
extern int  pic30_rR_or_near_operand(rtx, enum machine_mode);
extern int  pic30_rR_or_near_APSV_operand(rtx, enum machine_mode);
extern int  pic30_reg_imm_or_near_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_code_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_near_operand(rtx, enum machine_mode);
extern int  pic30_wreg_operand(rtx, enum machine_mode);
extern int  pic30_wreg_or_near_operand(rtx, enum machine_mode);
extern int  pic30_breg_operand(rtx, enum machine_mode);
extern int  pic30_creg_operand(rtx, enum machine_mode);
extern int  pic30_ereg_operand(rtx, enum machine_mode);
extern int  pic30_sfr_operand(rtx, enum machine_mode);
extern int  pic30_near_operand(rtx, enum machine_mode);
extern int  pic30_near_math_operand(rtx, enum machine_mode);
extern int  pic30_near_mode2_operand(rtx, enum machine_mode);
extern int  pic30_near_mode2_APSV_operand(rtx, enum machine_mode);
extern int  pic30_reg_or_symbolic_address(rtx, enum machine_mode);
extern int  pic30_neardata_space_operand_p(rtx);
extern int  pic30_IndexEqual(rtx, rtx);
extern int  pic30_Q_base(rtx);
extern int  pic30_Q_displacement(rtx);
extern int  pic30_registerpairs_p(rtx, rtx, rtx, rtx);
extern int  pic30_near_function_p(rtx);
extern int  pic30_shadow_operand_p(rtx);
extern int  pic30_has_space_operand_p(rtx, char *);
extern rtx  pic30_program_space_operand_p(rtx);
extern enum reg_class pic30_preferred_reload_class(rtx, enum reg_class);
extern enum reg_class pic30_secondary_reload_class(enum reg_class,
       enum machine_mode, rtx);
extern void pic30_asm_output_case_label(FILE *, char *, int, rtx);
extern void pic30_asm_output_case_end(FILE *, int, rtx);

#endif /* RTX_CODE */

extern void pic30_common_override_options(void);

struct isr_info {
  const char *id;
  int mask;
};

extern struct isr_info *valid_isr_names;
extern int valid_isr_names_cnt;

extern tree lTreeInterrupt;
extern tree lTreeShadow;
extern tree lTreeTextScnName;
extern tree lTreeIDataScnName;
extern tree lTreeUDataScnName;

extern char *pic30_target_cpu_id;
extern int pic30_smart_io_warning;
extern int pic30_fillupper_value;
extern int pic30_device_id;
extern int pic30_device_mask;


#ifdef GCC_C_PRAGMA_H
#endif

#endif /* ! GCC_PIC30_PROTOS_H */
