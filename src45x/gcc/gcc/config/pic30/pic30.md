;;- Machine description for GNU compiler
;;- Microchip dsPIC30 version.
;; Copyright (C) 1994, 1995, 1996, 1997 Free Software Foundation, Inc.

;; Contributed by John Elliott (john.elliott@microchip.com)

;; This file is part of GNU CC.

;; GNU CC is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation; either version 1, or (at your option)
;; any later version.

;; GNU CC is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GNU CC; see the file COPYING.  If not, write to
;; the Free Software Foundation, 59 Temple Place - Suite 330,
;; Boston, MA 02111-1307, USA.

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	This is the machine description for the Microchip dsPIC.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;;- instruction definitions
;;
;;- See file "rtl.def" for documentation on define_insn, match_*, et. al.
;;
;; dsPIC30 specific remarks:
;;
;; 1) BITS_PER_UNIT = 8
;;
;; 2) GCC to dsPIC30 data type mappings:
;;    QImode => char (8 bits or 1 reg).
;;    HImode => short/int (16 bits or 1 reg).
;;    SImode => long (32 bits or 2 regs).
;;    DImode => long long (64 bits or 4 regs).
;;    SFmode => single precision float (32 bits or 2 regs).
;;    DFmode => double precision float (64 bits or 4 regs).
;; 
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Condition code settings.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; clobber - value of cc is unknown
;; unchanged - insn does not affect cc
;; set - set to agree with the results of the operation
;; change0 - insn does not affect cc but it does modify operand 0
;;	cc only changed if the item previously set into the condition code
;;	has been modified.
;; math - value of cc has incorrect C and OV flags
;; move - value of cc has incorrect C and OV flags
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_attr "cc" "clobber,unchanged,set,change0,math,move"
  (const_string "clobber"))
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; machine specific PREDICATE_CODES

;; define_predicate automatically includes the equivalent of
;;    (ior (match_test "GET_MODE(op) == mode")
;;         (macch_test "GET_MODE(op) == VOIDmode"))
;;
;; so no need to do this.  If you don't want it, use define_special_predicate
;;

;;  {"pic30_general_operand", {CONST_INT, CONST_DOUBLE, CONST, SYMBOL_REF, 
;;                             LABEL_REF, SUBREG, REG, MEM }}, 

;; 
;; pic30_general_operand does not accept any for the funky C30 pointer modes
;;
(define_predicate "pic30_general_operand"
  (match_operand 0 "general_operand")
{
  if (GET_CODE(op) == MEM) {
    rtx inner = XEXP(op,0);

    if ((GET_MODE(inner) == P24PROGmode) || (GET_MODE(inner) == P24PSVmode) ||
        (GET_MODE(inner) == P16PMPmode) || (GET_MODE(inner) == P32EXTmode) ||
        (GET_MODE(inner) == P32EDSmode) || (GET_MODE(inner) == P32PEDSmode)) {
      /* a de-reference of an extended pointer - not general */
      return 0;
    }
#if 1
  } else if (REG_P(op) && (REGNO(op) < FIRST_PSEUDO_REGISTER)) {
    /* general regs doesn't check the regclass - which is really annoying
       because 'g' does... */
    return reg_fits_class_p(op, GENERAL_REGS, 0, GET_MODE(op));
#endif
  }
  return 1;
})

(define_predicate "pic30_pushhi_operand"
  (ior (match_code "reg")
       (match_test "pic30_T_constraint(op)")
       (and (match_code "mem")
            (match_test "GET_CODE(XEXP(op,0)) == REG")
            (match_test "GET_MODE(XEXP(op,0)) == machine_Pmode"))
       (and (match_code "const_int")
            (match_test "(INTVAL(op) == 0)"))
       (match_code "post_inc")
       (match_code "pre_inc")))

;;  { "pic30_psvapge_operand", { REG }}, 

(define_special_predicate "pic30_psvpage_operand"
  (and (match_code "reg")
       (match_test "REGNO(op) == PSVPAG")
       (match_test "GET_MODE(op) == QImode")))

;;  { "pic30_psv_operand", { MEM }}, 

(define_predicate "pic30_prog_operand"
  (and (match_test "pic30_extended_pointer_operand(op, P24PROGmode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_psv_operand"
  (and (match_test "pic30_extended_pointer_operand(op, P24PSVmode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_apsv_operand"
  (and (match_test "pic30_extended_pointer_operand(op, P16APSVmode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_pmp_operand"
  (and (match_test "pic30_extended_pointer_operand(op, P16PMPmode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_ext_operand"
  (and (match_test "pic30_extended_pointer_operand(op, P32EXTmode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_eds_operand"
  (and (match_test "pic30_extended_pointer_operand(op, mode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

(define_predicate "pic30_df_operand"
  (and (match_test "pic30_extended_pointer_operand(op, mode)")
       (match_code "post_inc,pre_dec,pre_inc,post_dec,reg,subreg,plus,symbol_ref,const")))

;;  { "pic30_tbl_operand", { MEM }}, 
;;  unused

;;  { "pic30_register_operand", { REG, SUBREG }}, 

(define_predicate "pic30_register_operand"
  (and (match_code "reg,subreg")
       (match_test "pic30_register_operand_helper(op,mode,parf_subreg_ok)")))

(define_predicate "pic30_D_register_operand"
  (and (match_code "reg,subreg")
       (match_test "pic30_register_operand_helper(op,mode,parf_D_regs_only | parf_subreg_ok)")))

(define_predicate "pic30_strict_register_operand"
  (and (match_code "reg")
       (match_test "pic30_register_operand_helper(op,mode,0)")))

;;  { "pic30_mode1_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_predicate "pic30_mode1_operand"
  (and (match_test "pic30_mode1MinMax_operand(op,mode,-16,15)")
       (match_code "subreg,reg,mem,const_int")))

(define_predicate "pic30_mode1_APSV_operand"
  (and (match_test "pic30_mode1MinMax_APSV_operand(op,mode,-16,15)")
       (match_code "subreg,reg,mem,const_int")))

;;  { "pic30_mode1P_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_predicate "pic30_mode1P_operand"
  (and (match_test "pic30_mode1MinMax_operand(op,mode,0,31)")
       (match_code "subreg,reg,mem,const_int")))

(define_predicate "pic30_mode1P_APSV_operand"
  (and (match_test "pic30_mode1MinMax_APSV_operand(op,mode,0,31)")
       (match_code "subreg,reg,mem,const_int")))

;;  { "pic30_mode1PN_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_predicate "pic30_mode1PN_operand"
  (and (match_test "pic30_mode1MinMax_operand(op,mode,-31,31)")
       (match_code "subreg,reg,mem,const_int")))

(define_predicate "pic30_mode1PN_APSV_operand"
  (and (match_test "pic30_mode1MinMax_APSV_operand(op,mode,-31,31)")
       (match_code "subreg,reg,mem,const_int")))

;;  { "pic30_mode1J_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_predicate "pic30_mode1J_operand"
  (and (match_test "pic30_mode1MinMax_operand(op,mode,0,1023)")
       (match_code "subreg,reg,mem,const_int")))

(define_predicate "pic30_mode1J_APSV_operand"
  (and (match_test "pic30_mode1MinMax_APSV_operand(op,mode,0,1023)")
       (match_code "subreg,reg,mem,const_int")))

;;  { "pic30_mode1JN_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_predicate "pic30_mode1JN_operand"
  (and (match_test "pic30_mode1MinMax_operand(op,mode,-31,1023)")
       (match_code "subreg,reg,mem,const_int")))

(define_predicate "pic30_mode1JN_APSV_operand"
  (and (match_test "pic30_mode1MinMax_APSV_operand(op,mode,-31,1023)")
       (match_code "subreg,reg,mem,const_int")))

;;  { "pic30_indirect_mem_operand", { MEM }}, 

(define_predicate "pic30_indirect_mem_operand_modify"
  (and (match_code "mem")
       (match_code "post_inc,post_dec,pre_inc,pre_dec" "0")
       (match_test "GET_MODE(XEXP(op,0)) == machine_Pmode")))

(define_predicate "pic30_APSV_indirect_mem_operand_modify"
  (and (match_code "mem")
       (match_code "post_inc,post_dec,pre_inc,pre_dec" "0")
       (ior (match_test "GET_MODE(XEXP(op,0)) == machine_Pmode")
            (match_test "GET_MODE(XEXP(op,0)) == P16APSVmode"))))

(define_predicate "pic30_indirect_mem_operand"
  (and (match_code "mem")
       (match_code "post_inc,post_dec,pre_inc,pre_dec,reg" "0")
       (match_test "GET_MODE(XEXP(op,0)) == machine_Pmode")))

(define_predicate "pic30_APSV_indirect_mem_operand"
  (and (match_code "mem")
       (match_code "post_inc,post_dec,pre_inc,pre_dec,reg" "0")
       (ior (match_test "GET_MODE(XEXP(op,0)) == machine_Pmode")
            (match_test "GET_MODE(XEXP(op,0)) == P16APSVmode"))))

;;  { "pic30_mode3_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_mode3_operand"
  (and (match_code "subreg,reg,mem")
       (match_test "pic30_mode3_operand_helper(op,mode)")))

(define_predicate "pic30_mode3_APSV_operand"
  (and (match_code "subreg,reg,mem")
       (match_test "pic30_mode3_APSV_operand_helper(op,mode)")))

;;  { "pic30_modek_operand", { MEM }}, 

(define_predicate "pic30_modek_operand"
  (and (match_code "subreg,mem")
       (match_test "pic30_modek_operand_helper(op,mode)")))

(define_predicate "pic30_modek_APSV_operand"
  (and (match_code "subreg,mem")
       (match_test "pic30_modek_APSV_operand_helper(op,mode)")))

;;  { "pic30_data_operand", { MEM }}, 

(define_predicate "pic30_data_operand"
  (and (match_code "mem")
       (match_test "GET_MODE(op) != QImode")
       (match_test "pic30_T_constraint(op)")))

;;  { "pic30_move2_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_move2_operand"
  (ior (match_operand 0 "pic30_mode3_operand")
       (match_operand 0 "pic30_modek_operand")))

(define_predicate "pic30_move2_APSV_operand"
  (ior (match_operand 0 "pic30_mode3_APSV_operand")
       (match_operand 0 "pic30_modek_APSV_operand")))

;;  { "pic30_mode2_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_mode2_operand"
  (match_code "subreg,reg,mem")
{
  int fMode2Operand;
  enum rtx_code code;

  fMode2Operand = FALSE;
  code = GET_CODE(op);
  switch (code) {
    case SUBREG:
      fMode2Operand = register_operand(op, mode);
      break;
    case REG:
      /*
      ** Register to register
      */
      fMode2Operand = ((GET_MODE(op) == mode) &&
                       ((REGNO(op) <= WR15_REGNO) ||
                        (REGNO(op) >= FIRST_PSEUDO_REGISTER)));
      break;
    case MEM:
      fMode2Operand = pic30_indirect_mem_operand(op,mode);
      break;
    default:
        break;
  }
  return fMode2Operand;
})

(define_predicate "pic30_D_mode2_operand"
  (match_code "subreg,reg,mem")
{
  int fMode2Operand;
  enum rtx_code code;

  fMode2Operand = FALSE;
  code = GET_CODE(op);
  switch (code) {
    case SUBREG:
      fMode2Operand = register_operand(op, mode);
      break;
    case REG:
      /*
      ** Register to register
      */
      fMode2Operand = ((GET_MODE(op) == mode) &&
                       (((REGNO(op) <= WR15_REGNO) &&
                         (REGNO(op) & 1 == 0)) ||
                        (REGNO(op) >= FIRST_PSEUDO_REGISTER)));
      break;
    case MEM:
      fMode2Operand = pic30_indirect_mem_operand(op,mode);
      break;
    default:
        break;
  }
  return fMode2Operand;
})

(define_predicate "pic30_mode2_APSV_operand"
  (match_code "subreg,reg,mem")
{
  int fMode2Operand;
  enum rtx_code code;

  fMode2Operand = FALSE;
  code = GET_CODE(op);
  switch (code) {
    case SUBREG:
      fMode2Operand = register_operand(op, mode);
      break;
    case REG:
      /*
      ** Register to register
      */
      fMode2Operand = ((GET_MODE(op) == mode) &&
                       ((REGNO(op) <= WR15_REGNO) ||
                        (REGNO(op) >= FIRST_PSEUDO_REGISTER)));
      break;
    case MEM:
      fMode2Operand = pic30_indirect_mem_operand(op,mode);
      break;
    default:
        break;
  }
  return fMode2Operand;
})

(define_predicate "pic30_D_mode2_APSV_operand"
  (match_code "subreg,reg,mem")
{
  int fMode2Operand;
  enum rtx_code code;

  fMode2Operand = FALSE;
  code = GET_CODE(op);
  switch (code) {
    case SUBREG:
      fMode2Operand = register_operand(op, mode);
      break;
    case REG:
      /*
      ** Register to register
      */
      fMode2Operand = ((GET_MODE(op) == mode) &&
                       (((REGNO(op) <= WR15_REGNO) &&
                         (REGNO(op) & 1 == 0)) ||
                        (REGNO(op) >= FIRST_PSEUDO_REGISTER)));
      break;
    case MEM:
      fMode2Operand = pic30_indirect_mem_operand(op,mode);
      break;
    default:
        break;
  }
  return fMode2Operand;
})

;;  { "pic30_P_operand", { CONST_INT }}, 

(define_special_predicate "pic30_P_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0")
       (match_test "INTVAL(op) <= 31")))

(define_special_predicate "pic30_N_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) <= 0")
       (match_test "INTVAL(op) >= -31")))

;;  { "pic30_mode2_or_P_operand", {SUBREG, REG, MEM, CONST_INT }}, 

(define_predicate "pic30_mode2_or_P_operand"
  (ior (match_operand 0 "pic30_mode2_operand")
       (match_operand 0 "pic30_P_operand")))

(define_predicate "pic30_mode2_or_P_APSV_operand"
  (ior (match_operand 0 "pic30_mode2_APSV_operand")
       (match_operand 0 "pic30_P_operand")))

;;  { "pic30_immediate_1bit_operand", { CONST_INT }}, 

(define_predicate "pic30_immediate_1bit_operand"
  (and (match_code "const_int")
       (match_test "(INTVAL(op) & 0xFFFF) & ((INTVAL(op) & 0xFFFF) - 1) == 0")))

;;  { "pic30_accumulator_operand", { REG }}, 

(define_predicate "pic30_accumulator_operand"
  (and (match_code "reg")
       (match_test "IS_ACCUM_REG(REGNO(op))")))

;;  { "pic30_awb_operand", { REG }}, 

(define_predicate "pic30_awb_operand"
  (and (match_code "reg")
       (ior (match_test "REGNO(op) >= FIRST_PSEUDO_REGISTER")
            (match_test "IS_AWB_REG(REGNO(op))"))))

;;  { "pic30_mac_input_operand", { REG }}, 

(define_predicate "pic30_mac_input_operand"
  (ior (match_code "const_int")
       (and (match_code "reg")
            (ior (match_test "REGNO(op) >= FIRST_PSEUDO_REGISTER")
                 (match_test "REGNO_REG_CLASS(REGNO(op)) == SINK_REGS")
                 (match_test "IS_PRODUCT_REG(REGNO(op))")))))

;;   { "pic30_xprefetch_operand", { REG }}, 

(define_predicate "pic30_xprefetch_operand"
  (and (match_code "reg")
       (ior (match_test "REGNO(op) >= FIRST_PSEUDO_REGISTER")
            (match_test "REGNO_REG_CLASS(REGNO(op)) == SINK_REGS")
            (match_test "IS_XPREFETCH_REG(REGNO(op))"))))

;;  { "pic30_yprefetch_operand", { REG }}, 

(define_predicate "pic30_yprefetch_operand"
  (and (match_code "reg")
       (ior (match_test "REGNO(op) >= FIRST_PSEUDO_REGISTER")
            (match_test "REGNO_REG_CLASS(REGNO(op)) == SINK_REGS")
            (match_test "IS_YPREFETCH_REG(REGNO(op))"))))

;;  { "pic30_mode2res_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_mode2res_operand"
  (and (match_operand 0 "pic30_mode2_operand")
       (if_then_else (match_code "mem")
                     (not (match_code "pre_inc,post_dec" "0"))
                     (match_test "1"))))

;;  { "pic30_mode2mres_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_mode2mres_operand"
  (and (match_operand 0 "pic30_mode2_operand")
       (if_then_else (match_code "mem")
                     (not (match_code "pre_inc,post_dec" "0"))
                     (match_test "1"))))

(define_predicate "pic30_mode2mres_APSV_operand"
  (and (match_operand 0 "pic30_mode2_APSV_operand")
       (if_then_else (match_code "mem")
                     (not (match_code "pre_inc,post_dec" "0"))
                     (match_test "1"))))

;;  { "pic30_wreg_operand",  { SUBREG, REG }}, 

(define_predicate "pic30_wreg_operand"
  (match_operand 0 "register_operand")
{  rtx inner = op;
 
   if (GET_CODE(inner) == SUBREG) inner = SUBREG_REG(inner);
   return (REG_P(inner) && IS_AREG_OR_PSEUDO_REGNO(inner));
})
   
;;  { "pic30_breg_operand",  { SUBREG, REG }}, 

(define_predicate "pic30_breg_operand"
  (match_operand 0 "register_operand")
{  rtx inner = op;
 
   if (GET_CODE(inner) == SUBREG) inner = SUBREG_REG(inner);
   return (REG_P(inner) && IS_BREG_OR_PSEUDO_REGNO(inner));
})
   
;;  { "pic30_creg_operand", { SUBREG, REG }}, 

(define_predicate "pic30_creg_operand"
  (match_operand 0 "register_operand")
{  rtx inner = op;
 
   if (GET_CODE(inner) == SUBREG) inner = SUBREG_REG(inner);
   return (REG_P(inner) && IS_CREG_OR_PSEUDO_REGNO(inner));
})
   
;;  { "pic30_ereg_operand", { SUBREG, REG }}, 

(define_predicate "pic30_ereg_operand"
  (match_operand 0 "register_operand")
{  rtx inner = op;
 
   if (GET_CODE(inner) == SUBREG) inner = SUBREG_REG(inner);
   return (REG_P(inner) && IS_EREG_OR_PSEUDO_REGNO(inner));
})

;;  { "pic30_near_operand", { MEM }}, 

(define_predicate "pic30_near_operand"
  (match_test "pic30_U_constraint(op,mode)"))

(define_predicate "pic30_sfr_operand"
  (and (match_test "pic30_U_constraint(op,mode)")
       (match_test "pic30_sfr_operand_helper(op)")))

;;  { "pic30_T_operand", { MEM }}, 

(define_predicate "pic30_T_operand"
  (and (match_code "mem")
       (match_test "pic30_T_constraint(op)")))

(define_predicate "pic30_UT_operand"
  (ior (match_operand 0 "pic30_T_operand")
       (match_operand 0 "pic30_near_operand")))

;;  { "pic30_code_operand", { MEM }}, 

(define_predicate "pic30_code_operand"
  (and (match_code "mem")
       (match_test "pic30_program_space_operand_p(XEXP(op,0))")))

;;  { "pic30_reg_or_code_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_reg_or_code_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "pic30_code_operand")))

;;  { "pic30_reg_or_near_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_reg_or_near_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "pic30_near_operand")))

;;  { "pic30_reg_imm_or_near_operand", { SUBREG, REG, MEM, CONST_INT }}, 

(define_predicate "pic30_reg_imm_or_near_operand"
  (ior (match_operand 0 "immediate_operand")
       (match_operand 0 "register_operand")
       (match_operand 0 "pic30_near_operand")))

;;  { "pic30_R_operand", { MEM }},

(define_predicate "pic30_R_operand"
  (match_test "pic30_R_constraint(op)"))

(define_predicate "pic30_R_APSV_operand"
  (match_test "pic30_R_constraint(op)"))

;;  { "pic30_reg_or_R_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_reg_or_R_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "pic30_R_operand")))

(define_predicate "pic30_reg_or_R_APSV_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "pic30_R_APSV_operand")))

;;  { "pic30_rR_or_near_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_rR_or_near_operand"
  (ior (match_operand 0 "pic30_reg_or_R_operand")
       (match_operand 0 "pic30_near_operand")))

(define_predicate "pic30_rR_or_near_APSV_operand"
  (ior (match_operand 0 "pic30_reg_or_R_APSV_operand")
       (match_operand 0 "pic30_near_operand")))

;;  { "pic30_wreg_or_near_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_wreg_or_near_operand"
  (ior (match_operand 0 "pic30_wreg_operand")
       (match_operand 0 "pic30_near_operand")))

;;  { "pic30_reg_or_imm_operand", { SUBREG, REG, CONST_INT }}, 

(define_predicate "pic30_reg_or_imm_operand"
  (ior (match_operand 0 "pic30_register_operand")
       (match_operand 0 "immediate_operand")))

;;  { "pic30_imm2to15_operand", { CONST_INT }}, 

(define_special_predicate "pic30_imm2to15_operand"
  (and (match_operand 0 "immediate_operand")
       (match_test "INTVAL(op) >= 2")
       (match_test "INTVAL(op) <= 15")))

;;  { "pic30_imm8_operand", { CONST_INT }}, 

(define_special_predicate "pic30_imm8_operand"
  (and (match_operand 0 "immediate_operand")
       (match_test "INTVAL(op) == 8")))

;;  { "pic30_imm16plus_operand", { CONST_INT }}, 

(define_special_predicate "pic30_imm16plus_operand"
  (and (match_operand 0 "immediate_operand")
       (match_test "INTVAL(op) >= 16")))

;;  { "pic30_M_operand", { CONST_INT }}, 

(define_special_predicate "pic30_M_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) <= 0")
       (match_test "INTVAL(op) >= -1023")))

;;  { "pic30_J_operand", { CONST_INT }}, 

(define_special_predicate "pic30_J_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= 0")
       (match_test "INTVAL(op) <= 1023")))

(define_special_predicate "pic30_OJM_operand"
  (and (match_code "const_int")
       (match_test "INTVAL(op) >= -511")
       (match_test "INTVAL(op) <= 511")))

;;  { "pic30_JN_operand", { CONST_INT }}, 

(define_special_predicate "pic30_JN_operand"
  (ior (match_operand 0 "pic30_J_operand")
       (match_operand 0 "pic30_N_operand")))

;;  { "pic30_JM_operand", { CONST_INT }}, 

(define_special_predicate "pic30_JM_operand"
  (ior (match_operand 0 "pic30_J_operand")
       (match_operand 0 "pic30_M_operand")))

;;  { "pic30_math_operand", { REG, MEM, SUBREG, CONST_INT }}, 

(define_special_predicate "pic30_math_operand"
  (ior (match_operand 0 "pic30_mode1_operand")
       (match_operand 0 "pic30_JN_operand")))

(define_special_predicate "pic30_JMmath_operand"
  (ior (match_operand 0 "pic30_mode1_operand")
       (match_operand 0 "pic30_JM_operand")))

(define_special_predicate "pic30_math_APSV_operand"
  (ior (match_operand 0 "pic30_mode1_APSV_operand")
       (match_operand 0 "pic30_JN_operand")))

;;  { "pic30_inc_imm_operand", { CONST_INT }}, 

(define_special_predicate "pic30_inc_imm_operand"
  (and (match_operand 0 "immediate_operand")
       (match_test "INTVAL(op) >= -2")
       (match_test "INTVAL(op) <= 2")))
      
;;  { "pic30_near_math_operand", { REG, MEM, SUBREG,  CONST_INT }}, 

(define_special_predicate "pic30_near_math_operand"
  (ior (match_operand 0 "pic30_math_operand")
       (match_operand 0 "pic30_wreg_or_near_operand")))

;;  { "pic30_near_mode2_operand", { REG, MEM, SUBREG }}, 

(define_predicate "pic30_near_mode2_operand"
  (ior (match_operand 0 "pic30_mode2_operand")
       (match_operand 0 "pic30_wreg_or_near_operand")))
   
(define_predicate "pic30_near_mode2_APSV_operand"
  (ior (match_operand 0 "pic30_mode2_APSV_operand")
       (match_operand 0 "pic30_wreg_or_near_operand")))
   
;;  { "pic30_near_mode1PN_operand", { REG, MEM, SUBREG,  CONST_INT }}, 

(define_predicate "pic30_near_mode1PN_operand"
  (ior (match_operand 0 "pic30_mode1PN_operand")
       (match_operand 0 "pic30_wreg_or_near_operand")))

(define_predicate "pic30_near_mode1PN_APSV_operand"
  (ior (match_operand 0 "pic30_mode1PN_APSV_operand")
       (match_operand 0 "pic30_wreg_or_near_operand")))

;;  { "pic30_move_operand", { SUBREG, REG, MEM }}, 

(define_predicate "pic30_move_operand"
  (ior (match_operand 0 "pic30_mode3_operand")
       (match_operand 0 "pic30_modek_operand")
       (match_operand 0 "pic30_near_operand")
       (match_operand 0 "pic30_data_operand")))

(define_predicate "pic30_moveb_operand"
  (ior (match_operand 0 "pic30_mode3_operand")
       (match_operand 0 "pic30_near_operand")
       (match_operand 0 "pic30_data_operand")))

(define_predicate "pic30_moveb_APSV_operand"
  (ior (match_operand 0 "pic30_mode3_APSV_operand")
       (match_operand 0 "pic30_near_operand")
       (match_operand 0 "pic30_data_operand")))

(define_predicate "pic30_move_APSV_operand"
  (ior (match_operand 0 "pic30_mode3_APSV_operand")
       (match_operand 0 "pic30_modek_APSV_operand")
       (match_operand 0 "pic30_near_operand")
       (match_operand 0 "pic30_data_operand")))

;;  { "pic30_invalid_address_operand", { SYMBOL_REF, LABEL_REF, CONST }}, 

(define_predicate "pic30_invalid_address_operand"
  (match_test "1")
{
  switch (GET_CODE(op)) {
    default: return FALSE;
    case MEM: return pic30_invalid_address_operand(XEXP(op,0), mode);
    case LABEL_REF:  return 0;
    case SYMBOL_REF:
      if ((PIC30_HAS_NAME_P(XSTR(op,0), PIC30_PROG_FLAG)) ||
          (PIC30_HAS_NAME_P(XSTR(op,0), PIC30_AUXFLASH_FLAG))) {
        const char *real_name = pic30_strip_name_encoding_helper(XSTR(op,0));
        tree sym=0;

        sym = maybe_get_identifier(real_name);
        if (sym) sym = lookup_name(sym);
        if (sym) error("%JInappropriate program address '%s'", sym, real_name);
        else error("Inappropriate program address '%s'", real_name);
        return 1;
      }
      break;
    case CONST:
      op = XEXP (op, 0);
      if (((GET_CODE (XEXP (op, 0)) == SYMBOL_REF) ||
           (GET_CODE (XEXP (op, 0)) == LABEL_REF)) &&
          (GET_CODE (XEXP (op, 1)) == CONST_INT))
      return pic30_invalid_address_operand(XEXP(op,0), mode);
  }
  return 0;
})

;;  { "pic30_symbolic_address_operand", { SYMBOL_REF, LABEL_REF, CONST }}, 

(define_special_predicate "pic30_symbolic_address_operand"
  (match_test "1")
{
  int done;

  do {
    done = 1;
    switch (GET_CODE (op)) {
      case SUBREG:
        if (GET_MODE(op) != mode) break;
        mode = VOIDmode;  /* allow the cast */
        op = XEXP(op,0);
        done = 0;
        break;
      case SYMBOL_REF:
      case LABEL_REF:
        if ((mode == P24PSVmode) ||
            (mode == P24PROGmode) ||
            (mode == VOIDmode)) {
          int string_constant = 0;

          if (GET_CODE(op) == SYMBOL_REF) {
            tree exp;
            exp = SYMBOL_REF_DECL(op);

            /* I assume this can only happen in there is no SYMBOL_REF
               which means that this is coming from IV cost anaylis? */
            if (exp == 0) return TRUE;

            string_constant = ((TREE_CODE(exp) == STRING_CST) &&
                               (TARGET_CONST_IN_CODE || TARGET_CONST_IN_PSV));
          }
          if (((mode == VOIDmode) || (GET_MODE(op) == mode)) &&
              (string_constant || pic30_builtin_tblpsv_arg_p(NULL,op)))
            return TRUE;
        }
        if ((mode == P16APSVmode) || (mode == VOIDmode)) {
          if (pic30_has_space_operand_p(op,PIC30_APSV_FLAG)) return TRUE;
          if (pic30_data_space_operand_p(GET_MODE(op),op,0)) return TRUE;
        }
        if ((mode == P32DFmode) || (mode == VOIDmode)) {
          if (pic30_has_space_operand_p(op,PIC30_DF_FLAG)) return TRUE;
        }
        if ((mode == P32EXTmode) || (mode == VOIDmode)) {
          if (pic30_has_space_operand_p(op,PIC30_EXT_FLAG)) return TRUE;
        }
        if ((mode == P16PMPmode) || (mode == VOIDmode)) {
          if (pic30_has_space_operand_p(op,PIC30_PMP_FLAG)) return TRUE;
        }
        if ((mode == P32EDSmode) ||
            (mode == P32PEDSmode) ||
            (mode == VOIDmode)) {
#if 0
          /* IV cost analysis sometimes makes an 'empty' symbol to validate
             the costs, and sym would be null.  

             I don't really think we need this code anyway, we are just 
             checking to see if it is a string... what else would it be? */
          tree sym = SYMBOL_REF_DECL(op);

          if (GET_CODE(op) == SYMBOL_REF) {
            if (TREE_CODE(sym) == STRING_CST) return TRUE;
          }
#endif
          /* these pointers are compatible with a multitude of spaces */
          if (pic30_eds_space_operand_p(op)) return TRUE;
        }
        if (mode == HImode) return TRUE;
        break;
      case CONST:
        op = XEXP (op, 0);
        if ((mode == P24PSVmode) ||
            (mode == P24PROGmode) ||
            (mode == VOIDmode)) {
          if (((GET_CODE (XEXP (op, 0)) == SYMBOL_REF ||
                    GET_CODE (XEXP (op, 0)) == LABEL_REF) &&
                   (GET_MODE(XEXP(op,0)) == mode) &&
                   (pic30_builtin_tblpsv_arg_p(NULL,op))) &&
                  GET_CODE (XEXP (op, 1)) == CONST_INT) return TRUE;
        }
        if ((mode == P16PMPmode) || (mode == P32EXTmode) || 
            (mode == P32DFmode)) {
          op = XEXP(op,0);
          done = 0;
          break;
        } else {
          return (((GET_CODE (XEXP (op, 0)) == SYMBOL_REF ||
                    GET_CODE (XEXP (op, 0)) == LABEL_REF)) &&
                  GET_CODE (XEXP (op, 1)) == CONST_INT);
        }
      default:
        break;
    }
  } while (!done);
  return FALSE;
})

;;  { "pic30_call_address_operand", { SYMBOL_REF, LABEL_REF, CONST, REG }}, 

(define_predicate "pic30_call_address_operand"
  (ior (match_code "reg")
       (match_operand 0 "pic30_symbolic_address_operand")))

(define_predicate "pic30_reg_or_symbolic_address"
  (ior (match_operand 0 "pic30_register_operand")
       (match_operand 0 "pic30_symbolic_address_operand")))

;;  { "pic30_reg_or_zero_operand", { SUBREG, REG, CONST_INT }}, 

(define_special_predicate "pic30_reg_or_zero_operand"
  (ior (match_test "op == CONST0_RTX(mode)")
       (match_operand 0 "register_operand")))
 
;;  { "pic30_rR_or_zero_operand", { SUBREG, REG, MEM, CONST_INT }}, 

(define_special_predicate "pic30_rR_or_zero_operand"
  (ior (match_test "op == CONST0_RTX(mode)")
       (match_operand 0 "pic30_reg_or_R_operand")))

;;  { "pic30_O_operand", { CONST_INT }}, 

(define_special_predicate "pic30_O_operand"
  (match_test "op == CONST0_RTX(mode)"))

;;  { "pic30_I_operand", { CONST_INT }}, 

(define_special_predicate "pic30_I_operand"
  (match_test "op == CONST1_RTX(mode)"))

;;  { "pic30_PN_operand", { CONST_INT }}, 

(define_special_predicate "pic30_PN_operand"
  (ior (match_operand 0 "pic30_P_operand")
       (match_operand 0 "pic30_N_operand")))

;;  { "pic30_reg_or_P_operand", { SUBREG, REG, CONST_INT }}, 

(define_special_predicate "pic30_reg_or_P_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "pic30_N_operand")))

;;  { "pic30_rR_or_JN_operand", { MEM, SUBREG, REG, CONST_INT }}, 

(define_special_predicate "pic30_rR_or_JN_operand"
  (ior (match_operand 0 "pic30_reg_or_R_operand")
       (match_operand 0 "pic30_JN_operand")))

(define_special_predicate "pic30_rR_or_JN_APSV_operand"
  (ior (match_operand 0 "pic30_reg_or_R_APSV_operand")
       (match_operand 0 "pic30_JN_operand")))

;;  { "pic30_Q_operand", { MEM }}, 

(define_predicate "pic30_Q_operand"
  (match_test "pic30_Q_constraint(op)"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Uses of UNSPEC in this file:

(define_constants
 [(UNSPEC_POPHI		1) ; pop HI
  (UNSPEC_PUSHSHADOW	2) ; push.s -- push shadowed registers
  (UNSPEC_POPSHADOW	3) ; pop.s -- pop shadowed registers
 ]
)

;; UNSPEC_VOLATILE:

(define_constants
 [
  (UNSPECV_BLOCKAGE	0) ; block insn movement
  (UNSPECV_PA		1) ; enable/disable PA
  (UNSPECV_IV		2) ; interrupt vector
  (UNSPECV_PP		3) ; pre-prologue
  (UNSPECV_NOP		4) ; '__builtin_nop' instruction
  
  
  
  
  (UNSPECV_DIVSD	9) ; '__builtin_divsd' instruction
  (UNSPECV_DIVUD       10) ; '__builtin_divud' instruction
  (UNSPECV_MULSS       11) ; '__builtin_mulss' instruction
  (UNSPECV_MULSU       12) ; '__builtin_mulsu' instruction
  (UNSPECV_MULUS       13) ; '__builtin_mulus' instruction
  (UNSPECV_MULUU       14) ; '__builtin_muluu' instruction
  (UNSPECV_READSFR     15) ; '__builtin_readsfr' instruction
  (UNSPECV_DISI        16) ; disi instruction
  (UNSPECV_WRITESFR    17) ; '__builtin_writesfr' instruction
  (UNSPECV_WRITEDISICNT 18) ; 
  (UNSPECV_SAC         19) ; __builtin_sac
  (UNSPECV_SACR        20) ; __builtin_sacr
  (UNSPECV_FBCL        21) ; __builtin_fbcl
  (UNSPECV_LAC         22) ; __builtin_lac
  (UNSPECV_SFTAC       23) ; __builtin_sftac
  (UNSPECV_RRNC        24) ; rrnc
  (UNSPECV_TBLADDRESS  25) ; __builtin_tbladdress
  (UNSPECV_SETPSV      26) ; set psv
  (UNSPECV_WRITEOSCCONH 27) ; __builtin_write_OSCCONH
  (UNSPECV_WRITENVM    28) ; __builtin_write_NVM
  (UNSPECV_MODSD       29) ; __builtin_modsd
  (UNSPECV_MODUD       30) ; __builtin_modud
  (UNSPECV_DIVMODSD    31) ; __builtin_divmodsd
  (UNSPECV_DIVMODUD    32) ; __builtin_divmodud
  (UNSPECV_DSPMULT     33) ; __builtin_mpy, etc
  (UNSEPCV_GENLABEL    34) ; __builtin_unique_id
  (UNSPECV_WRITEOSCCONL 35) ; __builtin_write_OSCCONL
  (UNSPECV_TBLRDL      36) ; __builtin_tblrdl
  (UNSPECV_TBLRDH      37) ; __builtin_tblrd7
  (UNSPECV_TBLWTL      38) ; __builtin_tblwtl
  (UNSPECV_TBLWTH      39) ; __builtin_tblwth
  (UNSPEC_SAVE_CONSTPSV 40)
  (UNSPECV_UNPACKEDS   41)
  (UNSPECV_P24PROGPTR  42)
  (UNSPECV_UNPACKMPSV  43)  
  (UNSPECV_UNPACKMPROG 44)  
  (UNSPECV_DIVF        45)
  (UNSPECV_WRITERTCWEN 46)
  (UNSPECV_PSVRDPSV    47) ; psv pointer access
  (UNSPECV_PSVRDPROG   48) ; psv pointer access
  (UNSPECV_PMPRD       49) ; PMP read
  (UNSPECV_PMPWT       50) ; PMP write
  (UNSPECV_EXTRD       51) ; EXT read
  (UNSPECV_EXTWT       52) ; EXT write
  (UNSPECV_WHILEPMMODE 53) ; while (!PMMODEbits.busy)
  (UNSPECV_PMDIN1RD    54) ; read PMPDIN1
  (UNSPECV_PMDIN1WT    55) ; write PMPDIN1
  (UNSPECV_SETPMMODE   56) 
  (UNSPECV_CLRPMMODE   57)
  (UNSPECV_TBLRDLB     58) ; __builtin_tblrdlb
  (UNSPECV_TBLRDHB     59) ; __builtin_tblrdhb
  (UNSPECV_TBLWTLB     60) ; __builtin_tblwtlb
  (UNSPECV_TBLWTHB     61) ; __builtin_tblwthb
  (UNSPECV_USEPSV      62) ; any operation that uses the PSV
  (UNSPECV_EDSRD       63) ;  EDS read
  (UNSPECV_EDSWT       64) ;  EDS write
  (UNSPECV_PEDSRD      65) ;  PEDS read  (paged)
  (UNSPECV_PEDSWT      66) ;  PEDS write (paged)
  (UNSPECV_SETNVPSV    67) ; set psv nonvolatile
  (UNSPECV_SETDSW      68) ; set dsw 
  (UNSPECV_SETNVDSW    69) ; set dsw nonvolatile
  (UNSPECV_OFFSET      70)  
  (UNSPECV_PAGE        71)  
  (UNSPECV_NOEDSRD     72)
  (UNSPECV_NOEDSWT     72)
  (UNSPECV_WRITEPWMCON 73) ; __builtin_write_PWMCON
  (UNSPECV_WRITECRTOTP 74) ; __builtin_write_CRYOTP
  (UNSPEC_SECTION_BEGIN 75)
  (UNSPEC_SECTION_END  76)
  (UNSPEC_SECTION_SIZE 77)
  (UNSPECV_SET_ISR_STATE 78)
  (UNSPEC_GET_ISR_STATE 79)
  (UNSPECV_ENABLE_ISR  80)
  (UNSPECV_DISABLE_ISR 81)
  (UNSPECV_ENABLE_ISR_GIE  82)
  (UNSPECV_DISABLE_ISR_GIE 83)
  (UNSPEC_EXTRACT_GIE  84)
  (UNSPEC_INSERT_GIE   85)
  (UNSPECV_TEMP        199)
 ]
)

;; Hard registers (SFRs)

(define_constants
 [
  (FPREG	14) ; Frame-pointer hard register number
  (SPREG	15) ; Stack-pointer hard register number
  (RCOUNT	16) ; Repeat-count hard register number
  (PSVPAG       19) ; PSVPAG or DSRPAG
  (PMADDR       20) 
  (PMMODE       21)
  (PMDIN1       22)
  (PMDIN2       23)
  (DSWPAG       24)
 ]
)
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Instruction scheduling
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Memory access attribute.
;; We only care about loads and stores.
;; use -- indirect memory load
;; def -- store to register
;; Everything else is 'etc'.
;;
;; For dealing with RAW stalls, we use the following truth table,
;; where D denotes a store to a wreg (definition), and U denotes an
;; indirect load of a source operand (use).
;;
;;           type   description
;; +---+---+
;; | D | U | 
;; +---+---+
;; | F | F | etc    Insn does not load indirect, does not store to wreg
;; | F | T | use    Insn loads indirect
;; | T | F | def    Insn stores to wreg
;; | T | T | defuse Insn loads indirect, and stores to wreg
;; +---+---+
;;
;; RAW stalls occur when D is followed by U.

(define_automaton "dsPIC30F")

(define_cpu_unit "sched_raw" "dsPIC30F")

(define_insn_reservation "insn_def" 0
  (eq_attr "type" "def")
  "sched_raw")

(define_insn_reservation "insn_use" 1
  (eq_attr "type" "use")
  "sched_raw")

(define_insn_reservation "insn_defuse" 1
  (eq_attr "type" "defuse")
  "sched_raw")

(define_insn_reservation "insn_etc" 0
  (eq_attr "type" "etc")
  "nothing")

;; Insn type.

(define_attr "type"
  "def,use,defuse,etc"
  (const_string "etc"))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
;; dsp instructions
; DSP instructions
;

; ********* to support builtins

(define_insn "addac_hi"
  [(set (match_operand: HI          0 "pic30_accumulator_operand" "=w")
        (plus:HI (match_operand: HI 1 "pic30_accumulator_operand" "w") 
                 (match_operand: HI 2 "pic30_accumulator_operand" "w")))]
  "(REGNO(operands[1]) != REGNO(operands[2]))"
  "add %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addac2_hi"
  [(set (match_operand: HI          0 "pic30_accumulator_operand" "=w")
        (plus:HI (match_dup 0)
                 (match_operand: HI 1 "pic30_accumulator_operand" "w")))]
  "(REGNO(operands[1]) != REGNO(operands[0]))"
  "add %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftrt_hi"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (ashiftrt:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " rRS<>")
             (match_operand: HI 2 "immediate_operand"         " Z"))
           (match_operand:HI    3 "pic30_accumulator_operand" " 0")))]
  "(INTVAL(operands[2]) >= 0)"
  "add %1, #%2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftrt_hi_errata"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w,w")
        (plus:HI 
           (ashiftrt:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
             (match_operand: HI 2 "immediate_operand"         " Z,Z"))
           (match_operand:HI    3 "pic30_accumulator_operand" " 0,0")))
   (clobber (match_scratch: HI  4                             "=X,r"))
  ]
  "(INTVAL(operands[2]) >= 0)"
  "@
   add %1, #%2, %0
   mov %1,%4\;add %4,#%2,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftlt_hi"
  [(set (match_operand: HI               0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (ashift:HI (match_operand: HI 1 "pic30_mode3_operand"       " RS<>r")
                      (match_operand: HI 2 "immediate_operand"         " i"))
           (match_operand:HI    3 "pic30_accumulator_operand"          "0")))]
  "(INTVAL(operands[2]) >= 0)"
  "add %1, #%J2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftlt_hi_errata"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w,w")
        (plus:HI
           (ashift:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
             (match_operand: HI 2 "immediate_operand"         " i,i"))
           (match_operand:HI    3 "pic30_accumulator_operand" " 0,0")))
   (clobber (match_scratch:HI   4                             "=X,r"))
  ]
  "(INTVAL(operands[2]) >= 0)"
  "@
   add %1, #%J2, %0
   mov %1,%4\;add %4,#%J2,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_noshift_hi"
  [(set (match_operand: HI    0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (match_operand: HI 1 "pic30_mode3_operand"       " RS<>r")
           (match_operand:HI  2 "pic30_accumulator_operand" " 0")))]
  ""
  "add %1, #0, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_noshift1_hi"
  [(set (match_operand: HI    0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (match_operand:HI  1 "pic30_accumulator_operand" " 0")
           (match_operand: HI 2 "pic30_mode3_operand"       " RS<>r")))]
  ""
  "add %2, #0, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_noshift_hi_errata"
  [(set (match_operand: HI    0 "pic30_accumulator_operand" "=w,w")
        (plus:HI
           (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
           (match_operand:HI  2 "pic30_accumulator_operand" " 0,0")))
   (clobber (match_scratch:HI 3                             "=X,r"))
  ]
  ""
  "@
   add %1, #0, %0
   mov %1,%3\;add %3,#0,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_noshift1_hi_errata"
  [(set (match_operand: HI    0 "pic30_accumulator_operand" "=w,w")
        (plus:HI
           (match_operand:HI  1 "pic30_accumulator_operand" " 0,0")
           (match_operand: HI 2 "pic30_mode3_operand"       " r,RS<>")))
   (clobber (match_scratch:HI 3                             "=X,r"))
  ]
  ""
  "@
   add %2, #0, %0
   mov %2,%3\;add %3,#0,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftrt1_hi"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w")
        (plus:HI 
           (match_dup 0)
           (ashiftrt:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " RS<>r")
             (match_operand: HI 2 "immediate_operand"         " Z"))))]
  "((INTVAL(operands[2]) >= 0) && !(pic30_errata_mask & psv_errata))"
  "add %1, #%2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftrt1_hi_errata"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w,w")
        (plus:HI 
           (match_dup 0)
           (ashiftrt:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
             (match_operand: HI 2 "immediate_operand"         " Z,Z"))))
   (clobber (match_scratch:HI   3                             "=X,r"))
  ]
  "((INTVAL(operands[2]) >= 0) && (pic30_errata_mask & psv_errata))"
  "@
   add %1, #%2, %0
   mov %1,%3\;add %3,#%2,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftlt1_hi"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (ashift:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " RS<>r")
             (match_operand: HI 2 "immediate_operand"         " i"))
           (match_dup 0)))]
  "((INTVAL(operands[2]) >= 0) && !(pic30_errata_mask & psv_errata))"
  "add %1, #%J2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "addacr_shiftlt1_hi_errata"
  [(set (match_operand: HI      0 "pic30_accumulator_operand" "=w,w")
        (plus:HI
           (ashift:HI 
             (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
             (match_operand: HI 2 "immediate_operand"         " i,i"))
           (match_dup 0)))
   (clobber (match_scratch: HI  3                             "=X,r"))
  ]
  "((INTVAL(operands[2]) >= 0) && (pic30_errata_mask & psv_errata))"
  "@
   add %1, #%J2, %0
   mov %1,%3\;add %3,#%J2,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "clrac_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w,w,w,w,w,w,w,w,w")
        (const_int 0))
   (set (match_operand: HI 1 "pic30_mac_input_operand"   "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 2 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 3 "pic30_xprefetch_operand"   "=2,B,B,2,B,B,2,B,B")
        (plus:HI
          (match_dup 3)
          (match_operand: HI 4 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 5 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 6 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 7 "pic30_yprefetch_operand"   "=6,6,B,B,B,B,B,6,B")
        (plus:HI
          (match_dup 7)
          (match_operand: HI 8 "immediate_operand"       " Y,Y,i,i,i,i,i,Y,i")))
  ]
  ""
  "@
   clr %0, [%2]+=%4, %1, [%6]+=%8, %5
   clr %0, [%6]+=%8, %5
   clr %0, [%6], %5
   clr %0, [%2]+=%4, %1
   clr %0, [%2], %1
   clr %0, [%2], %1, [%6], %5
   clr %0, [%2]+=%4, %1, [%6], %5
   clr %0, [%2], %1, [%6]+=%8, %5
   clr %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "clracawb_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w,w,w,w,w,w,w,w,w")
        (const_int 0))
   (set (match_operand: HI 1 "pic30_mac_input_operand"   "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 2 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 3 "pic30_xprefetch_operand"   "=2,B,B,2,B,B,2,B,B")
        (plus:HI
          (match_dup 3)
          (match_operand: HI 4 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 5 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 6 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 7 "pic30_yprefetch_operand"   "=6,6,B,B,B,B,B,6,B")
        (plus:HI
          (match_dup 7)
          (match_operand: HI 8 "immediate_operand"       " Y,Y,i,i,i,i,i,Y,i")))
   (set (match_operand: HI 9 "pic30_awb_operand"         "=v,v,v,v,v,v,v,v,v")
        (unspec:HI [
          (match_operand:HI 10 "pic30_accumulator_operand" " w,w,w,w,w,w,w,w,w")
          (const_int 0)
        ] UNSPECV_SAC))
  ]
  "(REGNO(operands[0]) != REGNO(operands[10]))"
  "@
   clr %0, [%2]+=%4, %1, [%6]+=%8, %5, %9
   clr %0, [%6]+=%8, %5, %9
   clr %0, [%6], %5, %9
   clr %0, [%2]+=%4, %1, %9
   clr %0, [%2], %1, %9
   clr %0, [%2], %1, [%6], %5, %9
   clr %0, [%2]+=%4, %1, [%6], %5, %9
   clr %0, [%2], %1, [%6]+=%8, %5, %9
   clr %0, %9"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "ed_hi"
 [
  (set (match_operand:HI 0 "pic30_accumulator_operand" "=w,w,w,w")
       (unspec:HI [
           (match_operand:HI 1 "pic30_mac_input_operand" "z,z,z,z")
           (match_dup 1)
         ] UNSPECV_DSPMULT))
  (set (match_operand:HI 2 "pic30_mac_input_operand" "=z,z,z,z")
       (minus: HI
         (mem: HI (match_operand:HI 3 "pic30_xprefetch_operand" "x,x,x,x"))
         (mem: HI (match_operand:HI 4 "pic30_yprefetch_operand" "y,y,y,y"))))
  (set (match_operand:HI 5 "pic30_xprefetch_operand" "=3,3,B,B")
       (plus: HI
         (match_dup 5)
         (match_operand: HI 6 "immediate_operand" "Y,Y,i,i")))
  (set (match_operand:HI 7 "pic30_yprefetch_operand" "=4,B,B,4")
       (plus: HI
         (match_dup 7)
         (match_operand: HI 8 "immediate_operand" "Y,i,i,Y")))
 ]
 ""
 "@
  ed %1*%1, %0, [%3]+=%6, [%4]+=%8, %2
  ed %1*%1, %0, [%3]+=%6, [%4], %2
  ed %1*%1, %0, [%3], [%4], %2
  ed %1*%1, %0, [%3], [%4]+=%8, %2"
 [
   (set_attr "cc" "unchanged")
 ]
)

(define_insn "edac_hi"
 [
  (set (match_operand:HI 0 "pic30_accumulator_operand" "=w,w,w,w")
       (plus: HI
         (match_operand:HI 1 "pic30_accumulator_operand" "0,0,0,0")
         (unspec:HI [
             (match_operand:HI 2 "pic30_mac_input_operand" "z,z,z,z")
             (match_dup 2)
           ] UNSPECV_DSPMULT)))
  (set (match_operand:HI 3 "pic30_mac_input_operand" "=z,z,z,z")
       (minus: HI
         (mem: HI (match_operand:HI 4 "pic30_xprefetch_operand" "x,x,x,x"))
         (mem: HI (match_operand:HI 5 "pic30_yprefetch_operand" "y,y,y,y"))))
  (set (match_operand:HI 6 "pic30_xprefetch_operand" "=4,4,B,B")
       (plus: HI
         (match_dup 6)
         (match_operand: HI 7 "immediate_operand" "Y,Y,i,i")))
  (set (match_operand:HI 8 "pic30_yprefetch_operand" "=5,B,B,5")
       (plus: HI
         (match_dup 8)
         (match_operand: HI 9 "immediate_operand" "Y,i,i,Y")))
 ]
 ""
 "@
  edac %2*%2, %0, [%4]+=%7, [%5]+=%9, %3
  edac %2*%2, %0, [%4]+=%6, [%5], %3
  edac %2*%2, %0, [%4], [%5], %3
  edac %2*%2, %0, [%4], [%5]+=%9, %3"
 [
   (set_attr "cc" "unchanged")
 ]
)

(define_insn "fbcl_hi"
 [ 
  (set (match_operand:HI 0 "pic30_register_operand" "=r")
       (unspec: HI [
                     (match_operand: HI 1 "pic30_mode2_operand" "rR<>")
                   ] UNSPECV_FBCL))
 ]
 ""
 "fbcl %1, %0"
 [
  (set_attr "cc" "clobber")
 ]
)

(define_insn "lac_hi"
  [
  (set (match_operand:HI    0 "pic30_accumulator_operand" "=w")
       (unspec:HI [
         (match_operand: HI 1 "pic30_mode3_operand"       " rRS<>")
         (match_operand: HI 2 "immediate_operand"         " Z")
       ] UNSPECV_LAC))
  ]
  "(!(pic30_errata_mask & psv_errata))"
  "lac %1, #%2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "lac_error"
  [
  (set (match_operand:HI    0 "pic30_accumulator_operand" "=w")
       (unspec:HI [
         (match_operand: HI 1 "pic30_accumulator_operand" "w")
         (match_operand: HI 2 "immediate_operand"         " Z")
       ] UNSPECV_LAC))
  ]
  ""
  "*{
     error(\"Argument 0 should not be an accumulator register\");
   }
  "
)

(define_insn "lac_hi_errata"
  [
  (set (match_operand:HI    0 "pic30_accumulator_operand" "=w,w")
       (unspec:HI [
         (match_operand: HI 1 "pic30_mode3_operand"       " r,RS<>")
         (match_operand: HI 2 "immediate_operand"         " Z,Z")
       ] UNSPECV_LAC))
  (clobber (match_scratch:HI 3                            "=X,r"))
  ]
  "(pic30_errata_mask & psv_errata)"
  "@
   lac %1, #%2, %0
   mov %1,%3\;lac %3,#%2,%0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "sac_gen_hi"
  [
   (set (match_operand: HI 0 "pic30_mode3_operand" "=RS<>r")
        (unspec:HI [
                     (match_operand: HI 1 "pic30_accumulator_operand" "w")
                     (match_operand: HI 2 "immediate_operand" "Z")
                   ] UNSPECV_SAC))
  ]
  ""
  "sac %1, #%2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "sacr_gen_hi"
  [
   (set (match_operand: HI 0 "pic30_mode3_operand" "=RS<>r")
        (unspec:HI [
                     (match_operand: HI 1 "pic30_accumulator_operand" "w")
                     (match_operand: HI 2 "immediate_operand" "Z")
                   ] UNSPECV_SACR))
  ]
  ""
  "sac.r %1, #%2, %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "mac_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand"  "=w,w,w,w,w,w,w,w,w")
        (plus:HI 
          (match_operand: HI 1 "pic30_accumulator_operand" "0,0,0,0,0,0,0,0,0")
          (unspec:HI [
            (match_operand:HI 2 "pic30_mac_input_operand"  "z,z,z,z,z,z,z,z,z")
            (match_operand:HI 3 "pic30_mac_input_operand"  "z,z,z,z,z,z,z,z,z")
            ] UNSPECV_DSPMULT)))
   (set (match_operand: HI 4 "pic30_mac_input_operand"    "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_xprefetch_operand"   "x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 6 "pic30_xprefetch_operand"    "=5,B,B,5,B,B,5,B,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"        " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 8 "pic30_mac_input_operand"    "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 9 "pic30_yprefetch_operand"  " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 10 "pic30_yprefetch_operand"   "=9,9,B,B,B,B,B,9,B")
        (plus:HI
          (match_dup 10)
          (match_operand: HI 11 "immediate_operand"      "  Y,Y,i,i,i,i,i,Y,i")))
  ]
  "" 
  "* 
   {
     const char *mac_options[] = {
       \"mac %2*%3, %0, [%5]+=%7, %4, [%9]+=%11, %8\",  /* 0 */
       \"mac %2*%3, %0, [%9]+=%11, %8\",                /* 1 */
       \"mac %2*%3, %0, [%9], %8\",                     /* 2 */
       \"mac %2*%3, %0, [%5]+=%7, %4\",                 /* 3 */
       \"mac %2*%3, %0, [%5], %4\",                     /* 4 */
       \"mac %2*%3, %0, [%5], %4, [%9], %8\",           /* 5 */
       \"mac %2*%3, %0, [%5]+=%7, %4, [%9], %8\",       /* 6 */
       \"mac %2*%3, %0, [%5], %4, [%9]+=%11, %8\",      /* 7 */
       \"mac %2*%3, %0\",                               /* 8 */
       \"mac %3*%2, %0, [%5]+=%7, %4, [%9]+=%11, %8\",
       \"mac %3*%2, %0, [%9]+=%11, %8\",
       \"mac %3*%2, %0, [%9], %8\",
       \"mac %3*%2, %0, [%5]+=%7, %4\",
       \"mac %3*%2, %0, [%5], %4\",
       \"mac %3*%2, %0, [%5], %4, [%9], %8\",
       \"mac %3*%2, %0, [%5]+=%7, %4, [%9], %8\",
       \"mac %3*%2, %0, [%5], %4, [%9]+=%11, %8\",
       \"mac %3*%2, %0\" };

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return mac_options[which_alternative];
     } else {
       return mac_options[which_alternative+9];
     }
   }
  "
  [
    (set_attr "cc" "unchanged")
  ]
)

;
; GCC had trouble respecting the pre-condition; for now restrict the
;   register choices so that u and t don't overlap... this can be improved.
;   also for msc_gen_hi and msc_awbgen_hi
;
(define_insn  "macawb_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand"   "=w,w,w,w,w,w,w,w,w")
        (plus:HI
          (match_operand: HI 1 "pic30_accumulator_operand"  "0,0,0,0,0,0,0,0,0")
          (mult:HI
            (match_operand:HI 2 "pic30_mac_input_operand"  " u,u,u,u,u,u,u,u,u")
            (match_operand:HI 3 "pic30_mac_input_operand"  " t,t,t,t,t,t,t,t,t")
   )))
   (set (match_operand: HI 4 "pic30_mac_input_operand"     "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_xprefetch_operand"   " x,B,B,x,x,x,x,x,B")
   ))
   (set (match_operand: HI 6 "pic30_xprefetch_operand"     "=5,B,B,5,B,B,5,B,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"         " Y,i,i,Y,i,i,Y,i,i")
   ))
   (set (match_operand: HI 8 "pic30_mac_input_operand"     "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 9 "pic30_yprefetch_operand"   " y,y,y,B,B,y,y,y,B")
   ))
   (set (match_operand: HI 10 "pic30_yprefetch_operand"    "=9,9,B,B,B,B,B,9,B")
        (plus:HI
          (match_dup 10)
          (match_operand: HI 11 "immediate_operand"        " Y,Y,i,i,i,i,i,Y,i")
   ))
   (set (match_operand: HI 12 "pic30_awb_operand"         "=v,v,v,v,v,v,v,v,v")
        (unspec:HI [
          (match_operand:HI 13 "pic30_accumulator_operand" " w,w,w,w,w,w,w,w,w")
          (const_int 0)
        ] UNSPECV_SAC))
  ]
  "(REGNO(operands[1]) != REGNO(operands[2]))"
  "*
   {
     const char *mac_options[] = { 
       \"mac %2*%3, %0, [%5]+=%7, %4, [%8]+=%11, %8, %12\",              /* 0 */
       \"mac %2*%3, %0, [%9]+=%11, %8, %12\",                            /* 1 */
       \"mac %2*%3, %0, [%9], %8, %12\",                                 /* 2 */
       \"mac %2*%3, %0, [%5]+=%7, %4, %12\",                             /* 3 */
       \"mac %2*%3, %0, [%5], %4, %12\",                                 /* 4 */
       \"mac %2*%3, %0, [%5], %4, [%9], %8, %12\",                       /* 5 */
       \"mac %2*%3, %0, [%5]+=%7, %4, [%9], %8, %12\",                   /* 6 */
       \"mac %2*%3, %0, [%5], %4, [%9]+=%11, %8, %12\",                  /* 7 */
       \"mac %2*%3, %0, %12\",                                           /* 8 */
       \"mac %3*%2, %0, [%5]+=%7, %4, [%9]+=%11, %8, %12\",
       \"mac %3*%2, %0, [%9]+=%11, %8, %12\",
       \"mac %3*%2, %0, [%9], %8, %12\",
       \"mac %3*%2, %0, [%5]+=%7, %4, %12\",
       \"mac %3*%2, %0, [%5], %4, %12\",
       \"mac %3*%2, %0, [%5], %4, [%9], %8, %12\",
       \"mac %3*%2, %0, [%5]+=%7, %4, [%9], %8, %12\",
       \"mac %3*%2, %0, [%5], %4, [%9]+=%11, %8, %12\",
       \"mac %3*%2, %0, %12\" };

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return mac_options[which_alternative];
     } else {
       return mac_options[which_alternative+9];
     }
   }
  "
  [
    (set_attr "cc" "unchanged")
  ]
)

;
;  if an AWB is not specified, then it doesn't matter which accumulator we use
;    since the accumulator portion is only there to specify which ACC is 
;    written back
;
(define_insn "movsac_gen_hi"
  [ (set (match_operand: HI 0 "pic30_mac_input_operand"  "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 1 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 2 "pic30_xprefetch_operand"   "=1,B,B,1,X,X,1,B,B")
        (plus:HI
          (match_dup 2)
          (match_operand: HI 3 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 4 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 6 "pic30_yprefetch_operand"   "=5,5,B,B,B,B,B,5,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"       " Y,Y,i,i,i,i,i,Y,i")))
  ]
  ""
  "@
   movsac A, [%1]+=%3, %0, [%5]+=%7, %4
   movsac A, [%5]+=%7, %4
   movsac A, [%5], %4
   movsac A, [%1]+=%3, %0
   movsac A, [%1], %0
   movsac A, [%1], %0, [%5], %4
   movsac A, [%1]+=%3, %0, [%5], %4
   movsac A, [%1], %0, [%5]+=%7, %4
   movsac A" 
  [
    (set_attr "cc" "unchanged")
  ] 
) 

;
;  this is an odd instruction
;
(define_insn "movsacawb_gen_hi"
  [(set (match_operand: HI 0 "pic30_mac_input_operand"   "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 1 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 2 "pic30_xprefetch_operand"   "=1,B,B,1,B,B,1,B,B")
        (plus:HI
          (match_dup 2)
          (match_operand: HI 3 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 4 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 6 "pic30_yprefetch_operand"   "=5,5,B,B,B,B,B,5,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"       " Y,Y,i,i,i,i,i,Y,i")))
   (set (match_operand: HI 8 "pic30_awb_operand"         "=v,v,v,v,v,v,v,v,v")
        (unspec:HI [
          (match_operand: HI 9 "pic30_accumulator_operand" " w,w,w,w,w,w,w,w,w")
          (const_int 0)
        ] UNSPECV_SAC))
  ]
  ""
  "@
   movsac %A9, [%1]+=%3, %0, [%5]+=%7, %4, %8
   movsac %A9, [%5]+=%7, %4, %8
   movsac %A9, [%5], %4, %8
   movsac %A9, [%1]+=%3, %0, %8
   movsac %A9, [%1], %0, %8
   movsac %A9, [%1], %0, [%5], %4, %8
   movsac %A9, [%1]+=%3, %0, [%5], %4, %8
   movsac %A9, [%1], %0, [%5]+=%7, %4, %8
   movsac %A9, %8"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "mpy_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w,w,w,w,w,w,w,w,w")
        (unspec:HI [
            (match_operand:HI 1 "pic30_mac_input_operand"  " z,z,z,z,z,z,z,z,z")
            (match_operand:HI 2 "pic30_mac_input_operand"  " z,z,z,z,z,z,z,z,z")
          ] UNSPECV_DSPMULT))
   (set (match_operand: HI 3 "pic30_mac_input_operand"   "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 4 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 5 "pic30_xprefetch_operand"   "=4,B,B,4,B,B,4,B,B")
        (plus:HI
          (match_dup 5)
          (match_operand: HI 6 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 7 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 8 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 9 "pic30_yprefetch_operand"   "=8,8,B,B,B,B,B,8,B")
        (plus:HI
          (match_dup 9)
          (match_operand: HI 10 "immediate_operand"      " Y,Y,i,i,i,i,i,Y,i")))
  ]
  "" 
  "* 
   {
     const char *mpy_options[] = {
       \"mpy %1*%2, %0, [%4]+=%6, %3, [%8]+=%10, %7\", /* 0 */
       \"mpy %1*%2, %0, [%8]+=%10, %7\",               /* 1 */
       \"mpy %1*%2, %0, [%8], %7\",                    /* 2 */
       \"mpy %1*%2, %0, [%4]+=%6, %3\",                /* 3 */
       \"mpy %1*%2, %0, [%4], %3\",                    /* 4 */
       \"mpy %1*%2, %0, [%4], %3, [%8], %7\",          /* 5 */
       \"mpy %1*%2, %0, [%4]+=%6, %3, [%8], %7\",      /* 6 */
       \"mpy %1*%2, %0, [%4], %3, [%8]+=%10, %7\",     /* 7 */
       \"mpy %1*%2, %0\",                              /* 8 */
       \"mpy %2*%1, %0, [%4]+=%6, %3, [%8]+=%10, %7\",
       \"mpy %2*%1, %0, [%8]+=%10, %7\",
       \"mpy %2*%1, %0, [%8], %7\",
       \"mpy %2*%1, %0, [%4]+=%6, %3\",
       \"mpy %2*%1, %0, [%4], %3\",
       \"mpy %2*%1, %0, [%4], %3, [%8], %7\",
       \"mpy %2*%1, %0, [%4]+=%6, %3, [%8], %7\",
       \"mpy %2*%1, %0, [%4], %3, [%8]+=%10, %7\",
       \"mpy %2*%1, %0\"};

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return mpy_options[which_alternative];
     } else { 
       return mpy_options[which_alternative+9];
     }
   }
  "
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "mpyn_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w,w,w,w,w,w,w,w,w")
        (unspec:HI [
           (neg:HI
             (match_operand:HI 1 "pic30_mac_input_operand"" z,z,z,z,z,z,z,z,z"))
           (match_operand:HI 2 "pic30_mac_input_operand"  " z,z,z,z,z,z,z,z,z")
          ] UNSPECV_DSPMULT))
   (set (match_operand: HI 3 "pic30_mac_input_operand"   "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 4 "pic30_xprefetch_operand" " x,B,B,x,x,x,x,x,B")))
   (set (match_operand: HI 5 "pic30_xprefetch_operand"   "=4,B,B,4,B,B,4,B,B")
        (plus:HI
          (match_dup 5)
          (match_operand: HI 6 "immediate_operand"       " Y,i,i,Y,i,i,Y,i,i")))
   (set (match_operand: HI 7 "pic30_mac_input_operand"   "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 8 "pic30_yprefetch_operand" " y,y,y,B,B,y,y,y,B")))
   (set (match_operand: HI 9 "pic30_yprefetch_operand"   "=8,8,B,B,B,B,B,8,B")
        (plus:HI
          (match_dup 9)
          (match_operand: HI 10 "immediate_operand"      " Y,Y,i,i,i,i,i,Y,i")))
  ]
  "(REGNO(operands[1]) < REGNO(operands[2]))"
  "*
   {
     const char *mpy_options[] = {
       \"mpy.n %1*%2, %0, [%4]+=%6, %3, [%8]+=%10, %7\", /* 0 */
       \"mpy.n %1*%2, %0, [%8]+=%10, %7\",               /* 1 */
       \"mpy.n %1*%2, %0, [%8], %7\",                    /* 2 */
       \"mpy.n %1*%2, %0, [%4]+=%6, %3\",                /* 3 */
       \"mpy.n %1*%2, %0, [%4], %3\",                    /* 4 */
       \"mpy.n %1*%2, %0, [%4], %3, [%8], %7\",          /* 5 */
       \"mpy.n %1*%2, %0, [%4]+=%6, %3, [%8], %7\",      /* 6 */
       \"mpy.n %1*%2, %0, [%4], %3, [%8]+=%10, %7\",     /* 7 */
       \"mpy.n %1*%2, %0\",                              /* 8 */
       \"mpy.n %2*%1, %0, [%4]+=%6, %3, [%8]+=%10, %7\",
       \"mpy.n %2*%1, %0, [%8]+=%10, %7\",
       \"mpy.n %2*%1, %0, [%8], %7\",
       \"mpy.n %2*%1, %0, [%4]+=%6, %3\",
       \"mpy.n %2*%1, %0, [%4], %3\",
       \"mpy.n %2*%1, %0, [%4], %3, [%8], %7\",
       \"mpy.n %2*%1, %0, [%4]+=%6, %3, [%8], %7\",
       \"mpy.n %2*%1, %0, [%4], %3, [%8]+=%10, %7\",
       \"mpy.n %2*%1, %0\"};

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return mpy_options[which_alternative];
     } else {
       return mpy_options[which_alternative+9];
     }
   }
  "
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "msc_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand"   "=w,w,w,w,w,w,w,w,w")
        (minus:HI 
          (match_operand: HI 1 "pic30_accumulator_operand"  "0,0,0,0,0,0,0,0,0")
          (unspec:HI [
            (match_operand:HI 2 "pic30_mac_input_operand"  " u,u,u,u,u,u,u,u,u")
            (match_operand:HI 3 "pic30_mac_input_operand"  " t,t,t,t,t,t,t,t,t")
          ] UNSPECV_DSPMULT)))
   (set (match_operand: HI 4 "pic30_mac_input_operand"     "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_xprefetch_operand"   " x,B,B,x,x,x,x,x,B")
   ))
   (set (match_operand: HI 6 "pic30_xprefetch_operand"     "=5,B,B,5,B,B,5,B,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"         " Y,i,i,Y,i,i,Y,i,i")
   ))
   (set (match_operand: HI 8 "pic30_mac_input_operand"     "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 9 "pic30_yprefetch_operand"   " y,y,y,B,B,y,y,y,B")
   ))
   (set (match_operand: HI 10 "pic30_yprefetch_operand"    "=9,9,B,B,B,B,B,9,B")
        (plus:HI
          (match_dup 10)
          (match_operand: HI 11 "immediate_operand"        " Y,Y,i,i,i,i,i,Y,i")
   ))
  ]
  "" 
  "* 
   {
     const char *msc_options[] = {
       \"msc %2*%3, %0, [%5]+=%7, %4, [%9]+=%11, %8\",
       \"msc %2*%3, %0, [%9]+=%11, %8\",
       \"msc %2*%3, %0, [%9], %8\",
       \"msc %2*%3, %0, [%5]+=%7, %4\",
       \"msc %2*%3, %0, [%5], %4\",
       \"msc %2*%3, %0, [%5], %4, [%9], %8\",
       \"msc %2*%3, %0, [%5]+=%7, %4, [%9], %8\",
       \"msc %2*%3, %0, [%5], %4, [%9]+=%11, %8\",
       \"msc %2*%3, %0\",
       \"msc %3*%2, %0, [%5]+=%7, %4, [%9]+=%11, %8\",
       \"msc %3*%2, %0, [%9]+=%11, %8\",
       \"msc %3*%2, %0, [%9], %8\",
       \"msc %3*%2, %0, [%5]+=%7, %4\",
       \"msc %3*%2, %0, [%5], %4\",
       \"msc %3*%2, %0, [%5], %4, [%9], %8\",
       \"msc %3*%2, %0, [%5]+=%7, %4, [%9], %8\",
       \"msc %3*%2, %0, [%5], %4, [%9]+=%11, %8\",
       \"msc %3*%2, %0\"};

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return msc_options[which_alternative];
     } else { 
       return msc_options[which_alternative+9];
     }
   }
  "
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "mscawb_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand"   "=w,w,w,w,w,w,w,w,w")
        (minus:HI
          (match_operand: HI 1 "pic30_accumulator_operand"  "0,0,0,0,0,0,0,0,0")
          (unspec:HI [
            (match_operand:HI 2 "pic30_mac_input_operand"  " u,u,u,u,u,u,u,u,u")
            (match_operand:HI 3 "pic30_mac_input_operand"  " t,t,t,t,t,t,t,t,t")
          ] UNSPECV_DSPMULT)))
   (set (match_operand: HI 4 "pic30_mac_input_operand"     "=z,B,B,z,z,z,z,z,B")
        (mem:HI
          (match_operand: HI 5 "pic30_xprefetch_operand"   " x,B,B,x,x,x,x,x,B")
   ))
   (set (match_operand: HI 6 "pic30_xprefetch_operand"     "=5,B,B,5,B,B,5,B,B")
        (plus:HI
          (match_dup 6)
          (match_operand: HI 7 "immediate_operand"         " Y,i,i,Y,i,i,Y,i,i")
   ))
   (set (match_operand: HI 8 "pic30_mac_input_operand"     "=z,z,z,B,B,z,z,z,B")
        (mem:HI
          (match_operand: HI 9 "pic30_yprefetch_operand"   " y,y,y,B,B,y,y,y,B")
   ))
   (set (match_operand: HI 10 "pic30_yprefetch_operand"    "=9,9,B,B,B,B,B,9,B")
        (plus:HI
          (match_dup 10)
          (match_operand: HI 11 "immediate_operand"        " Y,Y,i,i,i,i,i,Y,i")
   ))
   (set (match_operand: HI 12 "pic30_awb_operand"         "=v,v,v,v,v,v,v,v,v")
        (unspec:HI [
          (match_operand:HI 13 "pic30_accumulator_operand" " w,w,w,w,w,w,w,w,w")
          (const_int 0)
        ] UNSPECV_SAC)
   )
  ]
  ""
  "*
   {
     const char *msc_options[] = {
       \"msc %2*%3, %0, [%5]+=%7, %4, [%9]+=%11, %8, %12\",
       \"msc %2*%3, %0, [%9]+=%11, %8, %12\",
       \"msc %2*%3, %0, [%9], %8, %12\",
       \"msc %2*%3, %0, [%5]+=%7, %4, %12\",
       \"msc %2*%3, %0, [%5], %4, %12\",
       \"msc %2*%3, %0, [%5], %4, [%9], %8, %12\",
       \"msc %2*%3, %0, [%5]+=%7, %4, [%9], %8, %12\",
       \"msc %2*%3, %0, [%5], %4, [%9]+=%11, %8, %12\",
       \"msc %2*%3, %0, %12\",
       \"msc %3*%2, %0, [%5]+=%7, %4, [%9]+=%11, %8, %12\",
       \"msc %3*%2, %0, [%9]+=%11, %8, %12\",
       \"msc %3*%2, %0, [%9], %8, %12\",
       \"msc %3*%2, %0, [%5]+=%7, %4, %12\",
       \"msc %3*%2, %0, [%5], %4, %12\",
       \"msc %3*%2, %0, [%5], %4, [%9], %8, %12\",
       \"msc %3*%2, %0, [%5]+=%7, %4, [%9], %8, %12\",
       \"msc %3*%2, %0, [%5], %4, [%9]+=%11, %8, %12\",
       \"msc %3*%2, %0, %12\"};

     if (REGNO(operands[1]) < REGNO(operands[2])) {
       return msc_options[which_alternative];
     } else {
       return msc_options[which_alternative+9];
     }
   }
  " 
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "sftac_gen_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w,w")
        (unspec:HI [
                     (match_operand: HI 1 "pic30_accumulator_operand" "0,0")
                     (match_operand: HI 2 "pic30_reg_or_imm_operand" "r,W")
                   ] UNSPECV_SFTAC)
   )]
  ""
  "@
   sftac %0, %2
   sftac %0, #%2"
  [
    (set_attr "cc" "unchanged")
  ]
)

(define_insn "sftac_error"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w")
        (ashift:HI
           (match_operand: HI 1 "general_operand" "")
           (match_operand: HI 2 "general_operand" "")))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_sftac() instead\");
   }
  "
)

(define_insn "subac_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand"  "=w")
        (minus:HI (match_operand: HI 1 "pic30_accumulator_operand" "w")
                  (match_operand: HI 2 "pic30_accumulator_operand" "w")))]
  "(REGNO(operands[2]) != REGNO(operands[1]))"
  "sub %0"
  [
    (set_attr "cc" "unchanged")
  ]
)

; ********* to support automagic generation

(define_insn "movhi_accumulator"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (match_operand: HI 1 "pic30_mode3_operand" "RS<>r"))]
  ""
  "*{
     /* lac %1, %0 */
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_lac() instead\");
   }
  "
)  

(define_insn "movhi_accumulator2"
  [(set (match_operand:HI 0 "general_operand" "")
        (match_operand:HI 1 "pic30_accumulator_operand" "w"))]
  ""
  "*{
     /* sac %1, %0 */
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_sac() instead\");
   }
  "
)

(define_insn "movhi_accumulator3"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (match_operand:HI 1 "immediate_operand" "i"))]
  ""
  "*
   {
     if (INTVAL(operands[1]) == 0) {
       /* clr %0 */
       error(\"Automatic generation of DSP instructions not yet supported; \"
             \"use __builtin_clr() instead\");
     } else {
       /* lac ... */
       error(\"Automatic generation of DSP instructions not yet supported; \"
             \"use __builtin_lac() instead\");
     }
   }
  "
)

(define_insn "addab_error"
  [(set (match_operand: HI          0 "pic30_accumulator_operand" "=w")
        (plus:HI (match_operand:HI 1 "pic30_mode3_operand" "RS<>r")
                 (match_operand:HI 2 "pic30_mode3_operand" "RS<>r")))]
  ""
  "*{
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_addab() instead\");
   }
  "
)

(define_insn "auto_mac"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (plus:HI
          (match_operand:HI 1 "pic30_accumulator_operand" "0")
          (subreg:HI
            (mult:SI
              (sign_extend:SI
                (match_operand: HI 2 "pic30_mac_input_operand" "z"))
              (sign_extend:SI
                (match_operand: HI 3 "pic30_mac_input_operand" "z"))) 0)))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_mac() instead\");
#if 0
     if (REGNO(operands[2]) < REGNO(operands[3])) {
       return \"mac %2*%3, %0\";
     } else {
       return \"mac %3*%2, %0\";
     }
#endif
   }
  "
)

(define_insn "auto_mac1"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (plus:HI
          (subreg:HI
            (mult:SI
              (sign_extend:SI
                (match_operand: HI 1 "pic30_mac_input_operand" "z"))
              (sign_extend:SI
                (match_operand: HI 2 "pic30_mac_input_operand" "z"))) 0)
          (match_operand:HI 3 "pic30_accumulator_operand" "0")))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_mac() instead\");
   }
  "
)

(define_insn "auto_mpy"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (neg: HI
          (subreg:HI
            (mult:SI
              (sign_extend:SI
                (match_operand: HI 1 "pic30_mac_input_operand" "z"))
              (sign_extend:SI
                (match_operand: HI 2 "pic30_mac_input_operand" "z"))) 0)))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_mpy() instead\");
   }
  "
)

(define_insn "auto_msc"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (minus:HI
          (match_operand:HI 1 "pic30_accumulator_operand" "0")
          (subreg:HI
            (mult:SI
              (sign_extend:SI
                (match_operand: HI 2 "pic30_mac_input_operand" "z"))
              (sign_extend:SI
                (match_operand: HI 3 "pic30_mac_input_operand" "z"))) 0)))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_msc() instead\");
   }
  "
)

(define_insn "auto_msc1"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (minus:HI
          (subreg:HI
            (mult:SI
              (sign_extend:SI
                (match_operand: HI 1 "pic30_mac_input_operand" "z"))
              (sign_extend:SI
                (match_operand: HI 2 "pic30_mac_input_operand" "z"))) 0)
          (match_operand:HI 3 "pic30_accumulator_operand" "0")))]
  ""
  "*
   {
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_msc() instead\");
   }
  "
)

(define_insn "auto_sftacr"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (ashiftrt:HI
          (match_dup 0)
          (match_operand:HI 1 "immediate_operand"       "W")))]
  "(INTVAL(operands[1]) > 0)"
  "*
   {
     /* sftac %0, #%1 */
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_sftac() instead\");
   }
  "
)

(define_insn "auto_sftacl"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w")
        (ashift:HI 
          (match_dup 0)
          (match_operand:HI 1 "immediate_operand"       "W")))]
  "(INTVAL(operands[1]) > 0)"
  "*
   { static char buffer[20];

     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_sftac() instead\");
   }
  "
)

(define_insn "addacr_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (match_operand: HI 1 "pic30_mode3_operand" "RS<>r")
           (match_operand: HI 2 "pic30_accumulator_operand" "0")))]
  ""
  "*
   {
     /* add %1, %0 */
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_add() instead\");
   }
  "
)

(define_insn "addacr1_hi"
  [(set (match_operand: HI 0 "pic30_accumulator_operand" "=w")
        (plus:HI
           (match_operand: HI 1 "pic30_accumulator_operand" "0")
           (match_operand: HI 2 "pic30_mode3_operand" "RS<>r")))]
  ""
  "*
   {
     /* add %2, %0 */
     error(\"Automatic generation of DSP instructions not yet supported; \"
           \"use __builtin_add() instead\");
   }
  "
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; __builtin_unique_id

(define_insn "unique_id"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (unspec_volatile:HI [ (match_operand 1 "immediate_operand" "i")
                              (match_operand:HI 2 "immediate_operand" "i")
                            ] UNSEPCV_GENLABEL))]
  ""
  "*
   {  static char buffer[80];
      char *label;

      label = (char *)(INTVAL(operands[1]));
      sprintf(buffer,\".global %s\n%s:\n\tmov #%d,%%0\",
              label,label,INTVAL(operands[2]));
      return buffer;
   }
  "
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; __builtin_tblrdl, __builtin_tblrdh, __builtin_tblwtl, __builtin_tblwth

(define_insn "tblrdl"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:HI 
          [(match_operand:HI 1 "pic30_register_operand" "r")] UNSPECV_TBLRDL
        )
    )]
  ""
  "tblrdl.w [%1], %0"
)

(define_insn "tblrdlb"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:QI
          [(match_operand:HI 1 "pic30_register_operand" "r")] UNSPECV_TBLRDLB
        )
    )]
  ""
  "tblrdl.b [%1], %0"
)

(define_insn "tblrdh"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>") 
        (unspec_volatile:HI 
          [(match_operand:HI 1 "pic30_register_operand" "r")] UNSPECV_TBLRDH
        )
    )]
  ""
  "tblrdh.w [%1], %0"
)

(define_insn "tblrdhb"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:QI
          [(match_operand:HI 1 "pic30_register_operand" "r")] UNSPECV_TBLRDHB
        )
    )]
  ""
  "tblrdh.b [%1], %0"
)

(define_insn "tblwtl"
  [(unspec_volatile:HI 
     [(match_operand:HI 0 "pic30_register_operand" "r")
      (match_operand:HI 1 "pic30_mode2_operand" "rR<>")] UNSPECV_TBLWTL
   )
  ]
  ""
  "tblwtl.w %1, [%0]"
)

(define_insn "tblwtlb"
  [(unspec_volatile:QI
     [(match_operand:HI 0 "pic30_register_operand" "r")
      (match_operand:QI 1 "pic30_mode2_operand" "rR<>")] UNSPECV_TBLWTLB
   )
  ]
  ""
  "tblwtl.b %1, [%0]"
)

(define_insn "tblwth"
  [(unspec_volatile:HI  
     [(match_operand:HI 0 "pic30_register_operand" "r")
      (match_operand:HI 1 "pic30_mode2_operand" "rR<>")] UNSPECV_TBLWTH
   )
  ]
  ""
  "tblwth.w %1, [%0]"
)

(define_insn "tblwthb"
  [(unspec_volatile:QI
     [(match_operand:HI 0 "pic30_register_operand" "r")
      (match_operand:QI 1 "pic30_mode2_operand" "rR<>")] UNSPECV_TBLWTHB
   )
  ]
  ""
  "tblwth.b %1, [%0]"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; stackpush
;; These patterns are used for passing arguments on the stack.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_insn "*pushqi"
  [(set (match_operand:QI 0 "push_operand"   "=>")
        (match_operand:QI 1 "pic30_general_operand" "r"))]
  ""
  "mov %1,[w15++]"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushqi1"
  [(set (mem:HI (post_inc:HI (reg:HI SPREG)))
                (subreg:HI (match_operand:QI 0 "register_operand" "r") 0))]
  ""
  "push %0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "*pushminus1hi"
  [(set (match_operand:HI 0 "push_operand"   "=>")
	(const_int -1))]
  ""
  "setm %0"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushhi1"
  [(set (mem:HI (post_inc:HI (reg:HI SPREG)))
                (match_operand:HI 0 "register_operand" "r"))]
  "0"
  "push %0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "pushhi"
  [(set (match_operand:HI 0 "push_operand"   "=>,>,>,>")
        (match_operand:HI 1 "pic30_pushhi_operand" "r,R>,O,T"))]
  ""
  "@
   mov %1,%0
   mov %1,%0
   clr %0
   push %1"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "pushsi1"
  [(set (mem:SI (post_inc:HI (reg:HI SPREG)))
                (match_operand:SI 0 "register_operand" "r"))]
  ""
  "push.d %0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "pushsi"
  [(set (match_operand:SI 0 "push_operand"   "=>")
        (match_operand:SI 1 "pic30_register_operand" "r"))]
  ""
  "mov.d %1,[w15++]"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushdi1"
  [(set (mem:DI (post_inc:HI (reg:HI SPREG)))
                (match_operand:DI 0 "register_operand" "r"))]
  ""
  "push.d %0\;push.d %t0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "*pushdi"
  [(set (match_operand:DI 0 "push_operand"   "=>")
        (match_operand:DI 1 "pic30_register_operand" "r"))]
  ""
  "mov.d %1,[w15++]\;mov.d %t1,[w15++]"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushsf1"
  [(set (mem:SF (post_inc:HI (reg:HI SPREG)))
                (match_operand:SF 0 "register_operand" "r"))]
  ""
  "push.d %0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "*pushsf"
  [(set (match_operand:SF 0 "push_operand"   "=>")
        (match_operand:SF 1 "pic30_register_operand" "r"))]
  ""
  "mov.d %1,[w15++]"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushdf1"
  [(set (mem:DF (post_inc:HI (reg:HI SPREG)))
                (match_operand:DF 0 "register_operand" "r"))]
  "0"
  "push.d %0\;push.d %t0"
  [ 
    (set_attr "cc" "unchanged")
  ]
)
 
(define_insn "*pushdf"
  [(set (match_operand:DF 0 "push_operand"   "=>")
        (match_operand:DF 1 "pic30_register_operand" "r"))]
  ""
  "mov.d %1,[w15++]\;mov.d %t1,[w15++]"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def")
  ]
)

(define_insn "pushshadow"
  [(unspec [(const_int 0)] UNSPEC_PUSHSHADOW)
   (use (reg:HI 0))
   (use (reg:HI 1))
   (use (reg:HI 2))
   (use (reg:HI 3))
  ]
  ""
  "push.s")

(define_insn "popshadow"
  [(unspec [(const_int 0)] UNSPEC_POPSHADOW)
   (clobber (reg:HI 0))
   (clobber (reg:HI 1))
   (clobber (reg:HI 2))
   (clobber (reg:HI 3))
  ]
  ""
  "pop.s"
  [(set_attr "cc" "clobber")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; stackpop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "popqi"
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "pop_operand"       "<"))]
  ""
  "mov %1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "use")
  ]
)

(define_insn "pophi"
  [(set (match_operand:HI 0 "pic30_general_operand" "=r,<,O,T")
        (match_operand:HI 1 "pop_operand"    " <,<,<,<"))]
  ""
  "@
   mov %1,%0
   mov %1,%0
   clr %0
   pop %0"
  [
   (set_attr "cc" "unchanged")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "pophi_unspec"
  [(set (unspec:HI [(match_operand:HI 0 "pic30_general_operand" "=r<,T")]
                   UNSPEC_POPHI)
        (mem:HI (pre_dec:HI (reg:HI SPREG))))
   (clobber (match_dup 0))
  ]
  ""
  "@
   mov [--w15],%0
   pop %0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "defuse,use")
  ]
)

(define_insn "popsi"
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (match_operand:SI 1 "pop_operand"       "<"))
  ]
  ""
  "mov.d %1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "use")
  ]
)

(define_insn "popdi"
  [(set (match_operand:DI 0 "pic30_register_operand" "=r")
        (match_operand:DI 1 "pop_operand"       "<"))]
  ""
  "mov.d %1,%t0\;mov.d %1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "use")
  ]
)

(define_insn "popsf"
  [(set (match_operand:SF 0 "pic30_register_operand" "=r")
        (match_operand:SF 1 "pop_operand"       "<"))]
  ""
  "mov.d %1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "use")
  ]
)
   
(define_insn "popdf"
  [(set (match_operand:DF 0 "pic30_register_operand" "=r")
        (match_operand:DF 1 "pop_operand"       "<"))]
  ""
  "mov.d %1,%t0\;mov.d %1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "use")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; stackcheck
;; If STACK_CHECK_BUILTIN is zero, and you define a named pattern called
;; check_stack in your 'md' file, GCC will call that pattern with one
;; argument which is the address to compare the stack value against.
;; You must arrange for this pattern to report an error if the stack
;; pointer is out of range.
;; (define_insn "check_stack"
;;   [(match_operand 0 "pic30_register_operand" "r")
;;    (clobber (match_scratch:HI 1 "=r"))]
;;   ""
;;   "mov .BOS,%1\;cpsge %0,%1\;reset")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Test operations.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "tstqi_DATA"
  [(set (cc0)
        (match_operand:QI 0 "pic30_near_mode2_operand" "U,r,R<>"))]
  ""
  "cp0.b %0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,etc,use")
  ]
)

(define_insn "tstqi_APSV"
  [(set (cc0)
        (match_operand:QI 0 "pic30_near_mode2_APSV_operand" "U,r,R<>"))]
  ""
  "cp0.b %0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,etc,use")
  ]
)

(define_expand "tstqi"
  [(set (cc0)
        (match_operand:QI 0 "pic30_near_mode2_APSV_operand" "U,r,R<>"))]
  ""
  "
{
  if (pic30_near_mode2_operand(operands[0],GET_MODE(operands[0])))
    emit(gen_tstqi_DATA(operands[0]));
  else
    emit(gen_tstqi_APSV(operands[0]));
  DONE;
}")


; a subset of tstqi
;(define_insn "*tstqi_sfr"
;  [(set (cc0)
;        (match_operand:QI 0 "pic30_near_operand" "U"))]
;  ""
;  "cp0.b %0"
;  [(set_attr "cc" "set")])

(define_insn "tsthi_DATA"
  [(set (cc0)
        (match_operand:HI 0 "pic30_near_mode2_operand" "U,r,R<>"))]
  ""
  "cp0 %0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,etc,use")
  ]
)

(define_insn "tsthi_APSV"
  [(set (cc0)
        (match_operand:HI 0 "pic30_near_mode2_APSV_operand" "U,r,R<>"))]
  ""
  "cp0 %0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,etc,use")
  ]
)

(define_expand "tsthi"
  [(set (cc0)
        (match_operand:HI 0 "pic30_near_mode2_APSV_operand" "U,r,R<>"))]
  ""
"
{
  if (pic30_near_mode2_operand(operands[0],GET_MODE(operands[0])))
    emit(gen_tsthi_DATA(operands[0]));
  else
    emit(gen_tsthi_APSV(operands[0]));
  DONE;
}")

; a subset of tsthi
;(define_insn "*tsthi_sfr"
;  [(set (cc0)
;        (match_operand:HI 0 "pic30_near_operand" "U"))]
;  ""
;  "cp0 %0"
;  [(set_attr "cc" "set")])


(define_insn "tstsi"
  [(set (cc0)
        (match_operand:SI 0 "pic30_register_operand" "r"))]
  ""
  "sub %0,#0,[w15]\;subb %d0,#0,[w15]"
  [(set_attr "cc" "set")])

(define_insn "tstdi"
  [(set (cc0)
        (match_operand:DI 0 "pic30_register_operand" "r"))]
  ""
  "sub %0,#0,[w15]\;subb %d0,#0,[w15]\;subb %t0,#0,[w15]\;subb %q0,#0,[w15]"
  [(set_attr "cc" "set")])

; With dsPIC30 floats, testing the most significant word does not suffice,
; since -0.0 == 0.0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Block moves.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Argument 0 is the destination
;; Argument 1 is the source
;; Argument 2 is the length
;; Argument 3 is the alignment
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "movstrhi"
  [(parallel [(set (match_operand:BLK 0 "memory_operand" "")
		   (match_operand:BLK 1 "memory_operand" ""))
	      (use (match_operand:HI 2 "const_int_operand" ""))
	      (use (match_operand:HI 3 "const_int_operand" ""))
	      (clobber (match_dup 4))
	      (clobber (match_dup 5))
   	      (clobber (reg:HI RCOUNT))
  ])]
  ""
  "
{
	rtx addr0;
	rtx addr1;
	int n;

	if (GET_CODE (operands[2]) != CONST_INT)
		FAIL;
	n = INTVAL(operands[2]);
	if (n < 1)
		FAIL;

	switch (INTVAL(operands[3]))
	{
	case 1:
		if (n > 16383)
			FAIL;
		break;
	default:
		if ((n > (16383*2)) || (n & 1))
			FAIL;
	}

	addr0 = copy_to_mode_reg(Pmode, XEXP (operands[0], 0));
	addr1 = copy_to_mode_reg(Pmode, XEXP (operands[1], 0));

	operands[4] = addr0;
	operands[5] = addr1;
	operands[0] = change_address(operands[0], VOIDmode, addr0);
	operands[1] = change_address(operands[1], VOIDmode, addr1);
}")

;
;  This pattern should restore the src/ dest ptrs before finishing
;
(define_insn "movmemhi"
  [(set (match_operand:BLK 0 "memory_operand" "=R,m,R,m,R,m,R,m")
        (match_operand:BLK 1 "memory_operand" "R,R,m,m,R,R,m,m"))
   (use (match_operand:HI 2 "immediate_operand" "J,J,J,J,i,i,i,i"))
   (use (match_operand:HI 3 "const_int_operand" ""))
   (clobber (reg:HI RCOUNT))
   (clobber (match_scratch:HI 4  "=X,&r,&r,&r,&r,&r,&r,&r"))
   (clobber (match_scratch:HI 5  "=X,X,X,&r,X,&r,&r,&r"))
  ]
  ""
  "*
   { /* my calculation says maximum string size is ~120 */
     static char buffer[160];
     char *c = buffer;
     char *op0 = \"%r0\";
     char *op1 = \"%r1\";
     int restore_with_sub_0=0;
     int restore_with_sub_1=0;
     char *sub_value = 0;  /* != 0 => use register */
     int repeat_repeat;
     int repeat_count;

     repeat_count = INTVAL(operands[2]);
     switch (which_alternative) {
     default: break;
     case 0:  /* both registers */
              /* literal <= 10 bits */
              restore_with_sub_0 = 1;
              restore_with_sub_1 = 1;
              break;
     case 1:  /* op0 is memory, take its address */
              /* literal <= 10 bits */
              c += sprintf(c,\"mov #%%0,%%4\;\");
              op0 = \"%4\";
              restore_with_sub_1 = 1;
              break;
     case 2:  /* op1 is memory, take its address */
              /* literal <= 10 bits */
              c += sprintf(c,\"mov #%%1,%%4\;\");
              restore_with_sub_0 = 1;
              op1 = \"%4\";
              break;
     case 3:  /* op0 and op1 are memory, take their addresses */
              c += sprintf(c,\"mov #%%0,%%4\;\");
              op0 = \"%4\";
              c += sprintf(c,\"mov #%%1,%%5\;\");
              op1 = \"%5\";
              break;
     case 4:  /* op0 and op1 are registers */
              /* literal > 10 bits, save repeat count in reg */
              restore_with_sub_0 = 1;
              restore_with_sub_1 = 1;
              c += sprintf(c,\"mov #%%2,%%4\;\");
              sub_value =\"%4\";
              break;
     case 5:  /* op0 is memory, take its address */
              /* literal > 10 bits, save repeat count in reg */
              c += sprintf(c,\"mov #%%0,%%4\;\");
              op0 = \"%4\";
              c += sprintf(c,\"mov #%%2,%%5\;\");
              sub_value = \"%5\";
              restore_with_sub_1 = 1;
              break;
     case 6:  /* op1 is memory, take its address */
              /* literal > 10 bits, save repeat count in reg */
              c += sprintf(c,\"mov #%%1,%%4\;\");
              restore_with_sub_0 = 1;
              op1 = \"%4\";
              c += sprintf(c,\"mov #%%2,%%5\;\");
              sub_value = \"%5\";
              break;
     case 7:  /* op0 and op1 are memory, take their addresses */
              c += sprintf(c,\"mov #%%0,%%4\;\");
              op0 = \"%4\";
              c += sprintf(c,\"mov #%%1,%%5\;\");
              op1 = \"%5\";
              break;
     }
     
     switch (INTVAL(operands[3]))
     {
       case 1:
         /* 
         ** Byte operation
         */
         repeat_repeat =  repeat_count - 16383;
         if (repeat_repeat < 0) repeat_repeat = 0;
	 if (repeat_repeat) {
           c += sprintf(c,\"repeat #16383-1\;mov.b [%s++],[%s++]\;\", op1, op0);
           c += sprintf(c,\"repeat #%d-1\;mov.b [%s++],[%s++]\", repeat_count, 
                                                            op1, op0);
         } else c += sprintf(c,\"repeat #%d-1\;mov.b [%s++],[%s++]\", 
                             repeat_count, op1, op0);
         break;
     default: {
         int repeat_remainder;
	 /* 
	 ** Word operation
	 */
         /* repeat count is expressed in bytes */
         repeat_remainder = (repeat_count & 1);
         repeat_count = repeat_count / 2;
         repeat_repeat =  repeat_count - 16383;
         if (repeat_repeat < 0) repeat_repeat = 0;
	 if (repeat_repeat) {
           c += sprintf(c,\"repeat #16383-1\;mov [%s++],[%s++]\;\", op1, op0);
           c += sprintf(c,\"repeat #%d-1\;mov [%s++],[%s++]\", 
                        repeat_count, op1, op0);
         } else c += sprintf(c,\"repeat #%d-1\;mov [%s++],[%s++]\", 
                             repeat_count, op1, op0);
         if (repeat_remainder) 
           c += sprintf(c,\"\;mov.b [%s++],[%s++]\", op1,op0);
         break;
       }
     }
     if (restore_with_sub_0) {
       if (sub_value) {
         c += sprintf(c,\"\;sub %s, %s, %s\", op0, sub_value, op0);
       } else c += sprintf(c,\"\;sub #%%2, %s\", op0);
     }
     if (restore_with_sub_1) {
       if (sub_value) {
         c += sprintf(c,\"\;sub %s, %s, %s\", op1, sub_value, op1);
       } else c += sprintf(c,\"\;sub #%%2, %s\", op1);
     }
     return buffer;
   } ")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Block clear.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Argument 0 is the destination
;; Argument 1 is the length
;; Argument 2 is the alignment
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "clrstrhi"
  [(parallel [(set (match_operand:BLK 0 "memory_operand" "")
		   (const_int 0))
	      (use (match_operand:HI 1 "const_int_operand" ""))
	      (use (match_operand:HI 2 "const_int_operand" ""))
	      (clobber (match_dup 3))
	      (clobber (reg:HI RCOUNT))
  ])]
  ""
  "
{
	rtx addr0;
	int n;

	if (GET_CODE (operands[1]) != CONST_INT)
		FAIL;
	n = INTVAL(operands[1]);
	if (n < 1)
		FAIL;

	switch (INTVAL(operands[2]))
	{
	case 1:
		if (n > 16383)
			FAIL;
		break;
	default:
		if ((n > (16383/2)) || (n & 1))
			FAIL;
	}
	addr0 = copy_to_mode_reg (Pmode, XEXP (operands[0], 0));

	operands[3] = addr0;

	operands[0] = gen_rtx_MEM (BLKmode, addr0);
}")

;; Block clear.
;; Argument 0 is the destination
;; Argument 1 is the length
;; Argument 2 is the alignment

(define_insn "*bzero"
  [(set (mem:BLK (match_operand:HI 0 "pic30_register_operand" "r"))
	(const_int 0))
   (use (match_operand:HI 1 "const_int_operand" ""))
   (use (match_operand:HI 2 "immediate_operand" ""))
   (clobber (match_dup 0))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
	switch (INTVAL(operands[2]))
	{
	case 1:
		return \"repeat #%1-1\;clr.b [%0++]\";
	default:
		return \"repeat #%1/2-1\;clr [%0++]\";
	}
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; compare instructions.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "any_branch"
  [(set (pc)
       (if_then_else (match_operator 0 "comparison_operator"
                      [(cc0) (const_int 0)])
                     (label_ref (match_operand 1 "" ""))
                     (pc)))]
  ""
  "* 
   return pic30_conditional_branch(GET_CODE(operands[0]), operands[1]);
  "
  [(set_attr "cc" "unchanged")]
)

(define_mode_iterator CMPMODES [QI HI SI DI])

(define_expand "cbranch<mode>4"
 [(set (cc0)
       (compare (match_operand:CMPMODES 1 "pic30_mode1PN_operand" "")
                (match_operand:CMPMODES 2 "pic30_mode1PN_operand" "")))
  (set (pc)
       (if_then_else (match_operator 0 "comparison_operator"
                      [(cc0) (const_int 0)])
                     (label_ref (match_operand 3 "" ""))
                     (pc)))]
  ""
{ /* The optimizer will coalesce this if possible, right? */
  rtx reg_1 = force_reg (GET_MODE(operands[1]), operands[1]);

  emit(
    gen_cmp<mode>(reg_1,operands[2])
  );
  emit(
    gen_any_branch(operands[0], operands[3])
  );
  DONE;
}
) 

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "*cmpqi_imm"
  [(set (cc0)
        (compare (match_operand:QI 0 "pic30_register_operand" "r")
                 (match_operand:QI 1 "pic30_M_operand"  "M")))
   (clobber (match_dup 0))]
  ""
  "add.b #%J1,%0"
  [(set_attr "cc" "set")])

(define_insn "cmpqi3_sfr0"
  [(set (cc0)
        (compare:QI (match_operand:QI 0 "pic30_reg_or_near_operand" "U,r")
                    (match_operand:QI 1 "pic30_wreg_operand" "a,r")))]
  ""
  "@
   cp.b %0
   sub.b %0,%1,[w15]"
  [
    (set_attr "cc" "set,set")
    (set_attr "type" "etc,etc")
  ]
)

(define_insn "cmpqi3_2sfr"
  [(set (cc0)
        (compare:QI 
           (match_operand:QI 0 "pic30_wreg_or_near_operand" "?r,U, U,  r")
           (match_operand:QI 1 "pic30_wreg_or_near_operand" "U, a, ?r, r")))
   (clobber (match_scratch:HI 2 "=r,X,r,X"))]
  ""
  "@
   mov #%1,%2\;sub.b %0,[%2],[w15]
   cp.b %0
   mov #%0,%2\;sub.b %0,[%2],[w15]
   cp.b %0,%1"
  [(set_attr "cc" "set")])

(define_expand "cmpqi"
 [(set (cc0)
       (compare (match_operand:QI 0 "pic30_near_mode1PN_operand" "")
                (match_operand:QI 1 "pic30_near_mode1PN_APSV_operand" "")))
  (match_dup 2)]
  ""
  "{
      rtx (*gen)(rtx,rtx);

      if (pic30_near_mode1PN_operand(operands[1],GET_MODE(operands[1]))) {
        gen = gen_cmpqi_normal_DATA;
      } else {
        gen = gen_cmpqi_normal_APSV;
      }
      if (pic30_near_operand(operands[0],QImode) &&
          pic30_near_operand(operands[1],QImode)) {
  
        operands[2] = gen_reg_rtx(QImode);
        
        emit_insn(gen_movqi_gen_DATA(operands[2],operands[1]));
        emit_insn(gen_cmpqi3_sfr0(operands[0],operands[2]));
      } else if (pic30_near_operand(operands[1],QImode)) {
        if (pic30_wreg_operand(operands[0],QImode))
          emit_insn(gen_cmpqi3_2sfr(operands[0],operands[1]));
        else { 
          rtx pop; 

          operands[2] = gen_reg_rtx(HImode);
          pop = gen_rtx_MEM(QImode, operands[2]);
          emit_insn(gen_movhi_address(operands[2],  XEXP(operands[1],0)));
          emit_insn(gen(operands[0],pop));
        }
      }
      else if (pic30_near_operand(operands[0],QImode)) {
        if (pic30_wreg_operand(operands[1],QImode))
          emit_insn(gen_cmpqi3_sfr0(operands[0],operands[1]));
        else {
          rtx pop;

          operands[2] = gen_reg_rtx(HImode);
          pop = gen_rtx_MEM(QImode, operands[2]);
          emit_insn(gen_movhi_address(operands[2], XEXP(operands[0],0)));
          emit_insn(gen(pop,operands[1]));
        }
      } else {
        emit_insn(gen(operands[0],operands[1]));
      }
      DONE;
   }
  "
)

(define_insn "cmpqi_normal_DATA"
  [(set (cc0)
        (compare (match_operand:QI 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:QI 1 "pic30_mode1PN_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub.b %0,%1,[w15]
   sub.b %0,%1,[w15]
   subr.b %1,%0,[w15]
   sub.b %0,#%1,[w15]
   add.b %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

(define_insn "cmpqi_normal_APSV"
  [(set (cc0)
        (compare (match_operand:QI 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:QI 1 "pic30_mode1PN_APSV_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub.b %0,%1,[w15]
   sub.b %0,%1,[w15]
   subr.b %1,%0,[w15]
   sub.b %0,#%1,[w15]
   add.b %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

;;;;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;;;;

(define_insn "cmphi_imm"
  [(set (cc0)
        (compare (match_operand:HI 0 "pic30_register_operand" "r")
                 (match_operand:HI 1 "pic30_M_operand"  "M")))
   (clobber (match_dup 0))]
  ""
  "add #%J1,%0"
  [(set_attr "cc" "set")])

(define_insn "cmphi_sfr0"
  [(set (cc0)
        (compare:HI (match_operand:HI 0 "pic30_reg_or_near_operand" "U,r")
                    (match_operand:HI 1 "pic30_register_operand" "a,r")))]
  ""
  "@
   cp %0
   sub %0,%1,[w15]"
  [(set_attr "cc" "set")])

(define_insn "cmphi_DATA"
  [(set (cc0)
        (compare (match_operand:HI 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:HI 1 "pic30_mode1PN_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub %0,%1,[w15]
   sub %0,%1,[w15]
   subr %1,%0,[w15]
   sub %0,#%1,[w15]
   add %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

(define_insn "cmphi_APSV"
  [(set (cc0)
        (compare (match_operand:HI 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:HI 1 "pic30_mode1PN_APSV_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub %0,%1,[w15]
   sub %0,%1,[w15]
   subr %1,%0,[w15]
   sub %0,#%1,[w15]
   add %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

(define_expand "cmphi"
  [(set (cc0)
        (compare (match_operand:HI 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:HI 1 "pic30_mode1PN_APSV_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "
{
  if (pic30_mode1PN_operand(operands[1],GET_MODE(operands[1])))
    emit_insn(gen_cmphi_DATA(operands[0],operands[1]));
  else
    emit_insn(gen_cmphi_APSV(operands[0],operands[1]));
  DONE;
}")

(define_insn "*cmpp16apsv_imm"
  [(set (cc0)
        (compare (match_operand:P16APSV 0 "pic30_register_operand" "r")
                 (match_operand:P16APSV 1 "pic30_M_operand"  "M")))
   (clobber (match_dup 0))]
  ""
  "add #%J1,%0"
  [(set_attr "cc" "set")])

(define_insn "*cmpp16apasv_sfr0"
  [(set (cc0)
        (compare:P16APSV
          (match_operand:P16APSV 0 "pic30_reg_or_near_operand" "U,r")
          (match_operand:P16APSV 1 "pic30_register_operand" "a,r")))]
  ""
  "@
   cp %0
   sub %0,%1,[w15]"
  [(set_attr "cc" "set")])

(define_insn "cmpp16apsv_DATA"
  [(set (cc0)
        (compare (match_operand:P16APSV 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:P16APSV 1 "pic30_mode1PN_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub %0,%1,[w15]
   sub %0,%1,[w15]
   subr %1,%0,[w15]
   sub %0,#%1,[w15]
   add %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

(define_insn "cmpp16apsv_APSV"
  [(set (cc0)
        (compare (match_operand:P16APSV 0 "pic30_mode2_operand"
                                "r,r,  R<>,r,r")
                 (match_operand:P16APSV 1 "pic30_mode1PN_APSV_operand"
                                "r,R<>,r,  P,N")))]
  ""
  "@
   sub %0,%1,[w15]
   sub %0,%1,[w15]
   subr %1,%0,[w15]
   sub %0,#%1,[w15]
   add %0,#%J1,[w15]"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,use,etc,etc")
  ]
)

(define_expand "cmpp16apsv"
  [(set (cc0)
        (compare (match_operand:P16APSV 0 "pic30_mode1PN_operand"
                                "r,r,  R<>,r,P,r,N")
                 (match_operand:P16APSV 1 "pic30_mode1PN_APSV_operand"
                                "r,R<>,r,  P,r,N,r")))]
  ""
  "
{
  if (pic30_mode1PN_operand(operands[1],GET_MODE(operands[1])))
    emit_insn(gen_cmpp16apsv_DATA(operands[0],operands[1]));
  else
    emit_insn(gen_cmpp16apsv_APSV(operands[0],operands[1]));
  DONE;
}")

;;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;;
(define_insn "cmpsi_DATA"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_mode2mres_operand" "r,r,R,r,>")
                 (match_operand:SI 1 "pic30_mode2mres_operand" "r,R,r,>,r")))]
  ""
  "*
{
  static char *patterns[] = {
     \"sub %0,%1,[w15]\;subb %d0,%d1,[w15]\",
     \"sub %0,%I1,[w15]\;subb %d0,%D1,[w15]\",
     \"subr %1,%I0,[w15]\;subbr %d1,%D0,[w15]\",
     \"sub %0,%1,[w15]\;subb %d0,%1,[w15]\",
     \"subr %1,%0,[w15]\;subbr %d1,%0,[w15]\",
  0 };

  return patterns[which_alternative];
}"
  [(set_attr "cc" "set")])


(define_insn "cmpsi_errata_APSV"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_mode2mres_operand" "r,r,R,r,>")
                 (match_operand:SI 1 "pic30_mode2mres_APSV_operand" "r,R,r,>,r")))
   (clobber (match_scratch:HI 2                               "=X,&r,&r,&r,&r"))
  ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
  static char *patterns[] = {
     \"sub %0,%1,[w15]\;subb %d0,%d1,[w15]\",
     \"sub %0,%I1,[w15]\;mov %D1,%2\;subb %d0,%2,[w15]\",
     \"subr %1,%I0,[w15]\;mov %D0,%2\;subbr %d1,%2,[w15]\",
     \"sub %0,%1,[w15]\;mov %1,%2\;subb %d0,%2,[w15]\",
     \"subr %0,%1,[w15]\;mov %1,%2\;subbr %d0,%2,[w15]\",
  0 };

  return patterns[which_alternative];
}"
  [(set_attr "cc" "set")])

(define_insn "cmpsi_noerrata_APSV"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_mode2mres_operand" "r,r,R,r,>")
                 (match_operand:SI 1 "pic30_mode2mres_APSV_operand" "r,R,r,>,r")))]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
  static char *patterns[] = {
     \"sub %0,%1,[w15]\;subb %d0,%d1,[w15]\",
     \"sub %0,%I1,[w15]\;subb %d0,%D1,[w15]\",
     \"subr %1,%I0,[w15]\;subbr %d1,%D0,[w15]\",
     \"sub %0,%1,[w15]\;subb %d0,%1,[w15]\",
     \"subr %0,%1,[w15]\;subbr %d0,%1,[w15]\",
  0 };

  return patterns[which_alternative];
}"
  [(set_attr "cc" "set")])

(define_insn "cmpsi_zero"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_register_operand" "r")
                 (match_operand:SI 1 "pic30_O_operand"        "O")))]
  ""
  "sub %0,#0,[w15]\;subb %d0,#0,[w15]"
   
  [(set_attr "cc" "clobber")])

(define_insn "cmpsi_immNP"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_register_operand"  "r,r,r")
                 (match_operand:SI 1 "immediate_operand" "P,N,i")))]
  "(((-31 <= INTVAL(operands[1])) && (INTVAL(operands[1]) <= 31)) ||
    ((0xFFE1 <= INTVAL(operands[1])) && (INTVAL(operands[1]) <= 0xFFFF))) &&
   (INTVAL(operands[1]) != 0)"
  "@
   sub %0,#%1,[w15]\;subb %d0,#0,[w15]
   add %0,#%J1,[w15]\;addc %d0,#0,[w15]
   add %0,#%j1,[w15]\;subb %d0,#0,[w15]"
  [(set_attr "cc" "set")])

(define_insn "cmpsi_imm"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_register_operand"  "r")
                 (match_operand:SI 1 "immediate_operand" "i")))
	         (clobber (match_scratch:HI 2           "=&r"))]
  "((1<INTVAL(operands[1])) && (INTVAL(operands[1])<65536))"
  "mov #%1,%2\;sub %0,%2,[w15]\;subb %d0,#0,[w15]"
  [(set_attr "cc" "clobber")])

(define_insn "*cmpsihi3"
  [(set (cc0)
        (compare (match_operand:SI   0 "pic30_register_operand" "r")
                 (zero_extend:SI 
                   (match_operand:HI 1 "pic30_register_operand" "r"))))]
  ""
  "sub %0,%1,[w15]\;subb %d0,#0,[w15]"
  [(set_attr "cc" "set")])

(define_expand "cmpsi"
  [(set (cc0)
        (compare (match_operand:SI 0 "pic30_mode2mres_operand" "")
                 (match_operand:SI 1 "pic30_mode2mres_APSV_operand" "")))]
  ""
  "
{
  if (pic30_mode2mres_APSV_operand(operands[1],GET_MODE(operands[1]))) {
    if (pic30_errata_mask & psv_errata) {
      emit(
        gen_cmpsi_errata_APSV(operands[0],operands[1])
      );
    } else {
      emit(
        gen_cmpsi_noerrata_APSV(operands[0],operands[1])
      );
    }
  } else if (pic30_mode2mres_operand(operands[1], GET_MODE(operands[1]))) {
    emit(
      gen_cmpsi_DATA(operands[0],operands[1])
    );
  } else if (immediate_operand(operands[1],VOIDmode)) {
    rtx reg_0 = force_reg(GET_MODE(operands[0]),operands[0]);
    if (INTVAL(operands[1]) == 0) {
      emit(
        gen_cmpsi_zero(reg_0, operands[1])
      );
    } else {
      rtx reg = force_reg(GET_MODE(operands[0]),operands[1]);
      emit( 
        gen_cmpsi_DATA(reg_0, reg)
      );
    }
  }
  DONE;
}")


;;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;;

(define_insn "cmpdi_gen"
  [(set (cc0)
        (compare (match_operand:DI 0 "pic30_register_operand"    "r,r")
                 (match_operand:DI 1 "pic30_reg_or_zero_operand" "r,O")))]
  ""
  "@
   cp %0,%1\;cpb %d0,%d1\;cpb %t0,%t1\;cpb %q0,%q1
   sub %0,#0,[w15]\;subb %d0,#0,[w15]\;subb %t0,#0,[w15]\;subb %q0,#0,[w15]"
  [(set_attr "cc" "set")])

(define_expand "cmpdi"
  [(set (cc0)
        (compare (match_operand:DI 0 "general_operand" "")
                 (match_operand:DI 1 "general_operand" "")))]
  ""
  "
{ rtx op0 = operands[0];
  rtx op1 = operands[1];

  if (!pic30_register_operand(operands[0], DImode))
    op0 = force_reg(DImode, operands[0]);
  if (!pic30_reg_or_zero_operand(operands[1], DImode))
    op1 = force_reg(DImode, operands[1]);
  emit(
    gen_cmpdi_gen(op0, op1)
  );
  DONE;
}"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; truncation instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "truncsip24psv2"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=rR<>")
        (truncate:P24PSV 
          (match_operand:SI 1 "pic30_mode2_operand" " rR<>")))]
  ""
  "mov.d %1,%0"
)

(define_insn "truncsip24prog2"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=rR<>")
        (truncate:P24PROG 
          (match_operand:SI 1 "pic30_mode2_operand" " rR<>")))]
  ""
  "mov.d %1,%0"
)

(define_insn "truncsip32eds2"
  [(set (match_operand:P32EDS 0 "pic30_register_operand"   "=r,r")
        (truncate:P32EDS
          (match_operand:SI 1 "pic30_register_operand"   "r,0")))]
  ""
  "@
   rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15
   rlc %1,[w15]\;rlc %d1,%d0\;bclr %0,#15"
)

(define_insn "truncsip32peds2"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand"   "=r")
        (truncate:P32PEDS
          (match_operand:SI 1 "pic30_register_operand"   "r")))]
  ""
  "rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15"
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; zero extension instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "zero_extendqihi2_DATA"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (zero_extend:HI (match_operand:QI 1 "pic30_mode2_operand" "r")) )]
  ""
  "ze %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendqihi2_APSV"
  [(set (match_operand:HI 0 "pic30_register_operand"                   "=r")
        (zero_extend:HI (match_operand:QI 1 "pic30_mode2_APSV_operand" "r")))]
  ""
  "ze %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_expand "zero_extendqihi2"
  [(set (match_operand:HI 0 "pic30_register_operand"                   "=r,r")
        (zero_extend:HI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,R<>")) )]
  ""
  "
{
  if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
    emit(gen_zero_extendqihi2_DATA(operands[0],operands[1]));
  else {
    rtx from = operands[1];

    if (!pic30_register_operand(from, QImode)) {
      from = gen_reg_rtx(QImode);
      emit_move_insn(from, operands[1]);
    }
    emit(gen_zero_extendqihi2_APSV(operands[0],from));
  }
  DONE;
}")

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
        (zero_extend:SI (match_operand:QI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
	return(	\"ze %1,%0\;\"
		\"mov #0,%d0\");
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendqidi2"
  [(set (match_operand:DI 0 "pic30_register_operand"                "=r")
        (zero_extend:DI (match_operand:QI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
	return(	\"ze %1,%0\;\"
		\"mov #0,%d0\;\"
		\"mov #0,%t0\;\"
		\"mov #0,%q0\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendhiP24PSV2"
  [(set (match_operand:P24PSV 0 "pic30_register_operand"                  "=r")
        (zero_extend:P24PSV (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
  int idDst;

  if (REGNO(operands[0]) == REGNO(operands[1]))
  {
    return \"mov #0,%d0\";
  }
  else
  {
    idDst = REGNO(operands[0]);
    if (idDst & 1)
      return \"mov %1,%0\;mov #0,%d0\";
    else
      return \"mul.uu %1,#1,%0\";
  }
}"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendhip32eds2"
  [(set (match_operand:P32EDS 0 "pic30_register_operand"      "=r,r")
        (zero_extend:P32EDS 
          (match_operand:HI 1 "pic30_reg_or_symbolic_address" "r ,q")))]
  ""
  "@
   lsr %1,#15,%d0\;mov %1,%0\;bclr %0,#15
   mov #edsoffset(%1), %0\;mov #edspage(%1), %d0"
)

(define_insn "zero_extendhip32peds2"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand"     "=r,r")
        (zero_extend:P32PEDS
          (match_operand:HI 1 "pic30_reg_or_symbolic_address" "r ,q")))]
  ""
  "@
   lsr %1,#15,%d0\;mov %1,%0\;bclr %0,#15
   mov #edsoffset(%1), %0\;mov #edspage(%1), %d0"
)

(define_insn "zero_extendsip32eds2"
  [(set (match_operand:P32EDS 0 "pic30_register_operand"   "=r,r")
        (zero_extend:P32EDS
          (match_operand:SI 1 "pic30_register_operand"   "r,0")))]
  ""
  "@
   rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15
   rlc %1,[w15]\;rlc %d1,%d0\;bclr %0,#15"
)

(define_insn "zero_extendsip32peds2"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand"   "=r")
        (zero_extend:P32PEDS
          (match_operand:SI 1 "pic30_register_operand"   "r")))]
  ""
  "rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15"
)

(define_insn "extendsip32eds2"
  [(set (match_operand:P32EDS 0 "pic30_register_operand"   "=r,r")
        (sign_extend:P32EDS
          (match_operand:SI 1 "pic30_register_operand"   "r,0")))]
  ""
  "@
   rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15
   rlc %1,[w15]\;rlc %d1,%d0\;bclr %0,#15"
)

(define_insn "extendsip32peds2"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand"   "=r")
        (sign_extend:P32PEDS
          (match_operand:SI 1 "pic30_register_operand"   "r")))]
  ""
  "rlc %1,[w15]\;rlc %d1,%d0\;mov %1,%0\;bclr %0,#15"
)


(define_insn "zero_extendhisi2"
  [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
        (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
  int idDst;

  if (REGNO(operands[0]) == REGNO(operands[1]))
  {
    return \"mov #0,%d0\";
  }
  else
  {
    idDst = REGNO(operands[0]);
    if (idDst & 1)
      return \"mov %1,%0\;mov #0,%d0\";
    else
      return \"mul.uu %1,#1,%0\";
  }
}"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendhidi2"
  [(set (match_operand:DI 0 "pic30_register_operand"                "=r")
        (zero_extend:DI (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
	if (REGNO(operands[0]) == REGNO(operands[1]))
	{
		return(	\"mov #0,%d0\;\"
			\"mov #0,%t0\;\"
			\"mov #0,%q0\");
	}
	else
	{
		return(	\"mov %1,%0\;\"
			\"mov #0,%d0\;\"
			\"mov #0,%t0\;\"
			\"mov #0,%q0\");
	}
}"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;;
;; nop extentsions because thse modes are the same size
;;
;; hi

(define_insn "zero_extendp16apsvhi2"
  [(set (match_operand:HI       0 "register_operand" "=r")
        (zero_extend:HI 
          (match_operand:P16APSV 1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendhip16apsv2"
  [(set (match_operand:P16APSV 0 "register_operand" "=r")
        (zero_extend:P16APSV 
          (match_operand:HI    1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

;; si

(define_insn "zero_extendp24psvsi2"
  [(set (match_operand:SI       0 "register_operand" "=r")
        (zero_extend:SI 
          (match_operand:P24PSV 1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendp24progsi2"
  [(set (match_operand:SI        0 "register_operand" "=r")
        (zero_extend:SI 
          (match_operand:P24PROG 1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendp32extsi2"
  [(set (match_operand:SI       0 "register_operand" "=r")
        (zero_extend:SI 
          (match_operand:P32EXT 1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "extendp32extsi2"
  [(set (match_operand:SI       0 "register_operand" "=r")
        (sign_extend:SI 
          (match_operand:P32EXT 1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendsip24psv2"
  [(set (match_operand:P24PSV   0 "register_operand" "=r")
        (zero_extend:P24PSV 
          (match_operand:SI     1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendsip24prog2"
  [(set (match_operand:P24PROG   0 "register_operand" "=r")
        (zero_extend:P24PROG 
          (match_operand:SI      1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

(define_insn "zero_extendsip32ext2"
  [(set (match_operand:P32EXT   0 "register_operand" "=r")
        (zero_extend:P32EXT 
          (match_operand:SI     1 "register_operand" " 0")))]
  ""
  "; nop %1,%0"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; sign extension instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "extendqihi2_DATA"
  [(set (match_operand:HI 0 "pic30_register_operand"                   "=r,r")
        (sign_extend:HI (match_operand:QI 1 "pic30_mode2_operand" "r,R<>"))
   )
  ]
  ""
  "se %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "extendqihi2_APSV"
  [(set (match_operand:HI 0 "pic30_register_operand"                   "=r,r")
        (sign_extend:HI (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,R<>"))
   )
  ]
  ""
  "se %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "extendqihi2"
 [(set (match_operand:HI 0 "pic30_register_operand"                   "=r,r")
        (sign_extend:HI (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,R<>"))
   )
  ]
  ""
  "
{
  if (pic30_mode2_operand(operands[1], GET_MODE(operands[1]))) 
    emit(gen_extendqihi2_DATA(operands[0], operands[1]));
  else
    emit(gen_extendqihi2_APSV(operands[0], operands[1]));
  DONE;
}")

(define_insn "extendqisi2"
  [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
        (sign_extend:SI (match_operand:QI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
	return(	\"se %1,%0\;\"
		\"asr %0,#15,%d0\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "extendqidi2"
  [(set (match_operand:DI 0 "pic30_register_operand"                "=r")
        (sign_extend:DI (match_operand:QI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
	return(	\"se %1,%0\;\"
		\"asr %0,#15,%d0\;\"
		\"mov %d0,%t0\;\"
		\"mov %t0,%q0\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "extendhisi2"
  [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
        (sign_extend:SI (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
  int idSrc, idDst;

  idDst = REGNO(operands[0]);
  idSrc = REGNO(operands[1]);
  if (idDst == idSrc) {
    return \"asr %0,#15,%d0\";
  } else {
    if (idDst & 1)
      return \"mov %1,%0\;asr %0,#15,%d0\";
    else return \"mul.su %1,#1,%0\";
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "extendhip32ext2"
  [(set (match_operand:P32EXT 0 "pic30_register_operand"                "=r")
        (sign_extend:P32EXT (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
  int idSrc, idDst;

  idDst = REGNO(operands[0]);
  idSrc = REGNO(operands[1]);
  if (idDst == idSrc) {
    return \"lsr %0,#15,%d0\";
  } else {
    if (idDst & 1)
      return \"mov %1,%0\;lsr %0,#15,%d0\";
    else return \"mul.uu %1,#1,%0\";
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "zero_extendhip32ext2"
  [(set (match_operand:P32EXT 0 "pic30_register_operand"                "=r")
        (zero_extend:P32EXT (match_operand:HI 1 "pic30_register_operand" "r")) )]
  ""
  "*
{
  int idSrc, idDst;

  idDst = REGNO(operands[0]);
  idSrc = REGNO(operands[1]);
  if (idDst == idSrc) {
    return \"lsr %0,#15,%d0\";
  } else {
    if (idDst & 1)
      return \"mov %1,%0\;lsr %0,#15,%d0\";
    else return \"mul.uu %1,#1,%0\";
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "extendhip32eds2"
  [(set (match_operand: P32EDS 0 "pic30_register_operand"    "=r")
        (sign_extend:P32EDS
          (match_operand:HI 1    "pic30_register_operand"    " r")))]
  ""
  "mul.su %1,#1,%0"
)

(define_insn "extendhip32peds2"
  [(set (match_operand: P32PEDS 0 "pic30_register_operand"    "=r")
        (sign_extend:P32PEDS
          (match_operand:HI 1    "pic30_register_operand"    " r")))]
  ""
  "mul.su %1,#1,%0"
)

(define_insn "extendhidi2"
  [(set (match_operand:DI 0 "pic30_register_operand"                "=r,r")
        (sign_extend:DI (match_operand:HI 1 "pic30_register_operand" "0,r")) )]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
		return(	\"asr %0,#15,%d0\;\"
			\"mov %d0,%t0\;\"
			\"mov %t0,%q0\");
	default:
		return(	\"mov %1,%0\;\"
			\"asr %0,#15,%d0\;\"
			\"mov %d0,%t0\;\"
			\"mov %t0,%q0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Move instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Invalid move

(define_insn "*movqi_invalid_1"
  [(set (match_operand:QI 0 "pic30_register_operand"  "=r")
        (match_operand:QI 1 "pic30_code_operand" "g"))]
  ""
  "*
{
	error(\"invalid address space for operand\");
	return(\"nop\");
}

")

(define_insn "*movqi_invalid_2"
  [(set (match_operand:QI 0 "pic30_code_operand" "=g")
        (match_operand:QI 1 "pic30_register_operand"    "r"))]
  ""
  "*
{
	error(\"invalid address space for operand\");
	return(\"nop\");
}
")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 8-bit moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*movqi_const0"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int 0))]
  ""
  "@
   clr.b %0
   clr.b %0
   clr.b %0"
  [
   (set_attr "cc" "change0,change0,change0")
   (set_attr "type" "def,use,defuse")
  ]
)

(define_insn "*movqi_const1"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int -1))]
  ""
  "@
   setm.b %0
   setm.b %0
   setm.b %0"
  [
   (set_attr "cc" "change0,change0,change0")
   (set_attr "type" "def,use,defuse")
  ]
)

(define_insn "*movqi_const0sfr"
  [(set (match_operand:QI 0 "pic30_near_operand" "=U")
	(const_int 0))]
  ""
  "clr.b %0"
  [(set_attr "cc" "unchanged")])

(define_insn "*movqi_const1sfr"
  [(set (match_operand:QI 0 "pic30_near_operand" "=U")
	(const_int -1))]
  ""
  "setm.b %0"
  [(set_attr "cc" "unchanged")])

(define_insn "movqi_rimm"
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "immediate_operand" "i"))]
  ""
  "mov.b #%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

; general case
(define_insn "movqi_gen_DATA"
  [(set (match_operand:QI 0 "pic30_moveb_operand"
		"=r<>,RS,r<>, R,  r<>,RS,r<>, R,   Q,r,a,U,?d,?U,?U,?U, ?U, ? U,RS,<>RS,RS<>,Q,?T, ?T,   RS<>,<>RS,T, r")
        (match_operand:QI 1 "pic30_moveb_operand"
		 "r,  r,<>RS,<>RS,r,  r, RS<>,RS<>,r,Q,U,a, U, d,RS,<>RS,RS<>,Q,?U,?U,? U,?  U,RS<>,<>RS,?T,  ?T, ?r,?T"))
  ]
  ""
  "*
{  rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   rtx other_reg_with_save = 0;
   rtx other_reg = 0;
   int save=1;
   int reg;
   int not_reg = 0;
   rtx op;
   static char buffer[256];
  
   pic30_identify_used_regs(operands[1], &not_reg);
   pic30_identify_used_regs(operands[0], &not_reg);
   if (pic30_dead_or_set_p(insn,w0)) save = 0;
   for (reg = WR1_REGNO; reg < WR15_REGNO; reg++) {
     extern FILE *asm_out_file;
     if (not_reg & (1<<reg)) continue;
     other_reg = gen_rtx_REG(HImode,reg);
     if (other_reg_with_save == 0)
       other_reg_with_save = gen_rtx_REG(HImode,reg);
     if (pic30_dead_or_set_p(insn,other_reg)) {
       if (pic30_md_mustsave(other_reg)) {
         /* other_reg is not used and is already saved in prologue, so its 
            free! */
         break;
       }
       if ((reg < WR8_REGNO) && (!pic30_interrupt_function_p(cfun->decl))) {
         /* other_reg is not used and its a clobbered register, so its free! */
         break;
       }
       /* otherwise inelligble */
     }
     other_reg = 0;
   }

   switch (which_alternative) {
     case 0:  return \"mov.b %1,%0\";
     case 1:  return \"mov.b %1,%0\";
     case 2:  return \"mov.b %1,%0\";
     case 3:  return \"mov.b %1,%0\";
     case 4:  return \"mov.b %1,%0\";
     case 5:  return \"mov.b %1,%0\";
     case 6:  return \"mov.b %1,%0\";
     case 7:  return \"mov.b %1,%0\";
     case 8:  return \"mov.b %1,%0\";
     case 9:  return \"mov.b %1,%0\";
     case 10: return \"mov.b %1,WREG\";
     case 11: return \"mov.b WREG,%0\";
     case 12: return \"mov #%1,%0\;mov.b [%0],%0\";
     case 13: if (pic30_dead_or_set_p(insn, w0))
                return \"mov %1,w0\;mov.b WREG,%0\";
              else if (pic30_errata_mask & exch_errata)
                return \"push w0\;mov %1,w0\;mov.b WREG,%0\;pop w0\";
              else
                return \"exch w0,%1\;mov.b WREG,%0\;exch w0,%1\";
     case 14: /* U, RS */
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
              }
              break;
     case 15: /* U, <>RS */ 
     case 16: /* U, RS<> */ {
              char *inc=\"inc\";
              int pre=0;
              int post=0;
              if (GET_CODE(XEXP(operands[1],0)) == POST_INC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  post=1;
                }
              } else if (GET_CODE(XEXP(operands[1],0)) == POST_DEC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  post=1;
                  inc=\"dec\";
                }
              }
              if (GET_CODE(XEXP(operands[1],0)) == PRE_INC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  pre=1;
                }
              }
              if (GET_CODE(XEXP(operands[1],0)) == PRE_DEC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  pre=1;
                  inc=\"dec\";
                }
              }
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                if (post) {
                  sprintf(buffer,\"push w0\;mov.b %%s1,w0\;mov.b WREG,%%0\"
                                 \"\;pop w0\;%s %%r1,%%r1\", inc);
                  return buffer;
                } else if (pre) {
                  sprintf(buffer,\"%s %%s1,%ss1\;push w0\;mov.b %%s1,w0\"
                                 \"\;mov.b WREG,%%0\;pop w0\;%s %%r1,%%r1\", 
                                 inc);
                  return buffer;
                } else {
                  return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
                }

              }
              break;
              }
     case 17: /* U, Q */
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
              }
              break;
     case 18: /* R, U */
     case 19: /* <>R, U */
     case 20: /* R<>, U */
     case 21: /* Q, U */
              if (!save) {
                return \"mov.b %1,WREG\;mov.b w0,%0\";
              } else if (other_reg) {
                sprintf(buffer,\"mov #(%%1),w%d\;mov.b [w%d],%%0\",
                        REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                if (which_alternative == 21) {
                  sprintf(buffer,
                          \"push w%d\;mov #(%%1),w%d\;mov.b [w%d],w%d\"
                          \"\;mov.b w%d,%%0\;pop w%d\",
                          regno, regno, regno, regno, regno, regno);
                  return buffer;
                } else {
                  sprintf(buffer,
                         \"push w%d\;mov #(%%1),w%d\;mov.b [w%d],%%0\;pop w%d\",
                          regno, regno, regno, regno);
                  return buffer;
                }
              } else {
                return \"push w0\;mov.b %1,WREG\;mov.b w0,%0\;pop w0\";
              }
     case 22:
     case 23: /* T, <>R */
              if (!save) {
                return \"mov #(%0),w0\;mov.b %1,[w0]\";
              } else if (other_reg) {
                sprintf(buffer, \"mov #(%%0),w%d\;mov.b %%1,[w%d]\",
                                REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                sprintf(buffer,
                        \"push w%d\;mov #(%%0),w%d\;mov.b %%1,[w%d]\;pop w%d\",
                        regno, regno, regno, regno);
                return buffer;
              } else {
                return \"push w0\;mov #(%0),w0\;mov.b %1,[w0]\;pop w0\";
              }
     case 24:
     case 25: 
     case 26: /* T,r */
              if (!save) {
                return \"mov #(%0),w0\;mov.b %1,[w0]\";
              } else if (other_reg) {
                sprintf(buffer, \"mov #(%%0),w%d\;mov.b %%1,[w%d]\",
                                REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                sprintf(buffer,
                        \"push w%d\;mov #(%%0),w%d\;mov.b %%1,[w%d]\;pop w%d\",
                        regno, regno, regno, regno);
                return buffer;
              } else {
                return \"push w0\;mov #(%0),w0\;mov.b %1,[w0]\;pop w0\";
              }
     case 27: /* r, T */
              return \"mov #(%1),%0\;mov.b [%0],%0\";
  }
}
"
  [(set_attr "cc"
	"change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move,unchanged,change0,unchanged,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0")
   (set_attr "type"
	"def,etc,defuse,use,def,etc,defuse,use,etc,defuse,def,etc,def,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc")
  ]
)

(define_insn "movqi_gen_APSV"
  [(set (match_operand:QI 0 "pic30_mode3_APSV_operand"
                "=r<>,RS,r<>, R,  r<>,RS,r<>, R,   Q,r,a,U,?d,?U,?U,?U, ?U, ? U,RS,<>RS,RS<>,Q,?T, ?T,   RS<>,<>RS,T, r")
        (match_operand:QI 1 "pic30_move_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, RS<>,RS<>,r,Q,U,a, U, d,RS,<>RS,RS<>,Q,?U,?U,? U,?  U,RS<>,<>RS,?T,  ?T, ?r,?T"))
    (use (reg:HI PSVPAG))
  ]
  ""
  "*
{  rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   rtx other_reg_with_save = 0;
   rtx other_reg = 0;
   int save=1;
   int reg;
   int not_reg = 0;
   rtx op;
   static char buffer[256];

   pic30_identify_used_regs(operands[1], &not_reg);
   pic30_identify_used_regs(operands[0], &not_reg);
   if (pic30_dead_or_set_p(insn,w0)) save = 0;
   for (reg = WR1_REGNO; reg < WR15_REGNO; reg++) {
     extern FILE *asm_out_file;
     if (not_reg & (1<<reg)) continue;
     other_reg = gen_rtx_REG(HImode,reg);
     if (other_reg_with_save == 0)
       other_reg_with_save = gen_rtx_REG(HImode,reg);
     if (pic30_dead_or_set_p(insn,other_reg)) {
       if (pic30_md_mustsave(other_reg)) {
         /* other_reg is not used and is already saved in prologue, so its
            free! */
         break;
       }
       if ((reg < WR8_REGNO) && (!pic30_interrupt_function_p(cfun->decl))) {
         /* other_reg is not used and its a clobbered register, so its free! */
         break;
       }
       /* otherwise inelligble */
     }
     other_reg = 0;
   }

   switch (which_alternative) {
     case 0:  return \"mov.b %1,%0\";
     case 1:  return \"mov.b %1,%0\";
     case 2:  return \"mov.b %1,%0\";
     case 3:  return \"mov.b %1,%0\";
     case 4:  return \"mov.b %1,%0\";
     case 5:  return \"mov.b %1,%0\";
     case 6:  return \"mov.b %1,%0\";
     case 7:  return \"mov.b %1,%0\";
     case 8:  return \"mov.b %1,%0\";
     case 9:  return \"mov.b %1,%0\";
     case 10: return \"mov.b %1,WREG\";
     case 11: return \"mov.b WREG,%0\";
     case 12: return \"mov #%1,%0\;mov.b [%0],%0\";
     case 13: if (pic30_dead_or_set_p(insn, w0))
                return \"mov %1,w0\;mov.b WREG,%0\";
              else if (pic30_errata_mask & exch_errata)
                return \"push w0\;mov %1,w0\;mov.b WREG,%0\;pop w0\";
              else
                return \"exch w0,%1\;mov.b WREG,%0\;exch w0,%1\";
     case 14: /* U, RS */
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
              }
              break;
     case 15: /* U, <>RS */
     case 16: /* U, RS<> */ {
              char *inc=\"inc\";
              int pre=0;
              int post=0;
              if (GET_CODE(XEXP(operands[1],0)) == POST_INC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  post=1;
                }
              } else if (GET_CODE(XEXP(operands[1],0)) == POST_DEC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  post=1;
                  inc=\"dec\";
                }
              }
              if (GET_CODE(XEXP(operands[1],0)) == PRE_INC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  pre=1;
                }
              }
              if (GET_CODE(XEXP(operands[1],0)) == PRE_DEC) {
                if (REGNO(XEXP(XEXP(operands[1],0),0)) == 0) {
                  pre=1;
                  inc=\"dec\";
                }
              }
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                if (post) {
                  sprintf(buffer,\"push w0\;mov.b %%s1,w0\;mov.b WREG,%%0\"
                                 \"\;pop w0\;%s %%r1,%%r1\", inc);
                  return buffer;
                } else if (pre) {
                  sprintf(buffer,\"%s %%s1,%ss1\;push w0\;mov.b %%s1,w0\"
                                 \"\;mov.b WREG,%%0\;pop w0\;%s %%r1,%%r1\",
                                 inc);
                  return buffer;
                } else {
                  return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
                }

              }
              break;
              }
     case 17: /* U, Q */
              if (!save) {
                return \"mov.b %1,w0\;mov.b WREG,%0\";
              } else {
                return \"push w0\;mov.b %1,w0\;mov.b WREG,%0\;pop w0\";
              }
              break;
     case 18: /* R, U */
     case 19: /* <>R, U */
     case 20: /* R<>, U */
     case 21: /* Q, U */
              if (!save) {
                return \"mov.b %1,WREG\;mov.b w0,%0\";
              } else if (other_reg) {
                sprintf(buffer,\"mov #(%%1),w%d\;mov.b [w%d],%%0\",
                        REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                if (which_alternative == 21) {
                  sprintf(buffer,
                          \"push w%d\;mov #(%%1),w%d\;mov.b [w%d],w%d\"
                          \"\;mov.b w%d,%%0\;pop w%d\",
                          regno, regno, regno, regno, regno, regno);
                  return buffer;
                } else {
                  sprintf(buffer,
                         \"push w%d\;mov #(%%1),w%d\;mov.b [w%d],%%0\;pop w%d\",
                          regno, regno, regno, regno);
                  return buffer;
                }
              } else {
                return \"push w0\;mov.b %1,WREG\;mov.b w0,%0\;pop w0\";
              }
     case 22:
     case 23: /* T, <>R */
              if (!save) {
                return \"mov #(%0),w0\;mov.b %1,[w0]\";
              } else if (other_reg) {
                sprintf(buffer, \"mov #(%%0),w%d\;mov.b %%1,[w%d]\",
                                REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                sprintf(buffer,
                        \"push w%d\;mov #(%%0),w%d\;mov.b %%1,[w%d]\;pop w%d\",
                        regno, regno, regno, regno);
                return buffer;
              } else {
                return \"push w0\;mov #(%0),w0\;mov.b %1,[w0]\;pop w0\";
              }
     case 24:
     case 25:
     case 26: /* T,r */
              if (!save) {
                return \"mov #(%0),w0\;mov.b %1,[w0]\";
              } else if (other_reg) {
                sprintf(buffer, \"mov #(%%0),w%d\;mov.b %%1,[w%d]\",
                                REGNO(other_reg), REGNO(other_reg));
                return buffer;
              } else if (not_reg) {
                int regno;

                gcc_assert(other_reg_with_save);
                regno = REGNO(other_reg_with_save);
                sprintf(buffer,
                        \"push w%d\;mov #(%%0),w%d\;mov.b %%1,[w%d]\;pop w%d\",
                        regno, regno, regno, regno);
                return buffer;
              } else {
                return \"push w0\;mov #(%0),w0\;mov.b %1,[w0]\;pop w0\";
              }
     case 27: /* r, T */
              return \"mov #(%1),%0\;mov.b [%0],%0\";
  }
}
"
  [(set_attr "cc"
        "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move,unchanged,change0,unchanged,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,change0")
   (set_attr "type"
        "def,etc,defuse,use,def,etc,defuse,use,etc,defuse,def,etc,def,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc,etc")
  ]
)

(define_insn "movqi_gen_a_DATA"
  [(set (match_operand:QI 0 "pic30_move_operand"
		"=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:QI 1 "pic30_move2_operand"
		 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "*
{  rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   switch (which_alternative) {
     case 0:  return \"mov.b %1,%0\";
     case 1:  return \"mov.b %1,%0\";
     case 2:  return \"mov.b %1,%0\";
     case 3:  return \"mov.b %1,%0\";
     case 4:  return \"mov.b %1,%0\";
     case 5:  return \"mov.b %1,%0\";
     case 6:  return \"mov.b %1,%0\";
     case 7:  return \"mov.b %1,%0\";
     case 8:  return \"mov.b %1,%0\";
     case 9:  return \"mov.b %1,%0\";
     case 10: return \"mov.b WREG,%0\";
     case 11: if (REGNO(operands[1]) == WR0_REGNO)
                return \"mov.b WREG,%0\";
              else  if (pic30_dead_or_set_p(insn, w0))
                return \"mov %1,w0\;mov.b WREG,%0\";
              else if (pic30_errata_mask & exch_errata)
                return \"push w0\;mov %1,w0\;mov.b WREG,%0\;pop w0\";
              else
                return \"exch w0,%1\;mov.b WREG,%0\;exch w0,%1\";
  }
}
"
  [(set_attr "cc"
	"change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,unchanged,unchanged")
   (set_attr "type"
	"def,etc,defuse,use,def,etc,defuse,use,etc,defuse,etc,etc")
  ]
)

(define_insn "movqi_gen_a_APSV"
  [(set (match_operand:QI 0 "pic30_move_operand"
		"=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (unspec:QI [
           (match_operand:QI 1 "pic30_move2_APSV_operand"
		 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r")
           (reg:HI PSVPAG)] UNSPECV_USEPSV))
  ]
  ""
  "*
{  rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   switch (which_alternative) {
     case 0:  return \"mov.b %1,%0\";
     case 1:  return \"mov.b %1,%0\";
     case 2:  return \"mov.b %1,%0\";
     case 3:  return \"mov.b %1,%0\";
     case 4:  return \"mov.b %1,%0\";
     case 5:  return \"mov.b %1,%0\";
     case 6:  return \"mov.b %1,%0\";
     case 7:  return \"mov.b %1,%0\";
     case 8:  return \"mov.b %1,%0\";
     case 9:  return \"mov.b %1,%0\";
     case 10: return \"mov.b WREG,%0\";
     case 11: if (REGNO(operands[1]) == WR0_REGNO)
                return \"mov.b WREG,%0\";
              else  if (pic30_dead_or_set_p(insn, w0))
                return \"mov %1,w0\;mov.b WREG,%0\";
              else if (pic30_errata_mask & exch_errata)
                return \"push w0\;mov %1,w0\;mov.b WREG,%0\;pop w0\";
              else
                return \"exch w0,%1\;mov.b WREG,%0\;exch w0,%1\";
  }
}
"
  [(set_attr "cc"
	"change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,unchanged,unchanged")
   (set_attr "type"
	"def,etc,defuse,use,def,etc,defuse,use,etc,defuse,etc,etc")
  ]
)



(define_insn "movqi_gen_b"
  [(set (match_operand:QI 0 "pic30_move2_operand"
		"=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,a,r")
        (match_operand:QI 1 "pic30_move_operand"
		 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,U,  U"))
  ]
  ""
  "*
{  rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   switch (which_alternative) {
     case 0:  return \"mov.b %1,%0\";
     case 1:  return \"mov.b %1,%0\";
     case 2:  return \"mov.b %1,%0\";
     case 3:  return \"mov.b %1,%0\";
     case 4:  return \"mov.b %1,%0\";
     case 5:  return \"mov.b %1,%0\";
     case 6:  return \"mov.b %1,%0\";
     case 7:  return \"mov.b %1,%0\";
     case 8:  return \"mov.b %1,%0\";
     case 9:  return \"mov.b %1,%0\";
     case 10: return \"mov.b %1,WREG\";
     case 11: if (REGNO(operands[0]) == WR0_REGNO) 
                return \"mov.b %1,WREG\";
              else return \"mov #%1,%0\;mov.b [%0],%0\";
  }
}
"
  [(set_attr "cc"
	"change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move,change0")
   (set_attr "type"
	"def,etc,defuse,use,def,etc,defuse,use,etc,defuse,def,def")
  ]
)

; rare sfr->sfr case
;;(define_insn "movqi_sfr_a"
;;   [(set (match_operand:QI 0 "pic30_near_operand" "=U,U,   U, U,   U,U,U")
;;         (match_operand:QI 1 "pic30_mode3_operand"  "U,RS,<>RS,RS<>,Q,a,r"))]
;;   ""
;;   "*
;;{
;;    rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
;;    int save=1;
;;
;;    if (pic30_dead_or_set_p(insn, w0)) save=0;
;;    switch (which_alternative) {
;;      case 0:
;;        if (save) {
;;          return \"push w0\;mov.b %1,WREG\;mov.b WREG,%0\;pop w0\";
;;        } else return \"mov.b %1,WREG\;mov.b WREG,%0\";
;;        break;
;;      case 1:
;;      case 2:
;;      case 3:
;;      case 4:
;;        if (save) {
;;          return \"push w0\;mov.b %1,WREG\;mov.b w0,%0\;pop w0\";
;;        } else return \"mov.b %1,WREG\;mov.b w0,%0\";
;;        break;
;;      case 5:
;;        return \"mov.b WREG,%0\";
;;      case 6:
;;        if (save) {
;;          if (pic30_errata_mask & exch_errata)
;;            return \"push w0\;mov %1,w0\;mov.b WREG,%0\;pop w0\";
;;          else
;;            return \"exch %1,w0\;mov.b WREG,%0\;exch %1,w0\";
;;        } else return \"mov %1,w0\;mov.b WREG,%0\";
;;        break;
;;   }
;;}
;;   "
;;    
;;   [(set_attr "cc" "change0,change0, change0,change0,change0,change0,change0")
;;    (set_attr "type" "etc,etc,etc,etc,etc,etc,etc")]
;;)
;;
;;(define_insn "movqi_sfr_b"
;;   [(set (match_operand:QI 0 "pic30_move_operand" "=U,RS,<>RS,RS<>,Q,a,r")
;;         (match_operand:QI 1 "pic30_UT_operand" " U,U,    U,  U,   U,U,UT"))]
;;   ""
;;   "*
;;{
;;    rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
;;    int save=1;
;;    
;;    if (pic30_dead_or_set_p(insn, w0)) save = 0;
;;    switch (which_alternative) {
;;      case 0:
;;        if (save) {
;;          return \"push w0\;mov.b %1,WREG\;mov.b WREG,%0\;pop w0\";
;;        } else return \"mov.b %1,WREG\;mov.b w0,%0\";
;;        break;
;;      case 1:
;;      case 2:
;;      case 3:
;;      case 4:
;;        if (save) {
;;          return \"push w0\;mov.b %1,WREG\;mov.b w0,%0\;pop w0\";
;;        } else return \"mov.b %1,WREG\;mov.b w0,%0\";
;;        break;
;;      case 5:
;;        return \"mov.b %1,WREG\";
;;        break;
;;      case 6:
;;        return \"mov #(%1),%0\;mov.b [%0],%0\";
;;        break;
;;    }
;;}
;;   "
;;   [(set_attr "cc" "change0,change0,change0,change0,change0,change0,change0")
;;    (set_attr "type" "etc,etc,etc,etc,etc,etc,etc")]
;;)
;;

;; If one of the operands is immediate and the other is not a register,
;; then we should emit two insns, using a scratch register.  This will produce
;; better code in loops if the source operand is invariant, since
;; the source reload can be optimised out.  During reload we cannot
;; use change_address or force_reg which will allocate new pseudo regs.

;; Unlike most other insns, the move insns can`t be split with
;; different predicates, because register spilling and other parts of
;; the compiler, have memoized the insn number already.

(define_expand "movqi"
  [(set (match_operand:QI 0 "pic30_general_operand" "")
        (match_operand:QI 1 "pic30_general_operand" ""))]
  ""
  "
{
  if (pic30_emit_move_sequence(operands, QImode)) DONE;
  if (pic30_move_operand(operands[1],GET_MODE(operands[1]))) {
     emit(gen_movqi_gen_DATA(operands[0],operands[1]));
     DONE;
  } else if (pic30_move_APSV_operand(operands[1],GET_MODE(operands[1]))) {
     emit(gen_movqi_gen_APSV(operands[0],operands[1]));
     DONE;
  }
  /* FAIL;  why did i put this here? */
}")

;; (define_expand "reload_inqi"
;;   [(set (match_operand:QI 0      "pic30_register_operand" "=d")
;; 	(match_operand:QI 1     "pic30_near_operand" "U"))
;;    (clobber (match_operand:QI 2 "pic30_register_operand" "=&a"))]
;;   ""
;;   "
;; {
;;   emit_move_insn (operands[2], operands[1]);
;;   emit_move_insn (operands[0], operands[2]);
;;   DONE;
;; }")

;; (define_expand "reload_outqi"
;;   [(set (match_operand:QI 0    "pic30_near_operand" "=U")
;; 	(match_operand:QI 1       "pic30_register_operand" "d"))
;;    (clobber (match_operand:QI 2 "pic30_register_operand" "=&a"))]
;;   ""
;;   "
;; {
;;   emit_move_insn (operands[2], operands[1]);
;;   emit_move_insn (operands[0], operands[2]);
;;   DONE;
;; }")

;; Invalid move

(define_insn "*movhi_invalid_1"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "pic30_code_operand"  "g"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}

")

(define_insn "*movhi_invalid_2"
  [(set (match_operand:HI 0 "pic30_code_operand" "=g")
        (match_operand:HI 1 "pic30_register_operand"  "r"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")

(define_insn "*movhi_invalid_3"
  [(set (match_operand:HI 0 "pic30_move_operand" "")
        (mem: HI (match_operand:HI 1 "pic30_invalid_address_operand"  "")))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")

(define_insn "*movhi_invalid_4"
  [(set (mem (match_operand:P24PROG 0 "pic30_prog_operand" ""))
        (match_operand 1 "general_operand" ""))]
  ""
  "*
   {
     error(\"invalid address space for destination\");
     return (\"this is not an instruction\");
   }
")

(define_insn "*movhi_invalid_5"
  [(set (mem (match_operand:P24PSV 0 "pic30_psv_operand" ""))
        (match_operand 1 "general_operand" ""))]
  ""
  "*
   {
     error(\"invalid address space for destination\");
     return (\"this is not an instruction\");
   }
")

(define_insn "*movhi_invalid_6"
  [(set (mem (match_operand:P32DF 0 "pic30_df_operand" ""))
        (match_operand 1 "general_operand" ""))]
  ""
  "*
   {
     error(\"invalid address space for destination\");
     return (\"this is not an instruction\");
   }
")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 16-bit moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;
;; immediate zero
;;;;;;;;;;;;;;;;;

(define_insn "*movhi_const0sfr"
  [(set (match_operand:HI 0 "pic30_near_operand" "=U")
	(const_int 0))]
  ""
  "clr %0"
  [(set_attr "cc" "unchanged")])

(define_insn "*movhi_const1sfr"
  [(set (match_operand:HI 0 "pic30_near_operand" "=U")
	(const_int -1))]
  ""
  "setm %0"
  [(set_attr "cc" "unchanged")])

(define_insn "movhi_const0"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int 0))]
  ""
  "@
   clr %0
   clr %0
   clr %0"
  [
   (set_attr "cc" "change0,change0,change0")
   (set_attr "type" "def,use,defuse")
  ]
)

(define_insn "movp16apsv_const0"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int 0))]
  ""
  "@
   clr %0
   clr %0
   clr %0"
  [
   (set_attr "cc" "change0,change0,change0")
   (set_attr "type" "def,use,defuse")
  ]
)

(define_insn "*movhi_const1"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int -1))]
  ""
  "@
   setm %0
   setm %0
   setm %0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,use,defuse")
  ]
)

(define_insn "*movp16apsv_const1"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=r,R,<>")
	(const_int -1))]
  ""
  "@
   setm %0
   setm %0
   setm %0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,use,defuse")
  ]
)

;;;;;;;;;;;;;;;;;;;;
;; general immediate
;;;;;;;;;;;;;;;;;;;;

(define_insn "movqi_address"
  [(set (match_operand:QI 0 "pic30_register_operand"              "=r")
        (subreg:QI
          (match_operand:HI 1 "pic30_symbolic_address_operand"      "g") 0))]
  ""
  "*
{
   return(\"mov #%1,%0\");
}"
 [
  (set_attr "cc" "change0")
  (set_attr "type" "def")
 ]
)

(define_insn "movhi_address"
  [(set (match_operand:HI 0 "pic30_register_operand"         "=r")
        (match_operand:HI 1 "pic30_symbolic_address_operand" "g"))]
  ""
  "*
{ rtx sym;

  if (pic30_symbolic_address_operand(operands[1], GET_MODE(operands[1]))) {
    /* this can be converted to a register copy, so check for symref first */
    sym = pic30_program_space_operand_p(operands[1]);
    if (sym) {
      tree fndecl = GET_CODE(sym) == SYMBOL_REF ? SYMBOL_REF_DECL(sym) : 0;
      tree fndecl_attrib;
      char *access=0;
      int slot = 0;

      if (fndecl) {
        if (fndecl_attrib =
              lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                               DECL_ATTRIBUTES(fndecl))) {
          access=\"boot\";
        } else if (fndecl_attrib = lookup_attribute(
                                    IDENTIFIER_POINTER(pic30_identSecure[0]),
                                    DECL_ATTRIBUTES(fndecl))) {
          access=\"secure\";
        }
      }
      if (access) {
        if (TREE_VALUE(fndecl_attrib)) {
          if (TREE_CODE(TREE_VALUE(TREE_VALUE(fndecl_attrib))) == INTEGER_CST) {
            slot = TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(fndecl_attrib)));
          } else access = 0;
        } else access = 0;
      }
      if (access) {
        static char buffer[25];
        sprintf(buffer,\"mov #%s(%d),%%0\", access, slot);
        return buffer;
      } else {
        return \"mov #handle(%1),%0\";
      }
    } else {
      return \"mov #%1,%0\";
    }
  } else return \"mov %1,%0\";
}"
 [
  (set_attr "cc" "change0")
  (set_attr "type" "def")
 ]
)

(define_insn "movP16APSV_address"
  [(set (match_operand:P16APSV 0 "pic30_register_operand"         "=r")
        (match_operand:P16APSV 1 "pic30_symbolic_address_operand" "g"))]
  ""
  "*
{ rtx sym;

  if (pic30_symbolic_address_operand(operands[1], GET_MODE(operands[1]))) {
    /* this can be converted to a register copy, so check for symref first */
    sym = pic30_program_space_operand_p(operands[1]);
    if (sym) {
      tree fndecl = GET_CODE(sym) == SYMBOL_REF ? SYMBOL_REF_DECL(sym) : 0;
      tree fndecl_attrib;
      char *access=0;
      int slot = 0;

      if (fndecl) {
        if (fndecl_attrib =
              lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                               DECL_ATTRIBUTES(fndecl))) {
          access=\"boot\";
        } else if (fndecl_attrib = lookup_attribute(
                                    IDENTIFIER_POINTER(pic30_identSecure[0]),
                                    DECL_ATTRIBUTES(fndecl))) {
          access=\"secure\";
        }
      }
      if (access) {
        if (TREE_VALUE(fndecl_attrib)) {
          if (TREE_CODE(TREE_VALUE(TREE_VALUE(fndecl_attrib))) == INTEGER_CST) {
            slot = TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(fndecl_attrib)));
          } else access = 0;
        } else access = 0;
      }
      if (access) {
        static char buffer[25];
        sprintf(buffer,\"mov #%s(%d),%%0\", access, slot);
        return buffer;
      } else {
        return \"mov #handle(%1),%0\";
      }
    } else {
      return \"mov #%1,%0\";
    }
  } else return \"mov %1,%0\";
}"
 [
  (set_attr "cc" "change0")
  (set_attr "type" "def")
 ]
)

(define_insn "movhi_imm"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "immediate_operand" "i"))]
  ""
  "mov #%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "movP16APSV_imm"
  [(set (match_operand:P16APSV 0 "pic30_register_operand" "=r")
        (match_operand:P16APSV 1 "immediate_operand" "i"))]
  ""
  "mov #%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;
;; builtin move directives
;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "readsfr"
  [(set (match_operand:HI 0 "pic30_register_operand"           "=r")
        (unspec_volatile:HI [(match_operand:HI 1 "pic30_register_operand" "r")]
		    UNSPECV_READSFR))]
  ""
  "push SR\;bset SR,#5\;bset SR,#6\;bset SR,#7\;mov [%1],[w15]\;mov [%1],%0\;pop SR"
  [(set_attr "cc" "clobber")])

(define_insn "writesfr"
  [(unspec_volatile:HI [
     (match_operand:HI 0 "pic30_register_operand" "r")
     (match_operand:HI 1 "pic30_register_operand" "r")
  ] UNSPECV_WRITESFR)]
  ""
  "push SR\;bset SR,#5\;bset SR,#6\;bset SR,#7\;mov %0,%0\;mov %0,%0\;mov %1,[%0]\;pop SR"
  [(set_attr "cc" "clobber")])

(define_insn "tbladdress"
  [(set (match_operand:SI 0 "pic30_register_operand"              "=r")
        (unspec:SI [(match_operand:HI 1 "pic30_symbolic_address_operand" "g")]
                    UNSPECV_TBLADDRESS))]
  ""
  "mov #tbloffset(%1),%0\;mov #tblpage(%1),%d0"
  [(set_attr "cc" "change0")])

(define_expand "tblpage"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (tblpage:HI 
           (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "{ char *t = pic30_section_base(operands[1],1,0);

     emit_insn(
       gen_tblpage_helper(operands[0], GEN_INT((HOST_WIDE_INT)t))
     );
     DONE;
   }
  ")

(define_insn "tblpage_helper"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (tblpage:HI 
           (match_operand 1 "immediate_operand"   "i")))]
  ""
  "*
   { static char result[256];

     char *o1 = (char*)INTVAL(operands[1]);
     sprintf(result,\"mov #tblpage(%s),%%0\", o1);
     return result;
   }"
  [(set_attr "cc" "change0")])

(define_expand "edspage"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (edspage:HI 
           (match_operand 1 "pic30_symbolic_address_operand" "g")
           (match_operand 2 "immediate_operand" "i")))]
  ""
  "{ char *t = pic30_section_base(operands[1],1,0);

     emit_insn(
       gen_edspage_helper(operands[0], GEN_INT((HOST_WIDE_INT)t),operands[2])
     );
     DONE;
   }
  ")

(define_insn "movpag"
  [(set (reg:HI PSVPAG)
        (edspage:HI
           (match_operand 0 "pic30_reg_or_symbolic_address" "g,r")
           (match_operand 1 "immediate_operand" "i,i")))]
  "pic30_ecore_target()"
  "*
   {  
      if (INTVAL(operands[1]))
        error(\"Page offset not accepted\");
      if (which_alternative == 0) {
        char *t = pic30_section_base(operands[0],1,0);
        static char result[80];

        sprintf(result,\"movpag #edspage(%s),DSRPAG\", t);
        return result;
      } else {
        return \"movpag %0,DSRPAG\";
      }
   }"
  [(set_attr "cc" "change0")])

(define_insn "edspage_helper"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (edspage:HI 
           (match_operand 1 "immediate_operand"   "i")
           (match_operand 2 "immediate_operand"   "i")))]
  ""
  "*
   { static char result[256];
     char *e = result;

     char *o1 = (char*)INTVAL(operands[1]);
     e += sprintf(result,\"mov #edspage(%s),%%0\", o1);
     if (INTVAL(operands[2]))
       sprintf(e,\"\;add %%0,#%d,%%0\", INTVAL(operands[2]));
     return result;
   }"
  [(set_attr "cc" "change0")])

(define_expand "edsoffset"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (edsoffset:HI
           (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "{ char *t = pic30_section_base(operands[1],0,0);

     emit_insn(
       gen_edsoffset_helper(operands[0], GEN_INT((HOST_WIDE_INT)t))
     );
     DONE;
   }
  ")

(define_insn "edsoffset_helper"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (edsoffset:HI
           (match_operand 1 "immediate_operand"   "i")))]
  ""
  "*
   { static char result[256];

     char *o1 = (char*)INTVAL(operands[1]);
     sprintf(result,\"mov #edsoffset(%s),%%0\", o1);
     return result;
   }"
  [(set_attr "cc" "change0")])

(define_insn "tbloffset"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (tbloffset:HI 
           (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "mov #tbloffset(%1),%0"
  [(set_attr "cc" "change0")])

(define_expand "psvpage"
  [(set (match_operand:HI 0 "pic30_register_operand"            "=r")
        (psvpage:HI 
           (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "{ char *t = pic30_section_base(operands[1],0,0);

     emit_insn(
       gen_psvpage_helper(operands[0], GEN_INT((HOST_WIDE_INT)t))
     );
     DONE;
   }")

(define_insn "psvpage_helper"
  [(set (match_operand:HI 0 "pic30_register_operand"    "=r")
        (psvpage:HI 
           (match_operand 1 "immediate_operand" "g")))]
  ""
  "*
   { static char result[256];

     char *o1 = (char*)INTVAL(operands[1]);
     sprintf(result,\"mov #psvpage(%s),%%0\", o1);
     return result;
   }"
  [(set_attr "cc" "change0")])

(define_insn "psvoffset"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (psvoffset:HI  
           (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "mov #psvoffset(%1),%0"
  [(set_attr "cc" "change0")])

(define_insn "dmaoffset"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (dmaoffset:HI (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "mov #dmaoffset(%1),%0"
  [(set_attr "cc" "change0")])

(define_insn "dmapage"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (dmapage:HI (match_operand 1 "pic30_symbolic_address_operand" "g")))]
  ""
  "mov #dmapage(%1),%0"
  [(set_attr "cc" "change0")])

;;;;;;;;;;;;;;;
;; general case
;;;;;;;;;;;;;;;

;  (define_insn "movtosfrhi"
;    [(set (match_operand:HI 0 "pic30_sfr_operand" "=U")
;          (match_operand:HI 1 "pic30_register_operand" "r"))]
;    "pic30_ecore_target()"
;    "mov %1,%0"
;    [(set_attr "type" "def")])
;  
;  (define_insn "movfromsfrhi"
;    [(set (match_operand:HI 0 "pic30_register_operand" "=r")
;          (match_operand:HI 1 "pic30_sfr_operand" "U"))]
;    "pic30_ecore_target()"
;    "mov %1,%0"
;    [(set_attr "type" "use")])
;  

(define_insn "movhi_gen"
  [(set (match_operand:HI 0 
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a,U")
        (match_operand:HI 1
	   "pic30_move_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U,U"))]
  ""
  "*
   { 
     char *format[] = {
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,WREG\",
       \"push %1\;pop %0\" 
     };

     #define IDENT_EXTERNAL(t) \
       ((t) == pic30_identExternal[0] || (t) == pic30_identExternal[1])

     if ((which_alternative == 8) || (which_alternative == 10)) {
       tree decl = 0;
       if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF)
         decl = SYMBOL_REF_DECL(XEXP(operands[1],0));
       if (decl) {
         tree attr = lookup_attribute(IDENTIFIER_POINTER(pic30_identSpace[0]),
                                      DECL_ATTRIBUTES(decl));
         if (attr &&
             (TREE_CODE(TREE_VALUE(TREE_VALUE(attr))) == CALL_EXPR)) {
           tree id;
           id = TREE_OPERAND(CALL_EXPR_FN(TREE_VALUE(TREE_VALUE(attr))),0);
           if (IDENT_EXTERNAL(DECL_NAME(id))) return \"mov #%1,%0\";
         }
       }
     }
     return format[which_alternative];
   }"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move,change0")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def,etc")
  ])

(define_insn "movhi_gen_APSV"
  [(set (match_operand:HI 0 
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
        (unspec:HI [
          (match_operand:HI 1
	   "pic30_move_APSV_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
   { 
     char *format[] = {
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,WREG\" 
     };

     #define IDENT_EXTERNAL(t) \
       ((t) == pic30_identExternal[0] || (t) == pic30_identExternal[1])

     if ((which_alternative == 8) || (which_alternative == 10)) {
       tree decl = 0;
       if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF)
         decl = SYMBOL_REF_DECL(XEXP(operands[1],0));
       if (decl) {
         tree attr = lookup_attribute(IDENTIFIER_POINTER(pic30_identSpace[0]),
                                      DECL_ATTRIBUTES(decl));
         if (attr &&
             (TREE_CODE(TREE_VALUE(TREE_VALUE(attr))) == CALL_EXPR)) {
           tree id;
           id = TREE_OPERAND(CALL_EXPR_FN(TREE_VALUE(TREE_VALUE(attr))),0);
           if (IDENT_EXTERNAL(DECL_NAME(id))) return \"mov #%1,%0\";
         }
       }
     }
     return format[which_alternative];
   }"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
  ])

;(define_insn "movP16APSV_gen_APSV"
;  [(set (match_operand:P16APSV 0
;           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
;        (unspec:P16APSV [
;          (match_operand:P16APSV 1
;           "pic30_move_APSV_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U")
;          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
;  ""
;  "@
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,%0
;   mov %1,WREG"
;  [(set_attr "cc"
;  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
;   (set_attr "type"
;   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
;  ])


(define_insn "movP16PMP_gen"
  [(set (match_operand:P16PMP 0
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
        (match_operand:P16PMP 1
           "pic30_move_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U"))]
  ""
  "@
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,WREG"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
  ])

(define_insn "movP16PMP_gen_APSV"
  [(set (match_operand:P16PMP 0
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
        (unspec:P16PMP [
          (match_operand:P16PMP 1
           "pic30_move_APSV_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "@
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,WREG"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
  ])


;; If one of the operands is immediate and the other is not a register,
;; then we should emit two insns, using a scratch register.  This will produce
;; better code in loops if the source operand is invariant, since
;; the source reload can be optimised out.  During reload we cannot
;; use change_address or force_reg which will allocate new pseudo regs.

;; Unlike most other insns, the move insns can`t be split with
;; different predicates, because register spilling and other parts of
;; the compiler, have memoized the insn number already.

(define_expand "movhi"
  [(set (match_operand:HI 0 "pic30_move_operand" "")
        (match_operand:HI 1 "pic30_move_operand" ""))]
  ""
  "
{ int result = pic30_emit_move_sequence(operands, HImode);

  if (result > 0) DONE;
}")

(define_insn "movp16apsv_gen"
  [(set (match_operand:P16APSV 0 
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
        (match_operand:P16APSV 1
	   "pic30_move_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U"))]
  ""
  "*
   { 
     char *format[] = {
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,WREG\" 
     };

     #define IDENT_EXTERNAL(t) \
       ((t) == pic30_identExternal[0] || (t) == pic30_identExternal[1])

     if ((which_alternative == 8) || (which_alternative == 10)) {
       tree decl = 0;
       if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF)
         decl = SYMBOL_REF_DECL(XEXP(operands[1],0));
       if (decl) {
         tree attr = lookup_attribute(IDENTIFIER_POINTER(pic30_identSpace[0]),
                                      DECL_ATTRIBUTES(decl));
         if (attr &&
             (TREE_CODE(TREE_VALUE(TREE_VALUE(attr))) == CALL_EXPR)) {
           tree id;

           id = TREE_OPERAND(CALL_EXPR_FN(TREE_VALUE(TREE_VALUE(attr))),0);
           if (IDENT_EXTERNAL(DECL_NAME(id))) return \"mov #%1,%0\";
         }
       }
     }
     return format[which_alternative];
   }"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
  ])

(define_insn "movp16apsv_gen_APSV"
  [(set (match_operand:P16APSV 0 
           "pic30_move_operand" "=r<>, R,   r<>,R,S,S,  Q,r,r,T,a")
        (unspec:P16APSV [
          (match_operand:P16APSV 1
	   "pic30_move_APSV_operand"  "RS<>,RS<>,r,  r,r,<>R,r,Q,T,r,U")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
   { 
     char *format[] = {
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,%0\",
       \"mov %1,WREG\" 
     };

     #define IDENT_EXTERNAL(t) \
       ((t) == pic30_identExternal[0] || (t) == pic30_identExternal[1])

     if ((which_alternative == 8) || (which_alternative == 10)) {
       tree decl = 0;
       if (GET_CODE(XEXP(operands[1],0)) == SYMBOL_REF)
         decl = SYMBOL_REF_DECL(XEXP(operands[1],0));
       if (decl) {
         tree attr = lookup_attribute(IDENTIFIER_POINTER(pic30_identSpace[0]),
                                      DECL_ATTRIBUTES(decl));
         if (attr &&
             (TREE_CODE(TREE_VALUE(TREE_VALUE(attr))) == CALL_EXPR)) {
           tree id;

           id = TREE_OPERAND(CALL_EXPR_FN(TREE_VALUE(TREE_VALUE(attr))),0);
           if (IDENT_EXTERNAL(DECL_NAME(id))) return \"mov #%1,%0\";
         }
       }
     }
     return format[which_alternative];
   }"
  [(set_attr "cc"
  "change0,change0,change0,change0,change0,change0,change0,change0,change0,change0,move")
   (set_attr "type"
   "defuse,use,def,etc,etc,etc,use,defuse,def,etc,def")
  ])


(define_expand "movp16apsv"
  [(set (match_operand:P16APSV 0 "pic30_move_operand" "")
        (match_operand:P16APSV 1 "pic30_move_operand" ""))]
  ""
  "
{ int result = pic30_emit_move_sequence(operands, P16APSVmode);

  if (result > 0) DONE;
}")

;;
;; Reload can generate a partial load of a larger item
;;   Recognize the instruction. - Bug in nullstone cse -O[23]
;;

(define_insn "movhi_reload_lo"
  [(set (match_operand:HI 0 "pic30_move_operand"   "=r,RS,r,R,a,T,r")
        (subreg:HI 
           (match_operand:SI 1 "pic30_move_operand" "r,r,RS,R,U,r,T") 0))]
  ""
  "@
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,%0
   mov %1,WREG
   mov %1,%0
   mov %1,%0"
  [
   (set_attr "cc" "change0,change0,change0,change0,move,change0,change0")
   (set_attr "type" "def,etc,defuse,use,def,etc,def")
  ]
)

(define_insn "movhi_reload_hi"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"   "=a,r")
        (subreg:HI 
           (match_operand:SI 1 "pic30_UT_operand" "U,T") 2))]
  ""
  "@
   mov %1+2,WREG
   mov %1+2,%0"
  [
   (set_attr "cc" "move,change0")
   (set_attr "type" "def,def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 24-bit moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
  
(define_expand "movP24PROG_address"
  [(set 
     (match_operand:P24PROG 0 "pic30_move_operand"             "=r,R,<>,QSTU")
     (match_operand:P24PROG 1 "pic30_symbolic_address_operand" " g,g,g ,g"))
  ]
  ""
  "
{
  rtx op2 = 0;

  if (!reload_in_progress) op2 = gen_reg_rtx(GET_MODE(operand0));
  emit_insn(
    gen_tbloffset(
      simplify_gen_subreg(HImode, op2 ? op2 : operand0, GET_MODE(operand0), 0), 
      operand1)
  );
  emit_insn(
    gen_tblpage(
      simplify_gen_subreg(HImode, op2 ? op2 : operand0, GET_MODE(operand0), 2),
      operand1)
  );
  if (op2) emit_insn(
             gen_movP24PROG_gen(operand0,op2)
           );
  DONE;
}")

(define_insn_and_split "movP24PSV_address"
  [(set (match_operand:P24PSV 0 "pic30_move_operand"             "=r,R,<>,QSTU")
        (match_operand:P24PSV 1 "pic30_symbolic_address_operand" " g,g,g,g")
   )
   (clobber (match_scratch:HI 2                                  "=X,&r,&r,&r"))]
  ""
  "@
   mov #tbloffset(%1),%0\;mov #tblpage(%1),%d0
   mov #tbloffset(%1),%2\;mov %2,%I0\;mov #tblpage(%1),%2\;mov %2,%D0
   mov #tbloffset(%1),%2\;mov %2,%0\;mov #tblpage(%1),%2\;mov %2,%0
   mov #tbloffset(%1),%2\;mov %2,%0\;mov #tblpage(%1),%2\;mov %2,%0+2"
  "reload_completed && REG_P(operands[0])"
  [(set (match_operand:HI 3 "pic30_register_operand" "=r")
        (tblpage:HI (match_operand 4 "immediate_operand" "i")))
   (set (match_operand:HI 5 "pic30_register_operand" "=r")
        (tbloffset:HI (match_dup 1)))]
  "operands[3] = gen_rtx_REG(HImode, REGNO(operands[0])+1);
   operands[4] = GEN_INT((HOST_WIDE_INT)pic30_section_base(operands[1],1,0));
   operands[5] = gen_rtx_REG(HImode, REGNO(operands[0]));"
   
)  

(define_insn "movP16PMP_address"
  [(set (match_operand:P16PMP 0 "pic30_move_operand"             "=r,R,<>,QSTU")
        (match_operand:P16PMP 1 "pic30_symbolic_address_operand" " g,g,g ,g"))
   (clobber (match_scratch:HI 2                                  "=X,&r,&r,&r"))
  ]
  ""
  "*
   { static char result[256];
     char *o1 = (char*)INTVAL(operands[1]);

     switch (which_alternative) {
       case 0: sprintf(result,\"mov #%%1,%%0\");
               break;
       case 1: sprintf(result, \"mov #%%1,%%2\;mov %%2,%%0\");
               break;
       case 2: sprintf(result, \"mov #%%1,%%2\;mov %%2,%%0\");
               break;
       case 3: sprintf(result, \"mov #%%1,%%2\;mov %%2,%%0\");
               break;
     }
     return result;
   }
  "
)

(define_insn "movEDS_address_page"
  [(set (match_operand:HI     0 "pic30_move_operand"             "=r,R,<>,QSTU")
        (edspage: HI
          (match_operand     1 "immediate_operand" " i,i,i,i")
          (match_operand     2 "immediate_operand" " i,i,i,i"))
   )
   (clobber (match_scratch:HI 3                                  "=X,&r,&r,&r"))
  ]
  ""
  "*
   { static char result[256];
     char *o1 = (char*)INTVAL(operands[1]);
     char *e = result;
     int excess = 0;
     
     if (INTVAL(operands[2]) != 0) {
       // an excess was specified
       excess = INTVAL(operands[2]);
     }
     switch (which_alternative) {
       case 0: e += sprintf(result,\"mov #edspage(%s),%%0\", o1);
               if (excess) sprintf(e,\"\;add %%0,#%d,%%0\", excess);
               break;
       case 1: if (excess) 
                 sprintf(result, \"mov #edspage(%s),%%3\;add %%3,#%d,%%0\",
                         o1, excess);
               else
                 sprintf(result, \"mov #edspage(%s),%%3\;mov %%3,%%0\",o1);
               break;
       case 2: if (excess) 
                 sprintf(result, \"mov #edspage(%s),%%3\;add %%3,#%d,%%0\",
                         o1, excess);
               else
                 sprintf(result, \"mov #edspage(%s),%%3\;mov %%3,%%0\",o1);
               break;
       case 3: if (excess) 
                 sprintf(result, \"mov #edspage(%s),%%3\;add %%3,#%d,%%3\;\"
                                 \"mov %%3,%%0\", o1, excess);
               else
                 sprintf(result, \"mov #edspage(%s),%%3\;mov %%3,%%0\",o1);
               break;
     }
     return result;
   }
  ")

(define_insn "movEDS_address_offset"
  [(set (match_operand:HI     0 "pic30_move_operand"             "=r,R,<>,QSTU")
        (edsoffset: HI
          (match_operand     1 "immediate_operand"              " i,i,i,i"))
   )
   (clobber (match_scratch:HI 2                                  "=X,&r,&r,&r"))
  ]
  ""
  "*
   { static char result[256];
     char *o1 = (char*)INTVAL(operands[1]);

     switch (which_alternative) {
       case 0: sprintf(result,\"mov #edsoffset(%s),%%0\", o1);
               break;
       case 1: sprintf(result,\"mov #edsoffset(%s),%%2\;mov %%2,%%0\", o1);
               break;
       case 2: sprintf(result,\"mov #edsoffset(%s),%%2\;mov %%2,%%0\", o1);
               break;
       case 3: sprintf(result,\"mov #edsoffset(%s),%%2\;mov %%2,%%0\", o1);
               break;
     }
     return result;
   }
  ")


(define_expand "movP32EDS_address"
  [(set (match_operand:P32EDS 0 "pic30_move_operand"             "=r,R,<>,QSTU")
        (match_operand:P32EDS 1 "pic30_symbolic_address_operand" " g,g,g,g")
   )
  ]
  ""
  "{ rtx excess;
     char *t = pic30_section_base(operands[1],0,&excess);
     emit_insn(
       gen_movEDS_address_offset(
         simplify_gen_subreg(HImode, operands[0], GET_MODE(operands[0]), 0),
	 GEN_INT((HOST_WIDE_INT)t))
     );
     t = pic30_section_base(operands[1],1,&excess);
     if (pic30_register_operand(operands[0], P32EDSmode)) {
       emit_insn(
         gen_edspage_helper(
           simplify_gen_subreg(HImode, operands[0], GET_MODE(operands[0]), 2), 
           GEN_INT((HOST_WIDE_INT)t), excess)
       );
     } else {
       emit_insn(
         gen_movEDS_address_page(
           simplify_gen_subreg(HImode, operands[0], GET_MODE(operands[0]), 2),
           GEN_INT((HOST_WIDE_INT)t), excess)
       );
     }
#if 0
     if (excess) {
       emit_insn(
         gen_addp32eds3(operands[0],operands[0],excess)
       );
     }
#endif
     DONE;
   }
  ")

(define_expand "movP32PEDS_address"
  [(set (match_operand:P32PEDS 0 "pic30_move_operand"            "=r,R,<>,QSTU")
        (match_operand:P32PEDS 1 "pic30_symbolic_address_operand" " g,g,g,g")
   )
  ]
  ""
  "{ char *t = pic30_section_base(operands[1],0,0);
     emit_insn(
       gen_movEDS_address_offset(
         simplify_gen_subreg(HImode, operands[0], GET_MODE(operands[0]), 0),
         GEN_INT((HOST_WIDE_INT)t))
     );
     t = pic30_section_base(operands[1],1,0);
     emit_insn(
       gen_movEDS_address_page(
         simplify_gen_subreg(HImode, operands[0], GET_MODE(operands[0]), 2),
         GEN_INT((HOST_WIDE_INT)t), GEN_INT(0))
     );
     DONE;
   } 
  ")

(define_insn "movP32DF_address"
  [(set (match_operand:P32DF 0 "pic30_move_operand"             "=r")
        (match_operand:P32DF 1 "pic30_symbolic_address_operand" "g")
   )
  ]
  ""
  "mov #packed_lo(%1),%0\;mov #packed_hi(%1),%d0"
)


(define_expand "movP32EXT_address"
  [(set (match_operand:P32EXT 0 "pic30_move_operand"             "=rR<>QSTU")
        (match_operand:P32EXT 1 "pic30_symbolic_address_operand" " g"))]
  ""
  "{
     if (pic30_address_of_external(operands[0],operands[1])) {
       DONE;
     } else FAIL;
   }
  "
)

(define_insn "movP24PROG_gen"
  [(set (match_operand:P24PROG 0 "pic30_move_operand" "=r,r,  R<>,Q,r, TU,r")
        (match_operand:P24PROG 1 "pic30_move_operand"  "r,R<>,r,  r,TU,r, Q"))]
  ""
  "*
{
        int idDst, idSrc, pre;

        switch (which_alternative)
        {
        case 0: /* r = r */
                return \"mov.d %1,%0\";
        case 1: /* r = R<> */
                return \"mov.d %1,%0\";
        case 2: /* R<> = r */
                return \"mov.d %1,%0\";
        case 3: /* Q = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 4: /* r = TU */
                return \"mov %1,%0\;mov %Q1,%d0\";
        case 5: /* TU = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 6: /* r = Q */
                return \"mov %1,%0\;mov %Q1,%d0\";
        default:
                return \";\";
        }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,etc,etc,def,etc,defuse")
  ]
)

(define_insn "movP24PSV_gen"
  [(set (match_operand:P24PSV 0 "pic30_move_operand" "=r,r,  R<>,Q,r, TU")
        (match_operand:P24PSV 1 "pic30_move_operand"  "r,R<>,r,  r,TU,r"))]
  ""
  "*
{  
        int idDst, idSrc, pre;

        switch (which_alternative)
        {
        case 0: /* r = r */
                return \"mov.d %1,%0\";
        case 1: /* r = R<> */
                return \"mov.d %1,%0\";
        case 2: /* R<> = r */
                return \"mov.d %1,%0\";
        case 3: /* Q = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 4: /* r = TU */ 
                return \"mov %1,%0\;mov %Q1,%d0\";
        case 5: /* TU = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        default:
                return \";\";
        }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse,etc,etc,def,etc")
  ]
)

(define_insn "movP32EDS_gen"
  [(set (match_operand:P32EDS 0 "pic30_move_operand" "=r,r,  R<>,Q,r, TU,r")
        (match_operand:P32EDS 1 "pic30_move_operand"  "r,R<>,r,  r,TU,r, Q"))]
  ""
  "*
{
        int idDst, idSrc, pre;

        switch (which_alternative)
        {
        case 0: /* r = r */
                return \"mov.d %1,%0\";
        case 1: /* r = R<> */
                return \"mov.d %1,%0\";
        case 2: /* R<> = r */
                return \"mov.d %1,%0\";
        case 3: /* Q = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 4: /* r = TU */
                return \"mov %1,%0\;mov %Q1,%d0\";
        case 5: /* TU = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 6: /* r = Q */
                return \"mov %1,%0\;mov %Q1,%d0\";
        default:
                return \";\";
        }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse,etc,etc,def,etc,etc")
  ]
)

(define_insn "movP32DF_gen"
  [(set (match_operand:P32DF 0 "pic30_move_operand" "=r,r,  R<>,Q,r, TU,r")
        (match_operand:P32DF 1 "pic30_move_operand"  "r,R<>,r,  r,TU,r, Q"))]
  ""
  "*
{
        int idDst, idSrc, pre;

        switch (which_alternative)
        {
        case 0: /* r = r */
                return \"mov.d %1,%0\";
        case 1: /* r = R<> */
                return \"mov.d %1,%0\";
        case 2: /* R<> = r */
                return \"mov.d %1,%0\";
        case 3: /* Q = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 4: /* r = TU */
                return \"mov %1,%0\;mov %Q1,%d0\";
        case 5: /* TU = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 6: /* r = Q */
                return \"mov %1,%0\;mov %Q1,%d0\";
        default:
                return \";\";
        }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse,etc,etc,def,etc,etc")
  ]
)

(define_insn "movP32PEDS_gen"
  [(set (match_operand:P32PEDS 0 "pic30_move_operand" "=r,r,  R<>,Q,r, TU")
        (match_operand:P32PEDS 1 "pic30_move_operand"  "r,R<>,r,  r,TU,r"))]
  ""
  "*
{  
        int idDst, idSrc, pre;

        switch (which_alternative)
        {
        case 0: /* r = r */
                return \"mov.d %1,%0\";
        case 1: /* r = R<> */
                return \"mov.d %1,%0\";
        case 2: /* R<> = r */
                return \"mov.d %1,%0\";
        case 3: /* Q = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        case 4: /* r = TU */ 
                return \"mov %1,%0\;mov %Q1,%d0\";
        case 5: /* TU = r */
                return \"mov %1,%0\;mov %d1,%Q0\";
        default:
                return \";\";
        }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse,etc,etc,def,etc")
  ]
)

(define_expand "movp24prog"
  [(set (match_operand:P24PROG 0 "pic30_general_operand" "")
        (match_operand:P24PROG 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P24PROGmode)) DONE;
}")

(define_expand "movp24psv"
  [(set (match_operand:P24PSV 0 "pic30_general_operand" "")
        (match_operand:P24PSV 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P24PSVmode)) DONE;
}")

(define_expand "movp16pmp"
  [(set (match_operand:P16PMP 0 "pic30_general_operand" "")
        (match_operand:P16PMP 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P16PMPmode)) DONE;
}")

(define_expand "movp32eds"
  [(set (match_operand:P32EDS 0 "pic30_general_operand" "")
        (match_operand:P32EDS 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P32EDSmode)) DONE;
}")

(define_expand "movp32peds"
  [(set (match_operand:P32PEDS 0 "pic30_general_operand" "")
        (match_operand:P32PEDS 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, GET_MODE(operand1))) DONE;
}")

(define_expand "movp32df"
  [(set (match_operand:P32DF 0 "pic30_general_operand" "")
        (match_operand:P32DF 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P32DFmode)) DONE;
}")

(define_expand "movp32ext"
  [(set (match_operand:P32EXT 0 "pic30_general_operand" "")
        (match_operand:P32EXT 1 "pic30_general_operand" ""))]
  ""
  "
{
   if (pic30_emit_move_sequence(operands, P32EXTmode)) DONE;
}")

(define_insn "set_vpsv"
   [(set (reg:HI PSVPAG)
         (unspec_volatile: HI
           [(match_operand:HI 0 "pic30_register_operand" "r")] UNSPECV_SETPSV))]
   ""
   "*
{
   if (pic30_ecore_target()) return \"mov %0,_DSRPAG\;nop\";
   if (pic30_eds_target()) return \"mov %0,_DSRPAG\";
   return \"mov %0,_PSVPAG\";
}"
)

(define_insn "set_nvpsv"
   [(set (reg:HI PSVPAG)
         (match_operand:HI 0 "pic30_register_operand" "r"))]
   ""
   "*
{
   if (pic30_ecore_target()) return \"mov %0,_DSRPAG\;nop\";
   if (pic30_eds_target()) return \"mov %0,_DSRPAG\";
   return \"mov %0,_PSVPAG\";
}"
)

(define_expand "set_psv"
  [(set (reg:HI PSVPAG)
        (match_operand:HI 0 "pic30_register_operand" "r"))]
  ""
  "
   if (target_flags & TARGET_TRACK_PSVPAG) {
     emit_insn(
       gen_set_nvpsv(operand0)
     );
   } else {
     emit_insn(
       gen_set_vpsv(operand0)
     );
   }
   DONE;
")


(define_insn "set_vdsw"
   [(set (reg:HI DSWPAG)
         (unspec_volatile: HI
           [(match_operand:HI 0 "pic30_register_operand" "r")] 
           UNSPECV_SETDSW))]
   "pic30_eds_target()"
   "*
{
   return \"mov %0,_DSWPAG\";
}"
)

(define_insn "set_nvdsw"
   [(set (reg:HI DSWPAG)
         (match_operand:HI 0 "pic30_register_operand" "r"))]
         
   "pic30_eds_target()"
   "*
{
   return \"mov %0,_DSWPAG\";
}"
)

(define_expand "set_dsw"
   [(set (reg:HI DSWPAG)
         (match_operand:HI 0 "pic30_register_operand" "r"))]
   "pic30_eds_target()"
   "
   if (target_flags & TARGET_TRACK_PSVPAG) {
     emit_insn(
       gen_set_nvdsw(operand0)
     );
   } else {
     emit_insn(
       gen_set_vdsw(operand0)
     );
   }
   DONE;
")


(define_insn "copy_psv"
   [(set (match_operand:HI 0 "pic30_register_operand" "=r")
         (reg:HI PSVPAG))]
   ""
   "*
{
   if (pic30_eds_target()) return \"mov _DSRPAG,%0\";
   return \"mov _PSVPAG,%0\";
}"
)

(define_insn "copy_dsw"
   [(set (match_operand:HI 0 "pic30_register_operand" "=r")
         (reg:HI DSWPAG))]
   "pic30_eds_target()"
   "*
{
   return \"mov _DSWPAG,%0\";
}"
)

(define_insn "set_PMADDR_DATA"
  [(set (reg:P16PMP PMADDR)
        (match_operand: P16PMP 0 "pic30_mode2_operand" "r,R<>"))]
  ""
  "@
   mov %0,_PMADDR
   push %0\;pop _PMADDR"
)

(define_insn "set_PMADDR_APSV"
  [(set (reg:P16PMP PMADDR)
        (match_operand: P16PMP 0 "pic30_mode2_APSV_operand" "r,R<>"))]
  ""
  "@
   mov %0,_PMADDR
   push %0\;pop _PMADDR"
)

(define_expand "set_PMADDR"
  [(set (reg:P16PMP PMADDR)
        (match_operand: P16PMP 0 "pic30_mode2_APSV_operand" "r,R<>"))]
  ""
  "
{
  if (pic30_mode2_operand(operands[0], GET_MODE(operands[0]))) 
    emit(gen_set_PMADDR_DATA(operands[0]));
  else
    emit(gen_set_PMADDR_APSV(operands[0]));
  DONE;
}")

(define_insn "while_PMMODE_busy"
  [
   (unspec_volatile:HI  [(const_int 0)] UNSPECV_WHILEPMMODE)
   (use (reg:P16PMP PMADDR))
  ]
  ""
  "btsc _PMMODE, #15\;bra . - 2"
)

(define_insn "bitset_PMMODE"
  [ 
   (set (reg:HI PMMODE)
        (unspec_volatile:HI 
          [(reg:HI PMMODE)
           (match_operand 0 "const_int_operand"      "i")] UNSPECV_SETPMMODE))
  ]
  "((INTVAL(operands[0]) >= 0) && (INTVAL(operands[0]) <= 15))"
  "bset _PMMODE,#%0"
)

(define_insn "bitclr_PMMODE"
  [ 
   (set (reg:HI PMMODE)
        (unspec_volatile:HI 
          [(reg:HI PMMODE)
           (match_operand 0 "const_int_operand"      "i")] UNSPECV_CLRPMMODE))
   (clobber (reg:P16PMP PMADDR))
  ]
  "((INTVAL(operands[0]) >= 0) && (INTVAL(operands[0]) <= 15))"
  "bclr _PMMODE,#%0"
)

(define_insn "get_PMDIN1qi"
  [
   (set (match_operand:QI 0 "pic30_mode2_operand" "=r,R<>")
        (unspec_volatile:QI [ (reg:QI PMDIN1)
                              (reg:P16PMP PMADDR) ] UNSPECV_PMDIN1RD))
  ]
  ""
  "@
   mov _PMDIN1,%0
   push _PMDIN1\;pop %0"
)

(define_insn "get_PMDIN1hi"
  [
   (set (match_operand: HI 0 "pic30_mode2_operand" "=r,R<>")
        (unspec_volatile:HI [ (reg:HI PMDIN1)
                              (reg:P16PMP PMADDR) ] UNSPECV_PMDIN1RD))
  ]
  ""
  "@
   mov _PMDIN1,%0
   push _PMDIN1\;pop %0"
)

(define_insn "set_PMDIN1qi_DATA"
  [
   (set (reg:QI PMDIN1)
        (unspec_volatile:QI [ 
          (match_operand:QI 0 "pic30_mode2_operand" "r,R<>")
          (reg:P16PMP PMADDR) 
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "@
   mov %0,_PMDIN1
   push %0\;pop _PMDIN1"
)

(define_insn "set_PMDIN1qi_APSV"
  [
   (set (reg:QI PMDIN1)
        (unspec_volatile:QI [ 
          (match_operand:QI 0 "pic30_mode2_APSV_operand" "r,R<>")
          (reg:P16PMP PMADDR) 
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "@
   mov %0,_PMDIN1
   push %0\;pop _PMDIN1"
)

(define_expand "set_PMDIN1qi"
  [
   (set (reg:QI PMDIN1)
        (unspec_volatile:QI [
          (match_operand:QI 0 "pic30_mode2_APSV_operand" "r,R<>")
          (reg:P16PMP PMADDR)
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "
{
  if (pic30_mode2_operand(operands[0], GET_MODE(operands[0]))) 
    emit(gen_set_PMDIN1qi_DATA(operands[0]));
  else
    emit(gen_set_PMDIN1qi_APSV(operands[0]));
  DONE;
}")

(define_insn "set_PMDIN1hi_DATA"
  [
   (set (reg:HI PMDIN1)
        (unspec_volatile:HI [ 
          (match_operand:HI 0 "pic30_mode2_operand" "r,R<>")
          (reg:P16PMP PMADDR) 
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "@
   mov %0,_PMDIN1
   push %0\;pop _PMDIN1"
) 

(define_insn "set_PMDIN1hi_APSV"
  [
   (set (reg:HI PMDIN1)
        (unspec_volatile:HI [ 
          (match_operand:HI 0 "pic30_mode2_APSV_operand" "r,R<>")
          (reg:P16PMP PMADDR) 
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "@
   mov %0,_PMDIN1
   push %0\;pop _PMDIN1"
) 

(define_expand "set_PMDIN1hi"
  [
   (set (reg:HI PMDIN1)
        (unspec_volatile:HI [
          (match_operand:HI 0 "pic30_mode2_APSV_operand" "r,R<>")
          (reg:P16PMP PMADDR)
        ] UNSPECV_PMDIN1WT))
  ]
  ""
  "
{
  if (pic30_mode2_operand(operands[0], GET_MODE(operands[0]))) 
    emit(gen_set_PMDIN1hi_DATA(operands[0]));
  else
    emit(gen_set_PMDIN1hi_APSV(operands[0]));
  DONE;
}")


(define_insn "rotlhi_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotate:HI (match_operand:HI 1 "pic30_mode2_operand"  "rR<>")
                   (const_int 1)))]
  ""
  "rlnc %1,%0"
  [ 
    (set_attr "cc" "math") 
  ]
)

(define_insn "rotlhi_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotate:HI (match_operand:HI 1 "pic30_mode2_APSV_operand"  "rR<>")
                   (const_int 1)))]
  ""
  "rlnc %1,%0"
  [ 
    (set_attr "cc" "math") 
  ]
)

(define_expand "rotlhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotatert:HI (match_operand:HI 1 "pic30_mode2_APSV_operand" "rR<>")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  ""
  "
{  if (INTVAL(operands[2]) == 1) {
     if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
       emit_insn(gen_rotlhi_DATA(operands[0], operands[1]));
     else
       emit_insn(gen_rotlhi_APSV(operands[0], operands[1]));
     DONE;
   } else FAIL;
}")

(define_insn "rotrhi_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotatert:HI (match_operand:HI 1 "pic30_mode2_operand" "rR<>")
                     (const_int 1)))]
  ""
  "rrnc %1,%0"
  [ 
    (set_attr "cc" "math") 
  ]
)

(define_insn "rotrhi_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotatert:HI (match_operand:HI 1 "pic30_mode2_APSV_operand" "rR<>")
                     (const_int 1)))]
  ""
  "rrnc %1,%0"
  [ 
    (set_attr "cc" "math") 
  ]
)

(define_expand "rotrhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (rotatert:HI (match_operand:HI 1 "pic30_mode2_APSV_operand" "rR<>")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  ""
  "
{  if (INTVAL(operands[2]) == 1) {
     if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
       emit_insn(gen_rotrhi_DATA(operands[0], operands[1]));
     else
       emit_insn(gen_rotrhi_APSV(operands[0], operands[1]));
     DONE;
   } else FAIL;
}")

;
; 24-bit unpack instruction
;   a pointer is 23..15.....0
;                page  offset
;   but for PSV access it must be
;                24..16 15 14...0
;                page   1  offset

(define_insn "unpack_EDS"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "=r")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_register_operand" "0")]
            UNSPECV_UNPACKEDS))]
  ""
  "btss %0,#15\;inc %d0,%d0"
  [
    (set_attr "cc" "clobber")
  ]
)

(define_insn "unpack_MPSV_DATA"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI 
           [(match_operand:P24PSV 1 "pic30_mode2_operand" "0,0,r,R,r,R")] 
             UNSPECV_UNPACKMPSV))]
   ""
   "@
    btsts.c %0,#15\;rlc %d0,%d0
    btsts.c %I0,#15\;rlc %0,%D0
    mov %1,%0\;btsts.c %0,#15\;rlc %d1,%d0
    mov %I1,%0\;btsts.c %0,#15\;rlc %D1,%d0
    mov %1,%0\;btsts.c %I0,#15\;rlc %d1,%D0
    mov %I1,%0\;btsts.c %I0,#15\;rlc %D1,%D0"
   [
     (set_attr "cc" "clobber")
   ]
)

(define_insn "unpack_MPSV_APSV"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI 
           [(match_operand:P24PSV 1 "pic30_mode2_APSV_operand" "0,0,r,R,r,R")] 
             UNSPECV_UNPACKMPSV))]
   ""
   "@
    btsts.c %0,#15\;rlc %d0,%d0
    btsts.c %I0,#15\;rlc %0,%D0
    mov %1,%0\;btsts.c %0,#15\;rlc %d1,%d0
    mov %I1,%0\;btsts.c %0,#15\;rlc %D1,%d0
    mov %1,%0\;btsts.c %I0,#15\;rlc %d1,%D0
    mov %I1,%0\;btsts.c %I0,#15\;rlc %D1,%D0"
   [
     (set_attr "cc" "clobber")
   ]
)

(define_expand "unpack_MPSV"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI
           [(match_operand:P24PSV 1 "pic30_mode2_APSV_operand" "0,0,r,R,r,R")]
             UNSPECV_UNPACKMPSV))]
   ""
   "
{
   if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
     emit(gen_unpack_MPSV_DATA(operands[0],operands[1]));
   else
     emit(gen_unpack_MPSV_APSV(operands[0],operands[1]));
   DONE;
}")

(define_insn "unpack_MPROG_DATA"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI 
           [(match_operand:P24PROG 1 "pic30_mode2_operand" "0,0,r,R,r,R")] 
             UNSPECV_UNPACKMPROG))]
   ""
   "@
    btsts.c %0,#15\;rlc %d0,%d0
    btsts.c %I0,#15\;rlc %0,%D0
    mov %1,%0\;btsts.c %0,#15\;rlc %d1,%d0
    mov %I1,%0\;btsts.c %0,#15\;rlc %D1,%d0
    mov %1,%0\;btsts.c %I0,#15\;rlc %d1,%D0
    mov %I1,%0\;btsts.c %I0,#15\;rlc %D1,%D0"
   [
     (set_attr "cc" "clobber")
   ]
)

(define_insn "unpack_MPROG_APSV"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI 
           [(match_operand:P24PROG 1 "pic30_mode2_APSV_operand" "0,0,r,R,r,R")] 
             UNSPECV_UNPACKMPROG))]
   ""
   "@
    btsts.c %0,#15\;rlc %d0,%d0
    btsts.c %I0,#15\;rlc %0,%D0
    mov %1,%0\;btsts.c %0,#15\;rlc %d1,%d0
    mov %I1,%0\;btsts.c %0,#15\;rlc %D1,%d0
    mov %1,%0\;btsts.c %I0,#15\;rlc %d1,%D0
    mov %I1,%0\;btsts.c %I0,#15\;rlc %D1,%D0"
   [
     (set_attr "cc" "clobber")
   ]
)

(define_expand "unpack_MPROG"
   [(set (match_operand:SI 0 "pic30_mode2_operand"       "=r,R,r,r,R,R")
         (unspec_volatile:SI
           [(match_operand:P24PROG 1 "pic30_mode2_APSV_operand" "0,0,r,R,r,R")]
             UNSPECV_UNPACKMPROG))]
   ""
   "
{
  if (pic30_mode2_operand(operands[1],GET_MODE(operands[1]))) 
    emit(gen_unpack_MPROG_DATA(operands[0],operands[1]));
  else
    emit(gen_unpack_MPROG_APSV(operands[0],operands[1]));
  DONE;
}")

;
;  24-bit read instructions:
;
;    [PTRMODE]read_[READMODE]
;
;    P24PROGread_hi  (read a HI via a P24PROG pointer)
;
(define_insn "P24PROGread_qi"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:QI
          [(mem:QI (match_operand:HI 1 "pic30_register_operand" "r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
  ]
  ""
  "mov.b [%1],%0"
)

(define_insn "P24PROGread_HI"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:HI
          [(mem:HI (match_operand:HI 1 "pic30_register_operand" "r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
  ]
  ""
  "mov [%1],%0"
)

(define_insn "P24PROGread_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16APSV
          [(mem:P16APSV (match_operand:HI 1 "pic30_register_operand" "r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
  ]
  ""
  "mov [%1],%0"
)

(define_insn "P24PROGread_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16PMP
          [(mem:P16PMP (match_operand:HI 1 "pic30_register_operand" "r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
  ]
  ""
  "mov [%1],%0"
)

(define_insn "P24PROGread_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(mem:SI (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\" 
                   };
   char *edsresults[] = { 
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\" 
                   };

   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(mem:SI (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PROGread_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(mem:P32EDS (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };

   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PROGread_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(mem:P32PEDS (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };

   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PROGread_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(mem:P32EXT (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(mem:P32EDS (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PSVread_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(mem:P32PEDS (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PSVread_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(mem:P32EXT (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PROGread_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(mem:SF (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = { 
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(mem:SF (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PROGread_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(mem:P24PROG 
             (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(mem:P24PROG (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PROGread_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(mem: P24PSV 
              (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%d0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1],%D0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(mem:P24PSV (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1--],%d0
   mov [%1++],%I0\;mov [%1--],%D0"
)

(define_insn "P24PROGread_di"
  [(set (match_operand:DI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DI
          [(mem:DI (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1++],%d0\;btss %1,#15\;\"
                         \"inc _PSVPAG\;bset %1,#15\;mov [%1++],%t0\;\"
                         \"btss %1,#15\;inc _PSVPAG\;bset %1,#15\;\"
                         \"mov [%1],%q0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                         \"bset %1,#15\;mov [%1++],%I0\;btss %1,#15\;\"
                         \"inc _PSVPAG\;bset %1,#15\;mov [%1++],%I0\;\"
                         \"btss %1,#15\;inc _PSVPAG\;bset %1,#15\;mov [%1],%0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1++],%d0\;btss %1,#15\;\"
                         \"inc _DSRPAG\;bset %1,#15\;mov [%1++],%t0\;\"
                         \"btss %1,#15\;inc _DSRPAG\;bset %1,#15\;\"
                         \"mov [%1],%q0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                         \"bset %1,#15\;mov [%1++],%I0\;btss %1,#15\;\"
                         \"inc _DSRPAG\;bset %1,#15\;mov [%1++],%I0\;\"
                         \"btss %1,#15\;inc _PSVPAG\;bset %1,#15\;mov [%1],%0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_di"
  [(set (match_operand:DI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DI
          [(mem:DI (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
   (clobber (match_dup 1))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1++],%d0\;mov [%1++],%t0\;mov [%1],%q0
   mov [%1++],%I0\;mov [%1++],%I0\;mov [%1++],%I0\;mov [%1],%0"
)

(define_insn "P24PROGread_df"
  [(set (match_operand:DF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DF
          [(mem:DF (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPROG))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _PSVPAG\;\"
                        \"bset %1,#15\;mov [%1++],%d0\;btss %1,#15\;\"
                        \"inc _PSVPAG\;bset %1,#15\;mov [%1++],%t0\;\"
                        \"btss %1,#15\;inc _PSVPAG\;bset %1,#15\;mov [%1],%q0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _PSVPAG\;\"
                        \"bset %1,#15\;mov [%1++],%I0\;btss %1,#15\;\"
                        \"inc _PSVPAG\;bset %1,#15\;mov [%1++],%I0\;\"
                        \"btss %1,#15\;inc _PSVPAG\;bset %1,#15\;mov [%1],%0\"
                   };
   char *edsresults[] = {
                     \"mov [%1++],%0\;btss %1,#15\;inc _DSRPAG\;\"
                        \"bset %1,#15\;mov [%1++],%d0\;btss %1,#15\;\"
                        \"inc _DSRPAG\;bset %1,#15\;mov [%1++],%t0\;\"
                        \"btss %1,#15\;inc _DSRPAG\;bset %1,#15\;mov [%1],%q0\",
                     \"mov [%1++],%I0\;btss %1,#15\;inc _DSRPAG\;\"
                        \"bset %1,#15\;mov [%1++],%I0\;btss %1,#15\;\"
                        \"inc _DSRPAG\;bset %1,#15\;mov [%1++],%I0\;\"
                        \"btss %1,#15\;inc _DSRPAG\;bset %1,#15\;mov [%1],%0\"
                   };
   if (pic30_eds_target()) return edsresults[which_alternative];
   return results[which_alternative];
}"
)

(define_insn "P24PSVread_df"
  [(set (match_operand:DF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DF
          [(mem:DF (match_operand:HI 1 "pic30_register_operand" "r,r"))
           (reg:HI PSVPAG)]
          UNSPECV_PSVRDPSV))
   (clobber (match_dup 1))
  ]
  ""
  "@
   mov [%1++],%0\;mov [%1++],%d0\;mov [%1++],%t0\;mov [%1],%q0
   mov [%1++],%I0\;mov [%1++],%I0\;mov [%1++],%I0\;mov [%1],%0"
)

; EDSread

(define_insn "P32EDSread_qi"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:QI
          [(match_operand:QI 1 "pic30_mode2_operand" "rR<>")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "mov.b %1,%0"
)

(define_insn "P32EDSread_HI"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:HI
          [(match_operand:HI 1 "pic30_mode2_operand" "Rr<>")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32EDSread_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16APSV
          [(match_operand:P16APSV 1 "pic30_mode2_operand" "Rr<>")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32EDSread_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16PMP
          [(match_operand:P16PMP 1 "pic30_mode2_operand" "Rr<>")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "mov %1,%0"
)

; The general process for incrementing an EDS pointer (PAGE:offset) in 
;   current devices is
;
;  mov x, [offset]            ; read (or write) data
;  inc2 offset, offset        ; increment for next offset
;  bra nc,1f                  ; carry clear we are finished
;  bset offset,#15            ; put us back into EDS range
;  inc PAGE                   ; update PAGE
;  1:

(define_insn "P32EDSread_eds_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_si(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_si(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_sf(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_sf(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_P24PROG(operands[0],XEXP(operands[1],0)));
  } else  {
    emit(gen_P32EDSread_noeds_P24PROG(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_P24PSV(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_P24PSV(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_P32EDS(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_P32EDS(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_P32PEDS(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_P32PEDS(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSread_noeds_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(match_operand:HI 1 "pic30_register_operand" " r,r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%d0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: mov [%1],%D0\"
                   };

   return results[which_alternative];
}"
)

(define_expand "P32EDSread_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_R_operand" " R,R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))]
  ""
  "
{
  if (pic30_eds_target()) {
    emit(gen_P32EDSread_eds_P32EXT(operands[0],XEXP(operands[1],0)));
  } else {
    emit(gen_P32EDSread_noeds_P32EXT(operands[0],XEXP(operands[1],0)));
  }
  DONE;
}")

(define_insn "P32EDSread_eds_di"
  [(set (match_operand:DI 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DI
          [(match_operand:HI 1 "pic30_register_operand" " r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = {
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%d0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%t0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%q0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%0\"
                   };

   return edsresults[0];
}"
)

(define_insn "P32EDSread_noeds_di"
  [(set (match_operand:DI 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DI
          [(match_operand:HI 1 "pic30_register_operand" " r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%d0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%t0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%q0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%0\"
                   };

   return results[0];
}"
)

(define_expand "P32EDSread_di"
  [(set (match_operand:DI 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_R_operand" " R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSread_eds_di(operands[0],XEXP(operands[1],0)));
   } else {
     emit(gen_P32EDSread_noeds_di(operands[0],XEXP(operands[1],0)));
   }
   DONE;
}"
)

(define_insn "P32EDSread_eds_df"
  [(set (match_operand:DF 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DF
          [(match_operand:HI 1 "pic30_register_operand" " r")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = {
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%d0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%t0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%q0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _DSRPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%0\"
                   };

   return edsresults[0];
}"
)

(define_insn "P32EDSread_noeds_df"
  [(set (match_operand:DF 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DF
          [(match_operand:HI 1 "pic30_register_operand" " r")
           (reg:HI PSVPAG)]
          UNSPECV_NOEDSRD))
   (clobber (reg:HI PSVPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = {
                     \"mov [%1],%0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%d0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%t0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%q0\",
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L1_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L1_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L2_%=\;\"
                         \"bset %1,#15\;inc _PSVPAG\;\"
                         \".L2_%=: \"
                     \"mov [%1],%I0\;inc2 %1,%1\;bra nc,.L3_%=\;\"
                         \"bset %r1,#15\;inc _PSVPAG\;\"
                         \".L3_%=: \"
                     \"mov [%1],%0\"
                   };

   return results[0];
}"
)

(define_expand "P32EDSread_df"
  [(set (match_operand:DF 0 "pic30_register_operand" "=&r")
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_R_operand" " R")
           (reg:HI PSVPAG)]
          UNSPECV_EDSRD))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSread_eds_df(operands[0],XEXP(operands[1],0)));
   } else {
     emit(gen_P32EDSread_noeds_df(operands[0],XEXP(operands[1],0)));
   }
   DONE;
}"
)

; P32EDSwrite

(define_insn "P32EDSwrite_qi"
  [(set (match_operand:QI 0 "pic30_R_operand" "=R")
        (unspec_volatile:QI
          [(match_operand:QI 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "mov.b %1,%0"
)
(define_insn "P32EDSwrite_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_R_operand" "=R")
        (unspec_volatile:P16PMP
          [(match_operand:P16PMP 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32EDSwrite_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_R_operand" "=R")
        (unspec_volatile:P16APSV
          [(match_operand:P16APSV 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32EDSwrite_HI"
  [(set (match_operand:HI 0 "pic30_R_operand" "=R")
        (unspec_volatile:HI
          [(match_operand:HI 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32EDSwrite_eds_si"
  [(set (mem:SI (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_si"
  [(set (match_operand:SI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_si"
  [(set (match_operand:SI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_si(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_si(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_sf"
  [(set (mem:SF (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_sf"
  [(set (match_operand:SF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_sf"
  [(set (match_operand:SF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_sf(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_sf(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_P24PROG"
  [(set (mem:P24PROG (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,1.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_P24PROG(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_P24PROG(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_P24PSV"
  [(set (mem:P24PSV (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_P24PSV(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_P24PSV(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_P32EDS"
  [(set (mem:P32EDS (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_P32EDS(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_P32EDS(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_P32PEDS"
  [(set (mem:P32PEDS (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_P32PEDS(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_P32PEDS(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_P32EXT"
  [(set (mem:P32EXT (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
  ]
  ""
  "*
{
   char *edsresults[] = { 
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %d1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: mov %D1,[%0]\",
                   };
   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
  ]
  ""
  "*
{
   char *results[] = { \"mov %1,%I0\;mov %d1,%D0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };
                     
   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_P32EXT(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_P32EXT(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_di"
  [(set (mem:DI (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = {
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: \"
                     \"mov %d1,[%0]\;inc2 %0,%0\;bra nc,.L2_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L2_%=: \"
                     \"mov %t1,[%0]\;inc2 %0,%0\;bra nc,.L3_%=\;\"
                         \"bset [%0],#15\;inc _DSWPAG\;\"
                         \".L3_%=: \"
                     \"mov %q1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: \"
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L2_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L2_%=: \"
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L3_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L3_%=: \"
                     \"mov %1,[%0]\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_di"
  [(set (match_operand:DI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%I0\;mov.d %t1,%D0\",
                       \"mov %I1,%IO\;mov %I1,%IO\;mov %I1,%IO\;mov %I1,%IO\" };

   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_di"
  [(set (match_operand:DI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_di(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_di(operands[0],operands[1]));
   }
   DONE;
}"
)

(define_insn "P32EDSwrite_eds_df"
  [(set (mem:DF (match_operand:HI 0 "pic30_register_operand" "r,r"))
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *edsresults[] = {
                     \"mov %1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: \"
                     \"mov %d1,[%0]\;inc2 %0,%0\;bra nc,.L2_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L2_%=: \"
                     \"mov %t1,[%0]\;inc2 %0,%0\;bra nc,.L3_%=\;\"
                         \"bset [%0],#15\;inc _DSWPAG\;\"
                         \".L3_%=: \"
                     \"mov %q1,[%0]\",
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L1_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L1_%=: \"
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L2_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L2_%=: \"
                     \"mov %I1,[%0]\;inc2 %0,%0\;bra nc,.L3_%=\;\"
                         \"bset %0,#15\;inc _DSWPAG\;\"
                         \".L3_%=: \"
                     \"mov %1,[%0]\"
                   };

   return edsresults[which_alternative];
}"
)

(define_insn "P32EDSwrite_noeds_df"
  [(set (match_operand:DF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_mode2_operand" "r,R")]
          UNSPECV_EDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 0))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%I0\;mov.d %t1,%D0\",
                       \"mov %I1,%IO\;mov %I1,%IO\;mov %I1,%IO\;mov %I1,%IO\" };

   return results[which_alternative];
}"
)

(define_expand "P32EDSwrite_df"
  [(set (match_operand:DF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_EDSWT))
  ]
  ""
  "
{
   if (pic30_eds_target()) {
     emit(gen_P32EDSwrite_eds_df(XEXP(operands[0],0),operands[1]));
   } else {
     emit(gen_P32EDSwrite_noeds_df(operands[0],operands[1]));
   }
   DONE;
}"
)

; PEDSread

(define_insn "P32PEDSread_qi"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:QI
          [(match_operand:QI 1 "pic30_R_operand" "R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "mov.b %1,%0"
)

(define_insn "P32PEDSread_HI"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:HI
          [(match_operand:HI 1 "pic30_R_operand" "R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32PEDSread_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16APSV
          [(match_operand:P16APSV 1 "pic30_R_operand" "R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32PEDSread_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_mode2_operand" "=rR<>")
        (unspec_volatile:P16PMP
          [(match_operand:P16PMP 1 "pic30_R_operand" "R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "mov %1,%0"
)

; The general process for incrementing an PEDS pointer (PAGE:offset) in 
;   current devices is straightforward as the data they are pointing to 
;   does not cross a page

(define_insn "P32PEDSread_si"
  [(set (match_operand:SI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };
   
   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_sf"
  [(set (match_operand:SF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov %I1,%0\;mov %D1,%d0\",
                     \"mov %I1,%I0\;mov %D1,%D0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_di"
  [(set (match_operand:DI 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov.d %I1,%0\;mov.d %D1,%t0\",
                     \"mov.d %I1,%I0\;mov.d %I1,%I0\;\" 
                     \"mov.d %I1,%I0\;mov.d %1,%0\",
                   };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSread_df"
  [(set (match_operand:DF 0 "pic30_mode2_operand" "=&r,R")
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_R_operand" "R,R")
           (reg:HI PSVPAG)]
          UNSPECV_PEDSRD))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { 
                     \"mov.d %I1,%0\;mov.d %D1,%t0\",
                     \"mov.d %I1,%I0\;mov.d %I1,%I0\;\" 
                     \"mov.d %I1,%I0\;mov.d %I1,%0\",
                   };

   return results[which_alternative];
}"
)

; P32PEDSwrite

(define_insn "P32PEDSwrite_qi"
  [(set (match_operand:QI 0 "pic30_R_operand" "=R")
        (unspec_volatile:QI
          [(match_operand:QI 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
  ]
  ""
  "mov.b %1,%0"
)
(define_insn "P32PEDSwrite_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_R_operand" "=R")
        (unspec_volatile:P16PMP
          [(match_operand:P16PMP 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32PEDSwrite_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_R_operand" "=R")
        (unspec_volatile:P16APSV
          [(match_operand:P16APSV 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
  ]
  ""
  "mov %1,%0"
)

(define_insn "P32PEDSwrite_HI"
  [(set (match_operand:HI 0 "pic30_R_operand" "=R")
        (unspec_volatile:HI
          [(match_operand:HI 1 "pic30_mode2_operand" "rR<>")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
  ]
  ""
  "mov %1,%0"
)

; P32PEDS does not cross a page boundary

(define_insn "P32PEDSwrite_si"
  [(set (match_operand:SI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SI
          [(match_operand:SI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_sf"
  [(set (match_operand:SF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:SF
          [(match_operand:SF 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PROG
          [(match_operand:P24PROG 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P24PSV
          [(match_operand:P24PSV 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EDS
          [(match_operand:P32EDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32PEDS
          [(match_operand:P32PEDS 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:P32EXT
          [(match_operand:P32EXT 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%0\",
                       \"mov %I1,%I0\;mov %D1,%D0\" };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_di"
  [(set (match_operand:DI 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DI
          [(match_operand:DI 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%I0\;mov.d %t1,%D0\",
                       \"mov.d %I1,%I0\;mov.d %I1,%I0\;\"
                       \"mov.d %I1,%I0\;mov.d %1,%0\"
                     };

   return results[which_alternative];
}"
)

(define_insn "P32PEDSwrite_df"
  [(set (match_operand:DF 0 "pic30_R_operand" "=R,R")
        (unspec_volatile:DF
          [(match_operand:DF 1 "pic30_mode2_operand" "r,R")
           (reg:HI DSWPAG)]
          UNSPECV_PEDSWT))
   (clobber (reg:HI DSWPAG))
   (clobber (match_dup 1))
  ]
  ""
  "*
{
   char *results[] = { \"mov.d %1,%I0\;mov.d %t1,%D0\",
                       \"mov.d %I1,%I0\;mov.d %I1,%I0\;\"
                       \"mov.d %I1,%I0\;mov.d %1,%0\"
                     };

   return results[which_alternative];
}"
)

;
; funky addition
;   shift up low words first so that we don't have to worry about PSV enable bit
;   add
;   check for upper word being zero
;   zero -> 0 in upper bit 1 otherwise ... set carry 
;   shift it in

(define_insn "addp32eds3_lit"
  [(set (match_operand: P32EDS   0 "pic30_register_operand" "=r,r")
        (plus: P32EDS
          (match_operand:P32EDS  1 "pic30_register_operand" "%r,0")
          (match_operand:P32EDS  2 "pic30_OJM_operand"       "J,M")))]
  ""
  "@
   sl %1,%0\;add #%o2,%0\;bset _SR,#1\;addc %d1,#0,%d0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0
   sl %1,%0\;sub #%O2,%0\;bset _SR,#1\;subb %d1,#0,%d0\;bclr _SR,#0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0"
)

(define_insn "addp32eds3_r"
  [(set (match_operand: P32EDS   0 "pic30_register_operand" "=r,&r")
        (plus: P32EDS
          (match_operand:P32EDS  1 "pic30_register_operand" "%r,0")
          (match_operand:P32EDS  2 "pic30_mode2_operand"    "r,R")))
   (clobber (match_scratch:HI 3                             "=&r,&r"))
   (clobber (match_scratch:HI 4                             "=&r,&r"))
  ]
  ""
  "@
   sl %1,%3\;sl %2,%4\;add %3,%4,%0\;bset _SR,#1\;addc %d1,%d2,%d0\;bclr _SR,#0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0
   sl %1,%3\;sl %I2,%4\;add %3,%4,%0\;bset _SR,#1\;addc %d1,%D2,%d0\;bclr _SR,#0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0"
)

(define_expand "addp32eds3"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "")
        (plus: P32EDS (match_operand:P32EDS 1 "pic30_register_operand" "")
                      (match_operand:P32EDS 2 "pic30_mode1i_operand" "")))]
  ""
  "
{  rtx (*gen)(rtx,rtx,rtx) = gen_addp32eds3_r;

   if (GET_CODE(operands[2]) == CONST_INT) {
#if 0
     int val = INTVAL(operands[2]);
     rtx new_val = GEN_INT(val << 1);
#endif
     rtx new_val = operands[2];
     if (pic30_OJM_operand(new_val, HImode)) {
       emit_insn(gen_addp32eds3_lit(operand0,operand1,operand2));
       DONE;
     } else {
       /* load the immediate separately */
       rtx result = gen_reg_rtx(P32EDSmode);

       emit_insn(gen_movP32EDS_lit(result, new_val));
       emit_insn(gen(operand0, operand1, result));
       DONE;
     }
   }
   emit_insn(gen(operand0,operand1,operand2));
   DONE;
}")

;
;  Paged arithmetic doesn't cross a page boundary, add low part only
;

; (define_insn "addp32peds3"
;   [(set (match_operand: P32PEDS   0 "pic30_register_operand" "=r,r,r ,r,r,r")
;         (plus: P32PEDS
;           (match_operand:P32PEDS  1 "pic30_register_operand" "%r,0,r ,r,0,r")
;           (match_operand:P32PEDS  2 "pic30_rR_or_JN_operand" " P,J,JM,N,M,rR")))]
;   ""
;   "@
;    add %1,#%2,%0
;    add #%2,%0
;    mov #%2,%0\;add %1,%0,%0
;    sub %1,#%J2,%0
;    sub #%J2,%0
;    add %1,%2,%0"
; )

(define_expand "addp32peds3"
  [(set (match_operand: P32PEDS   0 "pic30_register_operand" "=r,r,r ,r,r,r")
        (plus: P32PEDS
          (match_operand:P32PEDS  1 "pic30_register_operand" "%r,0,r ,r,0,r")
          (match_operand:P32PEDS  2 "pic30_rR_or_JN_operand" " P,J,JM,N,M,rR"))
   )]
  ""
  "
{
   rtx op0_low = gen_rtx_SUBREG(HImode, operands[0],0);
   rtx op1_low = gen_rtx_SUBREG(HImode, operands[1],0);
   rtx op2_low;
   rtx op0_high = gen_rtx_SUBREG(HImode, operands[0],2);
   rtx op1_high = gen_rtx_SUBREG(HImode, operands[1],2);
   rtx op2_high;

   if (pic30_R_operand(operands[2], GET_MODE(operands[2]))) {
     rtx temp = gen_reg_rtx(P32PEDSmode);

     emit_move_insn(temp,operands[2]);
     op2_low = gen_rtx_SUBREG(HImode, temp, 0);
     op2_high = gen_rtx_SUBREG(HImode, temp, 2);
   } else if (pic30_JN_operand(operands[2], VOIDmode)) {
     int value = INTVAL(operands[2]);
     int sign = (value < 0 ? -1 : 1);

     value *= sign;

     op2_low = gen_rtx_CONST_INT(HImode, (value & 0xFFFF)*sign);
     op2_high = gen_rtx_CONST_INT(HImode, (value >> 16)*sign);
   } else {
     op2_low = gen_rtx_SUBREG(HImode, operands[2],0);
     op2_high = gen_rtx_SUBREG(HImode, operands[2],2);
   }

   emit(
     gen_addhi3(op0_low, op1_low, op2_low)
   );
   emit(
     gen_addhi3(op0_high, op1_high, op2_high)
   );
   DONE;
}")

(define_insn "addp24prog3_DATA"
  [(set (match_operand:P24PROG    0 
             "pic30_mode2_operand"    "=r,r,r,r,r,<>,<>,<>,<>,<>,R,R,R,R,R")
        (plus:P24PROG 
           (match_operand:P24PROG 1 
             "pic30_register_operand" "%r,r,r,r,r,r,r,r,r,r,r,r,r,r,r")
           (match_operand:P24PROG 2 
             "pic30_math_operand"     " r,<>,R,N,P,r,<>,R,N,P,r,<>,R,N,P")))]
  ""
  "@
   add %1,%2,%0\;addc %d1,%d2,%d0
   add %1,%2,%0\;addc %d1,%2,%d0
   add %1,%I2,%0\;addc %d1,%D2,%d0
   sub %1,#%J2,%0\;subb %d1,#0,%d0
   add %1,#%2,%0\;addc %d1,#0,%d0
   add %1,%2,%0\;addc %d1,%d2,%0
   add %1,%2,%0\;addc %d1,%2,%0
   add %1,%I2,%0\;addc %d1,%D2,%0
   sub %1,#%J2,%0\;subb %d1,#0,%0
   add %1,#%2,%0\;addc %d1,#0,%0
   add %1,%2,%I0\;addc %d1,%d2,%D0
   add %1,%2,%I0\;addc %d1,%2,%D0
   add %1,%I2,%I0\;addc %d1,%D2,%D0
   sub %1,#%J2,%I0\;subb %d1,#0,%D0
   add %1,#%2,%I0\;addc %d1,#0,%D0"
 [(set_attr "cc" "math")]
)

(define_insn "addp24prog3_APSV"
  [(set (match_operand:P24PROG    0 
             "pic30_mode2_operand"    "=r,r,r,r,r,<>,<>,<>,<>,<>,R,R,R,R,R")
        (plus:P24PROG 
           (match_operand:P24PROG 1 
             "pic30_register_operand" "%r,r,r,r,r,r,r,r,r,r,r,r,r,r,r")
           (match_operand:P24PROG 2 
             "pic30_math_APSV_operand" " r,<>,R,N,P,r,<>,R,N,P,r,<>,R,N,P")))]
  ""
  "@
   add %1,%2,%0\;addc %d1,%d2,%d0
   add %1,%2,%0\;addc %d1,%2,%d0
   add %1,%I2,%0\;addc %d1,%D2,%d0
   sub %1,#%J2,%0\;subb %d1,#0,%d0
   add %1,#%2,%0\;addc %d1,#0,%d0
   add %1,%2,%0\;addc %d1,%d2,%0
   add %1,%2,%0\;addc %d1,%2,%0
   add %1,%I2,%0\;addc %d1,%D2,%0
   sub %1,#%J2,%0\;subb %d1,#0,%0
   add %1,#%2,%0\;addc %d1,#0,%0
   add %1,%2,%I0\;addc %d1,%d2,%D0
   add %1,%2,%I0\;addc %d1,%2,%D0
   add %1,%I2,%I0\;addc %d1,%D2,%D0
   sub %1,#%J2,%I0\;subb %d1,#0,%D0
   add %1,#%2,%I0\;addc %d1,#0,%D0"
 [(set_attr "cc" "math")]
)

(define_expand "addp24prog3"
  [(set (match_operand:P24PROG    0
             "pic30_mode2_operand"    "=r,r,r,r,r,<>,<>,<>,<>,<>,R,R,R,R,R")
        (plus:P24PROG
           (match_operand:P24PROG 1
             "pic30_register_operand" "%r,r,r,r,r,r,r,r,r,r,r,r,r,r,r")
           (match_operand:P24PROG 2
             "pic30_math_APSV_operand" " r,<>,R,N,P,r,<>,R,N,P,r,<>,R,N,P")))]
  ""
  "
{
  if (pic30_math_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_addp24prog3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_addp24prog3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "add2P24PSV3_lit"
  [(set (match_operand:P24PSV   0 "pic30_reg_or_R_operand" "=r,r,R,r,r,R,r")
        (plus:P24PSV
          (match_operand:P24PSV 1 "pic30_register_operand" "%0,r,r,0,r,r,0")
          (match_operand:P24PSV 2 "pic30_JN_operand"       " P,P,P,N,N,N,J")))]
  ""
  "*
{
   static char *patterns[] = {
      \"add %1,#%2,%0\;addc %d1,#0,%d0\",
      \"add %1,#%2,%0\;addc %d1,#0,%d0\",
      \"add %1,#%2,%I0\;addc %d1,#0,%D0\",
      \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
      \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
      \"sub %1,#%J2,%I0\;subb %d1,#0,%D0\",
      \"add #%2,%0\;addc #0,%d0\",
      \"sub #%J2,%0\;subb #0,%d0\",
      \"add %1,#%2,%0\;addc %d1,#0,%d0\",
      \"sub %1,#%2,%0\;subb %d1,#0,%d0\",
      0};

  if ((which_alternative == 0) && (INTVAL(operands[2]) > 15)) {
    if (REGNO(operands[0]) == REGNO(operands[1]))
      which_alternative = 6;
    else which_alternative = 8;
  } else if ((which_alternative == 1) && (INTVAL(operands[2]) < -15)) {
    if (REGNO(operands[0]) == REGNO(operands[1]))
      which_alternative = 7;
    else which_alternative = 8;
  }
  return patterns[which_alternative];

}"
  [(set_attr "cc" "math")]
)

(define_insn "addp24psv3_DATA_e"
  [(set (match_operand:P24PSV    0 
             "pic30_mode2_operand"   "=r,r,r,<>,<>,<>,R,R,R")
        (plus:P24PSV
           (match_operand:P24PSV 1 
             "pic30_register_operand" "r,r,r,r,r,r,r,r,r")
           (match_operand:P24PSV 2
             "pic30_mode2_operand"    "r,<>,R,r,<>,R,r,<>,R")))]
  ""
  "@
   add %1,%2,%0\;addc %d1,%d2,%d0
   add %1,%2,%0\;addc %d1,%2,%d0
   add %1,%I2,%0\;addc %d1,%D2,%d0
   add %1,%2,%0\;addc %d1,%d2,%0
   add %1,%2,%0\;addc %d1,%2,%0
   add %1,%I2,%0\;addc %d1,%D2,%0
   add %1,%2,%I0\;addc %d1,%d2,%D0
   add %1,%2,%I0\;addc %d1,%2,%D0
   add %1,%I2,%I0\;addc %d1,%D2,%D0"
 [(set_attr "cc" "math")]
)

(define_insn "addp24psv3_APSV_e"
  [(set (match_operand:P24PSV    0 
             "pic30_mode2_operand"   "=r,r,r,<>,<>,<>,R,R,R")
        (plus:P24PSV
           (match_operand:P24PSV 1 
             "pic30_register_operand" "r,r,r,r,r,r,r,r,r")
           (match_operand:P24PSV 2
             "pic30_mode2_APSV_operand"    "r,<>,R,r,<>,R,r,<>,R")))]
  ""
  "@
   add %1,%2,%0\;addc %d1,%d2,%d0
   add %1,%2,%0\;addc %d1,%2,%d0
   add %1,%I2,%0\;addc %d1,%D2,%d0
   add %1,%2,%0\;addc %d1,%d2,%0
   add %1,%2,%0\;addc %d1,%2,%0
   add %1,%I2,%0\;addc %d1,%D2,%0
   add %1,%2,%I0\;addc %d1,%d2,%D0
   add %1,%2,%I0\;addc %d1,%2,%D0
   add %1,%I2,%I0\;addc %d1,%D2,%D0"
 [(set_attr "cc" "math")]
)

(define_expand "addp24psv3"
  [(set (match_operand:P24PSV 0 "pic30_mode2_operand" "")
        (plus: P24PSV (match_operand:P24PSV 1 "pic30_register_operand" "")
                      (match_operand:P24PSV 2 "pic30_mode1i_operand" "")))]
  ""
  "
{  rtx (*gen)(rtx,rtx,rtx) = gen_addp24psv3_APSV_e;

   if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
     gen = gen_addp24psv3_DATA_e;
   if (GET_CODE(operands[2]) == CONST_INT) {
#if 0
     int val = INTVAL(operands[2]);
     rtx new_val = GEN_INT(val << 1);
#endif
     rtx new_val = operands[2];
     if (pic30_JN_operand(new_val, HImode)) {
       emit_insn(gen_add2P24PSV3_lit(operand0,operand1,operand2));  
       DONE;
     } else {
       /* load the immediate separately */
       rtx result = gen_reg_rtx(P24PSVmode);

       emit_insn(gen_movP24PSV_lit(result, new_val));
       emit_insn(gen(operand0, operand1, result));
       DONE;
     }
   }
   emit_insn(gen(operand0,operand1,operand2));
   DONE;
}")

;
; APSVrd
;
; target_flags & TARGET_TRACK_PSVPAG => we want to make sure PSVPAG is set right
;

(define_expand "P16APSVrd_qi"
  [(set (match_operand:QI 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:QI 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;
  rtx from = operands[1];
  rtx to = operands[0];

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  if (can_create_pseudo_p()) {
    if (GET_CODE(operands[1]) == MEM) {
      rtx inner = XEXP(operands[1],0);
      switch (GET_CODE(inner)) {
        case CONST:
        case LABEL_REF:
        case SYMBOL_REF:
          from = gen_reg_rtx(GET_MODE(inner));
          emit_move_insn(from, inner);
          from = gen_rtx_MEM(GET_MODE(operands[1]), from);
          break;
        default: break;
      }
    }
    if ((GET_CODE(operands[0]) == MEM) && 
        (!pic30_data_operand(operands[0],GET_MODE(operands[0])))) {
      rtx inner = XEXP(operands[0],0);
      switch (GET_CODE(inner)) {
        case CONST:
        case LABEL_REF:
        case SYMBOL_REF:
          to = gen_reg_rtx(GET_MODE(inner));
          emit_move_insn(to, inner);
          to = gen_rtx_MEM(GET_MODE(operands[0]), to);
          break;
        default: break;
      }
    }
  }
  emit_insn(
    gen_movqi_gen_APSV(to,from)
  );
  DONE;
}")

(define_expand "P16APSVrd_HI"
  [(set (match_operand:HI 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:HI 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movhi_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P16APSV"
  [(set (match_operand:P16APSV 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P16APSV 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movp16apsv_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P16PMP"
  [(set (match_operand:P16PMP 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P16PMP 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP16PMP_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")


(define_expand "P16APSVrd_P24PSV"
  [(set (match_operand:P24PSV 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P24PSV 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP24PSV_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P24PROG"
  [(set (match_operand:P24PROG 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P24PROG 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP24PROG_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_si"
  [(set (match_operand:SI 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:SI 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movsi_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P32EXT"
  [(set (match_operand:P32EXT 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P32EXT 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP32EXT_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P32EDS"
  [(set (match_operand:P32EDS 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P32EDS 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP32EDS_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_P32PEDS"
  [(set (match_operand:P32PEDS 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:P32PEDS 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movP32PEDS_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_di"
  [(set (match_operand:DI 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:DI 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movdi_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_sf"
  [(set (match_operand:SF 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:SF 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movsf_general_APSV(operands[0],operands[1])
  );
  DONE;
}")

(define_expand "P16APSVrd_df"
  [(set (match_operand:DF 0 "pic30_move_operand"
                "=r<>,R,r<>, R,   r<>,RS,r<>,RS, Q,r,U,U")
        (match_operand:DF 1 "pic30_move2_APSV_operand"
                 "r,  r,<>RS,<>RS,r,  r, R<>,R<>,r,Q,a,r"))
  ]
  ""
  "
{ rtx sfr;
  rtx psv_page;

  if (target_flags & TARGET_TRACK_PSVPAG) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(
      gen_save_const_psv(psv_page, sfr)           /* hopefully optimized away */
    );
    emit(
      gen_set_nvpsv(psv_page)                     /* hopefully optimized away */
    );
  }
  emit_insn(
    gen_movdf_gen_APSV(operands[0],operands[1])
  );
  DONE;
}")

;
;  These insns and splits define how to read via a P24PROG or P24PSV, decomposed
;    into smaller operations if the optimizer is turned on.

(define_expand "P24PROGrd_qi"
   [(set (match_operand:QI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:QI 
           (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0 = operand0;
   
   if (!pic30_mode2_operand(op0, QImode)) {
     op0 = gen_reg_rtx(QImode);
   }
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_qi(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movqi(operand0, op0)
     );
   }
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   DONE;
}")
 
(define_expand "P24PSVrd_qi"
   [(set (match_operand:QI 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:QI (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(QImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_qi(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movqi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_HI"
   [(set (match_operand:HI 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:HI (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(HImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_HI(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movhi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P16APSV"
   [(set (match_operand:P16APSV 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:P16APSV (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(GET_MODE(operand0));
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P16APSV(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_move_insn(operand0, op0);
   }
   DONE;
}")

(define_expand "P24PSVrd_HI"
   [(set (match_operand:HI 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:HI (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(HImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_HI(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movhi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P16APSV"
   [(set (match_operand:P16APSV 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:HI (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(GET_MODE(operand0));
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P16APSV(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_move_insn(operand0, op0);
   }
   DONE;
}")


(define_expand "P24PROGrd_P16PMP"
   [(set (match_operand:P16PMP 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:P16PMP 
           (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;

   op0 = gen_reg_rtx(P16PMPmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P16PMP(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp16pmp(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P16PMP"
   [(set (match_operand:P16PMP 0 "pic30_move_operand" "=r,R,rR,r,R")
         (mem:P16PMP
           (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,RQ,ST,ST")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;

   op0 = gen_reg_rtx(P16PMPmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P16PMP(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp16pmp(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_si"
   [(set (match_operand:SI 0 "pic30_move_operand" "=r,R,r,R")
         (mem:SI (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(SImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_si(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movsi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_si"
   [(set (match_operand:SI 0 "pic30_move_operand" "=r,R,r,R")
         (mem:SI (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(SImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_si(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movsi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P32EDS"
   [(set (match_operand:P32EDS 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32EDS (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(P32EDSmode);
   rtx op0;
  
   op0 = gen_reg_rtx(P32EDSmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P32EDS(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32eds(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P32PEDS"
   [(set (match_operand:P32PEDS 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32PEDS (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(GET_MODE(operand0));
   rtx op0;
  
   op0 = gen_reg_rtx(GET_MODE(operand0));
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P32PEDS(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32eds(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P32EXT"
   [(set (match_operand:P32EXT 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32EXT (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(P32EXTmode);
   rtx op0;
  
   op0 = gen_reg_rtx(P32EXTmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                          /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P32EXT(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32ext(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P32EDS"
   [(set (match_operand:P32EDS 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32EDS (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(P32EDSmode);
   rtx op0;
  
   op0 = gen_reg_rtx(P32EDSmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_P32EDS(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32eds(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P32PEDS"
   [(set (match_operand:P32PEDS 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32PEDS (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(GET_MODE(operands[0]));
   rtx op0;
  
   op0 = gen_reg_rtx(GET_MODE(operands[0]));
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_P32PEDS(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32eds(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P32EXT"
   [(set (match_operand:P32EXT 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P32EXT (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(P32EXTmode);
   rtx op0;
  
   op0 = gen_reg_rtx(P32EXTmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_P32EXT(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp32ext(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_sf"
   [(set (match_operand:SF 0 "pic30_move_operand" "=r,R,r,R")
         (mem:SF (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0 = operand0;
  
   op0 = gen_reg_rtx(SFmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_sf(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movsf(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_sf"
   [(set (match_operand:SF 0 "pic30_move_operand" "=r,R,r,R")
         (mem:SF (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(SFmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_sf(op0,
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movsf(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P24PROG"
   [(set (match_operand:P24PROG 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P24PROG (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(P24PROGmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P24PROG(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp24prog(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P24PROG"
   [(set (match_operand:P24PROG 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P24PROG (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(P24PROGmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_P24PROG(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp24prog(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_P24PSV"
   [(set (match_operand:P24PSV 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P24PSV (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(P24PSVmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_P24PSV(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp24psv(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_P24PSV"
   [(set (match_operand:P24PSV 0 "pic30_move_operand" "=r,R,r,R")
         (mem:P24PSV (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(P24PSVmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_P24PSV(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movp24psv(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_di"
   [(set (match_operand:DI 0 "pic30_move_operand" "=r,R,r,R")
         (mem:DI (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(DImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_di(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movdi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_di"
   [(set (match_operand:DI 0 "pic30_move_operand" "=r,R,r,R")
         (mem:DI (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
    (clobber (match_scratch:P24PSV 2 "=r,r,r,r"))
    (clobber (match_scratch:QI 3 "=r,r,r,r"))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(DImode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PSV_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_di(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movdi(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PROGrd_df"
   [(set (match_operand:DF 0 "pic30_move_operand" "=r,R,r,R")
         (mem:DF (match_operand:P24PROG 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PROGmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(DFmode);
   pic30_managed_psv = 1;
   emit_insn(
     gen_movP24PROG_gen(op2, operand1)                   /* copy pointer */
   );
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPROG(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PROGread_df(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movdf(operand0, op0)
     );
   }
   DONE;
}")

(define_expand "P24PSVrd_df"
   [(set (match_operand:DF 0 "pic30_move_operand" "=r,R,r,rR")
         (mem:DF (match_operand:P24PSV 1 "pic30_psv_operand" "r,r,R,R")))
   ]
   ""
   "
{  rtx op2 = gen_reg_rtx(P24PSVmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1_ = gen_reg_rtx(SImode);
   rtx op0;
  
   op0 = gen_reg_rtx(DFmode);
   pic30_managed_psv = 1;
   if (pic30_reg_or_R_operand(operand1, GET_MODE(operand1))) {
     emit_insn(
       gen_movP24PSV_gen(op2, operand1)                  /* copy pointer */
     );
   } else {
     emit_insn(
       gen_movP24PSV_address(op2, operand1)              /* create pointer */
     );
   }
   emit_insn(
     gen_copy_psv(op3)                                   /* preserve PSVPAG */
   );
   emit_insn(
     gen_unpack_MPSV(op1_,op2)                           /* unpack PSV ptr */
   );
   emit_insn(
     gen_set_psv(gen_rtx_SUBREG(HImode, op1_, 2))        /* set PSVPAG */
   );
   emit_insn(
     gen_P24PSVread_df(op0, 
                        gen_rtx_SUBREG(HImode, op1_, 0)) /* read value */
   );
   emit_insn(
     gen_set_psv(op3)                                    /* restore PSVPAG */
   );
   if (op0 != operand0) {
     emit_insn(
       gen_movdf(operand0, op0)
     );
   }
   DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;; PMP

(define_expand "P16PMPrd_qi"
   [(set (match_operand:QI 0 "pic30_move_operand"       "=rR")
         (mem:QI
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0])); 
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1qi(result)                           /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1qi(result)                           /* real read */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_qi"
   [(set (mem:QI (match_operand:P16PMP 0 "pic30_pmp_operand" "r"))
         (match_operand:QI 1 "pic30_move_operand"       "rR"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1qi(result)                           /* write */
   );
   DONE;
}")

(define_expand "P16PMPrd_HI"
   [(set (match_operand:HI 0 "pic30_move_operand"       "=rR")
         (mem:HI
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(result)                           /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(result)                          /* real read */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPrd_P16APSV"
   [(set (match_operand:P16APSV 0 "pic30_move_operand"       "=rR")
         (mem:P16APSV
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(result)                          /* real read */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")


(define_expand "P16PMPwt_HI"
   [(set (mem:HI (match_operand:P16PMP 0 "pic30_pmp_operand" "r"))
         (match_operand:HI 1 "pic30_move_operand"       "rR"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                              /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(result)                          /* write */
   );
   DONE;
}")

(define_expand "P16PMPwt_P16APSV"
   [(set (mem:P16APSV (match_operand:P16PMP 0 "pic30_pmp_operand" "r"))
         (match_operand:P16APSV 1 "pic30_move_operand"       "rR"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                              /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   DONE;
}")

(define_expand "P16PMPrd_P16PMP"
   [(set (match_operand:P16PMP 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P16PMP
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_P16PMP"
   [(set (mem:P16PMP 
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P16PMP 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   DONE;
}")


(define_expand "P16PMPrd_P24PSV"
   [(set (match_operand:P24PSV 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P24PSV
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_P24PSV"
   [(set (mem:P24PSV 
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P24PSV 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                   /* set auto inc  mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write        */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write        */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc  mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_P24PROG"
   [(set (match_operand:P24PROG 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P24PROG
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_P24PROG"
   [(set (mem:P24PROG
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P24PROG 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_si"
   [(set (match_operand:SI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:SI
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc  mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_si"
   [(set (mem:SI
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:SI 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc  mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_P32EDS"
   [(set (match_operand:P32EDS 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32EDS
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPrd_P32PEDS"
   [(set (match_operand:P32PEDS 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32PEDS
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPrd_P32EXT"
   [(set (match_operand:P32EXT 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32EXT
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_P32EDS"
   [(set (mem:P32EDS
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32EDS 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                   /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPwt_P32PEDS"
   [(set (mem:P32PEDS
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32PEDS 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                   /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPwt_P32EXT"
   [(set (mem:P32EXT
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32EXT 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                   /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_sf"
   [(set (match_operand:SF 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:SF
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_sf"
   [(set (mem:SF
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:SF 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_di"
   [(set (match_operand:DI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:DI
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,4)) /* real read, pt3 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,6)) /* real read, pt4 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")


(define_expand "P16PMPwt_di"
   [(set (mem:DI
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:DI 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,4)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,6)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))       /* clr auto increment mode */
   );
   DONE;
}")

(define_expand "P16PMPrd_df"
   [(set (match_operand:DF 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:DF
           (match_operand:P16PMP 1 "pic30_pmp_operand" "r,r,RQ,ST,ST")))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[0]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[1]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[1])
     );
   } else {
     emit_move_insn(addr,operands[1]);
   }

   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* dummy read */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* real read, pt1 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* real read, pt2 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,4)) /* real read, pt3 */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_get_PMDIN1hi(gen_rtx_SUBREG(HImode,result,6)) /* real read, pt4 */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   emit_move_insn(operands[0],result);
   DONE;
}")

(define_expand "P16PMPwt_df"
   [(set (mem:DF
           (match_operand:P16PMP 0 "pic30_pmp_operand" "r,r,RQ,ST,ST"))
         (match_operand:DF 1 "pic30_move_operand"       "=r,R,rR,r,R"))]
   ""
   "
{  rtx result = gen_reg_rtx(GET_MODE(operands[1]));
   rtx addr;

   addr = gen_reg_rtx(P16PMPmode);
   if (GET_CODE(operands[0]) == SYMBOL_REF) {
     emit_insn(
       gen_movP16PMP_address(addr,operands[0])
     );
   } else {
     emit_move_insn(addr,operands[0]);
   }

   emit_insn(
     gen_set_PMADDR(addr)                               /* set up address */
   );
   emit_insn(
     gen_bitset_PMMODE(GEN_INT(11))                    /* set auto inc mode */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_move_insn(result,operands[1]);
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,0)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,2)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,4)) /* write */
   );
   emit_insn(
     gen_while_PMMODE_busy()
   );
   emit_insn(
     gen_set_PMDIN1hi(gen_rtx_SUBREG(HImode,result,6)) /* write */
   );
   emit_insn(
     gen_bitclr_PMMODE(GEN_INT(11))                    /* clr auto inc mode */
   );
   DONE;
}")


;;;;;;;;;;;;;;;;;;;;;;; EXT

;;

(define_expand "P32EXTrd_qi"
   [(set (match_operand:QI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:QI
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_8)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_8)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1, 
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;
  
        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(1));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        warning(0,\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external8\");
        DONE;
      }
    }"
)

(define_expand "P32EXTwt_qi"
   [(set (mem:QI (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:QI 1 "pic30_move_operand"            "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_8)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_8)),0),
                          LCT_NORMAL, VOIDmode, 2, 
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
  
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(1));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        warning(0,\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external8\");
        DONE;
      }
    }"
)

(define_mode_iterator EXT16BITRD [HI P16APSV P16PMP])
(define_mode_iterator EXT16BITWT [HI P16APSV P16PMP])

(define_expand "P32EXTrd_<MODE>"
   [(set (match_operand:EXT16BITRD 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:EXT16BITRD
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_16)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_16)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1, 
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;
  
        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(2));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else if (pst_8) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_8)),0),
          simplify_gen_subreg(QImode, operands[0], GET_MODE(operands[0]), 0), 
          LCT_NORMAL, QImode, 1, input, P32EXTmode);
        emit_insn(
          gen_addp32ext3_imm(input, input, GEN_INT(1))
        );
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_8)),0),
          simplify_gen_subreg(QImode, operands[0], GET_MODE(operands[0]), 1), 
          LCT_NORMAL, QImode, 1, input, P32EXTmode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external16\");
      }
    }"
)

;(define_expand "P32EXTrd_P16APSV"
;   [(set (match_operand:P16APSV 0 "pic30_move_operand"       "=r,R,rR,r,R")
;         (mem:P16APSV
;           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
;   ""
;   "{
;      if (pic30_read_externals(pst_16)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;
;        emit_move_insn(input, operands[1]);
;        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_16)),0),
;                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1, 
;                          input, P32EXTmode);
;        DONE;
;      } else if (pic30_read_externals(pst_any)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
;                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
;        rtx param2,param3;
;  
;        emit_move_insn(input, operands[1]);
;        param3 = gen_reg_rtx(HImode);
;        param2 = gen_reg_rtx(Pmode);
;        emit_move_insn(param3, GEN_INT(2));
;        emit_move_insn(param2, XEXP(temp,0));
;        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
;                          LCT_NORMAL, GET_MODE(operands[0]), 3,
;                          input, P32EXTmode,
;                          param2, Pmode,
;                          param3, HImode);
;        emit_move_insn(operands[0], temp);
;        DONE;
;      } else {
;        error(\"Cannot access external memory space;\n\"
;              \"\tdeclare __read_external or __read_external16\");
;      }
;    }"
;)

(define_expand "P32EXTwt_<MODE>"
   [(set (mem:EXT16BITWT
           (match_operand:P32EXT 0 "pic30_ext_operand"          "r,r,RQ,ST,ST"))
         (match_operand:EXT16BITWT 1 "pic30_move_operand"       "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_16)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_16)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(2));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else if (pst_8) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(QImode);

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, 
          simplify_gen_subreg(QImode, operands[1], GET_MODE(operands[1]),0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_8)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, QImode);
        emit_insn(
          gen_addp32ext3_imm(input, input, GEN_INT(1))
        );
        emit_insn(
          gen_addqi3(input1, input1, GEN_INT(1))
        );
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_8)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, QImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external16\");
      }
    }"
)

;(define_expand "P32EXTwt_P16APSV"
;   [(set (mem:P16APSV (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
;         (match_operand:P16APSV 1 "pic30_move_operand"            "r,R,rR,r,R"))]
;   ""
;   "{
;      if (pic30_write_externals(pst_16)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));
;
;        emit_move_insn(input, operands[0]);
;        emit_move_insn(input1, operands[1]);
;        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_16)),0),
;                          LCT_NORMAL, VOIDmode, 2,
;                          input, P32EXTmode,
;                          input1, GET_MODE(operands[1]));
;        DONE;
;      } else if (pic30_write_externals(pst_any)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
;                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
;        rtx param2,param3;
; 
;        emit_move_insn(input, operands[0]);
;        emit_move_insn(temp, operands[1]);
;        param3 = gen_reg_rtx(HImode);
;        param2 = gen_reg_rtx(Pmode);
;        emit_move_insn(param3, GEN_INT(2));
;        emit_move_insn(param2, XEXP(temp,0));
;        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
;                          LCT_NORMAL, GET_MODE(operands[0]), 3,
;                          input, P32EXTmode,
;                          param2, Pmode,
;                          param3, HImode);
;        DONE;
;      } else {
;        error(\"Cannot access external memory space;\n\"
;              \"\tdeclare __write_external or __write_external16\");
;      }
;    }"
;)

;(define_expand "P32EXTrd_P16PMP"
;   [(set (match_operand:P16PMP 0 "pic30_move_operand"       "=r,R,rR,r,R")
;         (mem:P16PMP
;           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
;   ""
;   "{
;      if (pic30_read_externals(pst_16)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;
;        emit_move_insn(input, operands[1]);
;        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_16)),0),
;                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
;                          input, P32EXTmode);
;        DONE;
;      } else if (pic30_read_externals(pst_any)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
;                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
;        rtx param2,param3;
; 
;        emit_move_insn(input, operands[1]);
;        param3 = gen_reg_rtx(HImode);
;        param2 = gen_reg_rtx(Pmode);
;        emit_move_insn(param3, GEN_INT(2));
;        emit_move_insn(param2, XEXP(temp,0));
;        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
;                          LCT_NORMAL, GET_MODE(operands[0]), 3,
;                          input, P32EXTmode,
;                          param2, Pmode,
;                          param3, HImode);
;        emit_move_insn(operands[0], temp);
;        DONE;
;      } else {
;        error(\"Cannot access external memory space;\n\"
;              \"\tdeclare __read_external or __read_external16\");
;      }
;    }"
;
;)

;(define_expand "P32EXTwt_P16PMP"
;   [(set (mem:P16PMP 
;           (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
;         (match_operand:P16PMP 1 "pic30_move_operand"  "r,R,rR,r,R"))]
;   ""
;   "{
;      if (pic30_write_externals(pst_16)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));
;
;        emit_move_insn(input, operands[0]);
;        emit_move_insn(input1, operands[1]);
;        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_16)),0),
;                          LCT_NORMAL, VOIDmode, 2,
;                          input, P32EXTmode,
;                          input1, GET_MODE(operands[1]));
;        DONE;
;      } else if (pic30_write_externals(pst_any)) {
;        rtx input = gen_reg_rtx(P32EXTmode);
;        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
;                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
;        rtx param2,param3;
; 
;        emit_move_insn(input, operands[0]);
;        emit_move_insn(temp, operands[1]);
;        param3 = gen_reg_rtx(HImode);
;        param2 = gen_reg_rtx(Pmode);
;        emit_move_insn(param3, GEN_INT(2));
;        emit_move_insn(param2, XEXP(temp,0));
;        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
;                          LCT_NORMAL, GET_MODE(operands[0]), 3,
;                          input, P32EXTmode,
;                          param2, Pmode,
;                          param3, HImode);
;        DONE;
;      } else {
;        error(\"Cannot access external memory space;\n\"
;              \"\tdeclare __write_external or __write_external16\");
;      }
;    }"
;)

(define_expand "P32EXTrd_P24PSV"
   [(set (match_operand:P24PSV 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P24PSV
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTwt_P24PSV"
   [(set (mem:P24PSV 
           (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:P24PSV 1 "pic30_move_operand"  "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTrd_P24PROG"
   [(set (match_operand:P24PROG 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P24PROG
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTwt_P24PROG"
   [(set (mem:P24PROG 
           (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:P24PROG 1 "pic30_move_operand" "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTrd_si"
   [(set (match_operand:SI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:SI
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external32\");
      }
    }"
)

(define_expand "P32EXTwt_si"
   [(set (mem:SI (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:SI 1 "pic30_move_operand"       "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTrd_P32EDS"
   [(set (match_operand:P32EDS 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32EDS
           (match_operand:P32EDS 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EDSmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EDSmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EDSmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EDSmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external32\");
      }
    }"
)

(define_expand "P32EXTrd_P32PEDS"
   [(set (match_operand:P32PEDS 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32PEDS
           (match_operand:P32PEDS 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32PEDSmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EDSmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32PEDSmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32PEDSmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external32\");
      }
    }"
)

(define_expand "P32EXTrd_P32EXT"
   [(set (match_operand:P32EXT 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:P32EXT
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external32\");
      }
    }"
)

(define_expand "P32EXTwt_P32EDS"
   [(set (mem:P32EDS 
           (match_operand:P32EDS 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32EDS 1 "pic30_move_operand"  "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EDSmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EDSmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EDSmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EDSmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTwt_P32PEDS"
   [(set (mem:P32PEDS 
           (match_operand:P32PEDS 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32PEDS 1 "pic30_move_operand"  "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32PEDSmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32PEDSmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32PEDSmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32PEDSmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTwt_P32EXT"
   [(set (mem:P32EXT 
           (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:P32EXT 1 "pic30_move_operand"  "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTrd_sf"
   [(set (match_operand:SF 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:SF
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_32)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        (input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external32\");
      }
    }"
)

(define_expand "P32EXTwt_sf"
   [(set (mem:SF (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:SF 1 "pic30_move_operand"            "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_32)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_32)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external32\");
      }
    }"
)

(define_expand "P32EXTrd_di"
   [(set (match_operand:DI 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:DI
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_64)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_64)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(8));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external64\");
      }
    }"
)

(define_expand "P32EXTwt_di"
   [(set (mem:DI (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:DI 1 "pic30_move_operand"            "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_64)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_64)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(8));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external64\");
      }
    }"
)

(define_expand "P32EXTrd_df"
   [(set (match_operand:DF 0 "pic30_move_operand"       "=r,R,rR,r,R")
         (mem:DF
           (match_operand:P32EXT 1 "pic30_ext_operand" "r,r,RQ,ST,ST")))]
   ""
   "{
      if (pic30_read_externals(pst_64)) {
        rtx input = gen_reg_rtx(P32EXTmode);

        emit_move_insn(input, operands[1]);
        emit_library_call_value(XEXP(DECL_RTL(pic30_read_externals(pst_64)),0),
                          operands[0], LCT_NORMAL, GET_MODE(operands[0]), 1,
                          input, P32EXTmode);
        DONE;
      } else if (pic30_read_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[0]),
                                     GET_MODE_SIZE(GET_MODE(operands[0])), 0);
        rtx param2,param3;

        emit_move_insn(input, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(4));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_read_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        emit_move_insn(operands[0], temp);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __read_external or __read_external64\");
      }
    }"
)

(define_expand "P32EXTwt_df"
   [(set (mem:DF (match_operand:P32EXT 0 "pic30_ext_operand" "r,r,RQ,ST,ST"))
         (match_operand:DF 1 "pic30_move_operand"            "r,R,rR,r,R"))]
   ""
   "{
      if (pic30_write_externals(pst_64)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx input1 = gen_reg_rtx(GET_MODE(operands[1]));

        emit_move_insn(input, operands[0]);
        emit_move_insn(input1, operands[1]);
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_64)),0),
                          LCT_NORMAL, VOIDmode, 2,
                          input, P32EXTmode,
                          input1, GET_MODE(operands[1]));
        DONE;
      } else if (pic30_write_externals(pst_any)) {
        rtx input = gen_reg_rtx(P32EXTmode);
        rtx temp = assign_stack_temp(GET_MODE(operands[1]),
                                     GET_MODE_SIZE(GET_MODE(operands[1])), 0);
        rtx param2,param3;
 
        emit_move_insn(input, operands[0]);
        emit_move_insn(temp, operands[1]);
        param3 = gen_reg_rtx(HImode);
        param2 = gen_reg_rtx(Pmode);
        emit_move_insn(param3, GEN_INT(8));
        emit_move_insn(param2, XEXP(temp,0));
        emit_library_call(XEXP(DECL_RTL(pic30_write_externals(pst_any)),0),
                          LCT_NORMAL, GET_MODE(operands[0]), 3,
                          input, P32EXTmode,
                          param2, Pmode,
                          param3, HImode);
        DONE;
      } else {
        error(\"Cannot access external memory space;\n\"
              \"\tdeclare __write_external or __write_external64\");
      }
    }"
)

;;;;;;;;;;;;;;;;;;; DF

(define_expand "P32DFrd"
  [(set (match_operand 0 "pic30_register_operand"    "=a")
        (match_operand 1 "pic30_mem_df_operand" " RQST"))]
  ""
  { rtx op0 = operand0;
     rtx op1 = operand1;
     rtx rtx_len;
     emit_library_call_value(
       gen_rtx_SYMBOL_REF(HImode, \"|Nf|__P32DFrd\"),
       op0,
       LCT_NORMAL,
       GET_MODE(op0),
       2,               /* args */
       XEXP(operands[1], 0), P32DFmode,
       GEN_INT(GET_MODE_SIZE(GET_MODE(op0))), HImode
     );
     DONE;
  }
)

;;;;;;;;;;;;;;;;;;; EDS

(define_expand "P32EDSrd"
   [(set (match_operand 0 "pic30_reg_or_R_operand"       "=rR")
         (match_operand 1 "pic30_mem_eds_operand"    " RQST"))]
   ""
   "{
   rtx op2 = gen_reg_rtx(P32EDSmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op0 = operand0;
   rtx op1 = operand1;
   rtx page = gen_reg_rtx(HImode);
   rtx offset = gen_reg_rtx(HImode);
   rtx (*fn)(rtx, rtx);
   int indirect_allowed=1;
   int psv_set=0;
   int unpack=0;

   if (GET_MODE(op0) != GET_MODE(op1)) FAIL;
   switch (GET_MODE(op0)) {
     default:
                  fprintf(stderr, \"Unknown mode: %s\n\",
                          mode_name[GET_MODE(op0)]);
                  FAIL;
     case QImode: fn = gen_P32EDSread_qi;
                  break;
     case HImode: fn = gen_P32EDSread_HI;
                  break;
     case SImode: fn = gen_P32EDSread_si;
                  unpack=1;
                  break;
     case DImode: fn = gen_P32EDSread_di;
                  indirect_allowed=0;
                  unpack=1;
                  break;
     case SFmode: fn = gen_P32EDSread_sf;
                  unpack=1;
                  break;
     case DFmode: fn = gen_P32EDSread_df;
                  indirect_allowed=0;
                  unpack=1;
                  break;
     case P16APSVmode: fn = gen_P32EDSread_P16APSV;
                  break;
     case P24PSVmode: fn = gen_P32EDSread_P24PSV;
                  unpack=1;
                  break;
     case P24PROGmode: fn = gen_P32EDSread_P24PROG;
                  unpack=1;
                  break;
     case P16PMPmode: fn = gen_P32EDSread_P16PMP;
                  unpack=1;
                  break;
     case P32EXTmode: fn = gen_P32EDSread_P32EXT;
                  unpack=1;
                  break;
     case P32EDSmode: fn = gen_P32EDSread_P32EDS;
                  unpack=1;
                  break;
     case P32PEDSmode: fn = gen_P32EDSread_P32PEDS;
                  unpack=1;
                  break;
   }
   if ((indirect_allowed == 0) && (pic30_R_operand(op0, GET_MODE(op0)))) {
     op0 = gen_reg_rtx(GET_MODE(op0));
   } else if (!pic30_reg_or_R_operand(op0, GET_MODE(op0))) {
     op0 = gen_reg_rtx(GET_MODE(op0));
   }
   pic30_managed_psv = 1;
   if (!TARGET_EDS) {
     emit_insn(
       gen_copy_psv(op3)                                /* preserve PSVPAG */
     );
   }
   if (pic30_R_constraint(op1) || pic30_Q_constraint(op1) ||
       pic30_S_constraint_ecore(op1,1)) {
     if (GET_CODE(XEXP(op1,0)) == PLUS) {
       expand_binop(P32EDSmode, add_optab, XEXP(XEXP(op1,0),0),
                    XEXP(XEXP(op1,0),1), op2, 1, OPTAB_DIRECT);
     } else {
       emit_insn(
         gen_movP32EDS_gen(op2, XEXP(op1,0)) /* copy pointer */
       );
     }
   } else {
     if (pic30_ecore_target()) {
       psv_set = 1;
       /* create pointer */
       if (pic30_symbolic_address_operand(XEXP(op1,0),VOIDmode)) {
         char *t = pic30_section_base(XEXP(op1,0),0,0);
         
         emit_insn(
           gen_movpag(XEXP(op1,0), GEN_INT(0))
         );
         emit_insn(
           gen_movEDS_address_offset(offset, GEN_INT((HOST_WIDE_INT)t))   
         );
       } else {
         rtx reg = force_reg(P32EDSmode, XEXP(op1,0));

         emit_insn(
           gen_movpag(simplify_gen_subreg(HImode, reg, GET_MODE(reg), 2), GEN_INT(0))
         );
         emit_move_insn(offset,
                        simplify_gen_subreg(HImode, reg, GET_MODE(reg), 0));
       }
     } else {
       emit_insn(
         gen_movP32EDS_address(op2, XEXP(op1,0))        /* create pointer */
       );
     }
   }
   if (unpack) {
     rtx op2a = gen_reg_rtx(P32EDSmode);
     emit_insn(
       gen_unpack_EDS(op2a,op2)
     );
     op2 = op2a;
   }
   if (!psv_set) {
     emit_move_insn(page, gen_rtx_SUBREG(HImode,op2,2));
     emit_insn(
       gen_set_psv(page)                                  /* set PSVPAG */
     );
     emit_move_insn(offset, gen_rtx_SUBREG(HImode,op2,0));
   }
   emit_insn(
     fn(op0, gen_rtx_MEM(GET_MODE(op1),offset))         /* read value */
   );
   if (op0 != operand0) {
     emit_move_insn(operand0, op0);
   }
   if (!TARGET_EDS) {
     emit_insn(
       gen_set_vpsv(op3)                                /* restore PSVPAG */
     );
   }
   DONE;
}")

(define_expand "P32PEDSrd"
   [(set (match_operand 0 "pic30_move_operand"       "=rR")
         (match_operand 1 "pic30_mem_peds_operand"   " RQST"))]
   ""
   "{
   rtx op2 = gen_reg_rtx(P32PEDSmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op0 = operand0;
   rtx op1 = operand1;
   rtx page = gen_reg_rtx(HImode);
   rtx offset = gen_reg_rtx(HImode);
   rtx (*fn)(rtx, rtx);
   int psv_set=0;

   if (GET_MODE(op0) != GET_MODE(op1)) FAIL;
   switch (GET_MODE(op0)) {
     default:
                  fprintf(stderr, \"Unknown mode: %s\n\",
                          mode_name[GET_MODE(op0)]);
                  FAIL;
     case QImode: fn = gen_P32PEDSread_qi;
                  break;
     case HImode: fn = gen_P32PEDSread_HI;
                  break;
     case SImode: fn = gen_P32PEDSread_si;
                  break;
     case DImode: fn = gen_P32PEDSread_di;
                  break;
     case SFmode: fn = gen_P32PEDSread_sf;
                  break;
     case DFmode: fn = gen_P32PEDSread_df;
                  break;
     case P16APSVmode: fn = gen_P32PEDSread_P16APSV;
                  break;
     case P24PSVmode: fn = gen_P32PEDSread_P24PSV;
                  break;
     case P24PROGmode: fn = gen_P32PEDSread_P24PROG;
                  break;
     case P16PMPmode: fn = gen_P32PEDSread_P16PMP;
                  break;
     case P32EXTmode: fn = gen_P32PEDSread_P32EXT;
                  break;
     case P32EDSmode: fn = gen_P32PEDSread_P32EDS;
                  break;
     case P32PEDSmode: fn = gen_P32PEDSread_P32PEDS;
                  break;
   }
   if (!pic30_mode2_operand(op0, GET_MODE(op0))) {
     op0 = gen_reg_rtx(GET_MODE(op0));
   }
   pic30_managed_psv = 1;
   if (!TARGET_EDS) {
     emit_insn(
       gen_copy_psv(op3)                                /* preserve PSVPAG */
     );
   }
   if (pic30_R_constraint(op1)) {
     emit_insn(
       gen_movP32PEDS_gen(op2, XEXP(op1,0))              /* copy pointer */
     );
   } else if (pic30_Q_constraint(op1) || pic30_S_constraint(op1)) {
     rtx inner = XEXP(operands[1],0);

     emit_insn(
       gen_addp32peds3(op2, XEXP(inner,0), XEXP(inner,1))
     );
   } else {
     if (pic30_ecore_target()) {
       char *t = pic30_section_base(XEXP(op1,0),0,0);
       psv_set=1;
       emit_insn(
         gen_movEDS_address_offset(offset, GEN_INT((HOST_WIDE_INT)t))   /* create pointer */
       );
       emit_insn(
         gen_movpag(XEXP(op1,0), GEN_INT(0))
       );
     } else {
       emit_insn(
         gen_movP32PEDS_address(op2, XEXP(op1,0))        /* create pointer */
       );
     }
   }
   if (!psv_set) {
     emit_move_insn(page, gen_rtx_SUBREG(HImode,op2,2));
     emit_move_insn(offset, gen_rtx_SUBREG(HImode,op2,0));
     emit_insn(
       gen_set_psv(page)                                  /* set PSVPAG */
     );
   }
   emit_insn(
     fn(op0, gen_rtx_MEM(GET_MODE(op1),offset))         /* read value */
   );
   if (op0 != operand0) {
     emit_move_insn(operand0, op0);
   }
   if (!TARGET_EDS) {
     emit_insn(
       gen_set_psv(op3)                                 /* restore PSVPAG */
     );
   }
   DONE;
}")

(define_expand "P32EDSwt"
   [(set (match_operand 0 "pic30_mem_eds_operand"   "=RQ,ST")
         (match_operand 1 "pic30_move_operand"       " rR,rR"))]
   ""
   "{
   rtx op2 = gen_reg_rtx(P32EDSmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1 = operand1;
   rtx op0 = operand0;
   rtx page = gen_reg_rtx(HImode);
   rtx offset = gen_reg_rtx(HImode);
   rtx (*fn)(rtx, rtx);
   int eds_target = pic30_eds_target();
   int unpack=0;

   if ((GET_CODE(XEXP(op0,0)) == POST_INC) &&
       (REGNO(XEXP(XEXP(op0,0),0)) == SP_REGNO)) {
     /* this is a stack write that can only be generated with -munified
        (which changes the Pmode to P32EDSmode) - change it back to a normal
        write */
     switch (GET_MODE(op0)) {
       default:
                    fprintf(stderr, \"Unknown mode: %s\n\",
                            mode_name[GET_MODE(op0)]);
                    FAIL;
       case QImode: fn = gen_movqi;
                    break;
       case HImode: fn = gen_movhi;
                    break;
       case SImode: fn = gen_movsi;
                    break;
       case DImode: fn = gen_movdi;
                    break;
       case SFmode: fn = gen_movsf;
                    break;
       case DFmode: fn = gen_movdf;
                    break;
       case P16APSVmode: fn = gen_movp16apsv;
                    break;
       case P24PSVmode: fn = gen_movp24psv;
                    break;
       case P24PROGmode: fn = gen_movp24prog;
                    break;
       case P16PMPmode: fn = gen_movp16pmp;
                    break;
       case P32EXTmode: fn = gen_movp32ext;
                    break;
       case P32EDSmode: fn = gen_movp32eds;
                    break;
       case P32PEDSmode: fn = gen_movp32peds;
                    break;
     }

     if (fn(gen_rtx_MEM(GET_MODE(op0),
                        gen_rtx_POST_INC(HImode,gen_rtx_REG(HImode, SP_REGNO))),
            op1))
       DONE;
     FAIL;
   }
   if (GET_MODE(op0) != GET_MODE(op1)) FAIL;
   switch (GET_MODE(op0)) {
     default:
                  fprintf(stderr, \"Unknown mode: %s\n\",
                          mode_name[GET_MODE(op0)]);
                  FAIL;
     case QImode: fn = gen_P32EDSwrite_qi;
                  break;
     case HImode: fn = gen_P32EDSwrite_HI;
                  break;
     case SImode: fn = gen_P32EDSwrite_si;
                  unpack=1;
                  break;
     case DImode: fn = gen_P32EDSwrite_di;
                  unpack=1;
                  break;
     case SFmode: fn = gen_P32EDSwrite_sf;
                  unpack=1;
                  break;
     case DFmode: fn = gen_P32EDSwrite_df;
                  unpack=1;
                  break;
     case P16APSVmode: fn = gen_P32EDSwrite_P16APSV;
                  unpack=1;
                  break;
     case P24PSVmode: fn = gen_P32EDSwrite_P24PSV;
                  unpack=1;
                  break;
     case P24PROGmode: fn = gen_P32EDSwrite_P24PROG;
                  unpack=1;
                  break;
     case P16PMPmode: fn = gen_P32EDSwrite_P16PMP;
                  break;
     case P32EXTmode: fn = gen_P32EDSwrite_P32EXT;
                  unpack=1;
                  break;
     case P32EDSmode: fn = gen_P32EDSwrite_P32EDS;
                  unpack=1;
                  break;
     case P32PEDSmode: fn = gen_P32EDSwrite_P32PEDS;
                  unpack=1;
                  break;
   }
   if (!pic30_mode2_operand(op1, GET_MODE(op1))) {
     op1 = gen_reg_rtx(GET_MODE(op1));
     emit_move_insn(op1, operand1);
   }
   if (eds_target) {
     pic30_managed_psv = 1;
   }
   if (pic30_R_constraint(op0) || pic30_Q_constraint(op0) ||
       pic30_S_constraint_ecore(op0,1)) {
     if (GET_CODE(XEXP(op0,0)) == PLUS) {
       expand_binop(P32EDSmode, add_optab, XEXP(XEXP(op0,0),0),
                    XEXP(XEXP(op0,0),1), op2, 1, OPTAB_DIRECT);
     } else {
       emit_insn(
         gen_movP32EDS_gen(op2, XEXP(op0,0)) /* copy pointer */
       );
     }
   } else {
     emit_insn(
       gen_movP32EDS_address(op2, XEXP(op0,0))          /* create pointer */
     );
   }
   if (unpack) {
     rtx op2a = gen_reg_rtx(P32EDSmode);
     emit_insn(
       gen_unpack_EDS(op2a,op2)
     );
     op2 = op2a;
   }
   if (eds_target) {
     if (!TARGET_EDS) {
       emit_insn(
         gen_copy_dsw(op3)                              /* preserve PSVPAG */
       );
     }
     emit_move_insn(page, gen_rtx_SUBREG(HImode,op2,2));
   }
   emit_move_insn(offset, gen_rtx_SUBREG(HImode,op2,0));
   if (eds_target) {
     emit_insn(
       gen_set_dsw(page)                                /* set PSVPAG */
     );
   }
   emit_insn(
     fn(gen_rtx_MEM(GET_MODE(op0),offset), op1)         /* write value */
   );
   if (eds_target) {
     if (!TARGET_EDS) {
       emit_insn(
         gen_set_dsw(op3)                               /* restore PSVPAG */
       );
     }
   }
   DONE;
}")

(define_expand "P32PEDSwt"
   [(set (match_operand 0 "pic30_mem_peds_operand"   "=RQ,ST")
         (match_operand 1 "pic30_move_operand"       " rR,rR"))]
   ""
   "{  
   rtx op2 = gen_reg_rtx(P32PEDSmode);
   rtx op3 = gen_reg_rtx(HImode);
   rtx op1 = operand1;
   rtx op0 = operand0;
   rtx page = gen_reg_rtx(HImode);
   rtx offset = gen_reg_rtx(HImode);
   rtx (*fn)(rtx, rtx);
   int eds_target = pic30_eds_target();

   if (GET_MODE(op0) != GET_MODE(op1)) FAIL;
   switch (GET_MODE(op0)) {
     default:
                  fprintf(stderr, \"Unknown mode: %s\n\", 
                          mode_name[GET_MODE(op0)]);
                  FAIL;
     case QImode: fn = gen_P32PEDSwrite_qi;
                  break;
     case HImode: fn = gen_P32PEDSwrite_HI;
                  break;
     case SImode: fn = gen_P32PEDSwrite_si;
                  break;
     case DImode: fn = gen_P32PEDSwrite_di;
                  break;
     case SFmode: fn = gen_P32PEDSwrite_sf;
                  break;
     case DFmode: fn = gen_P32PEDSwrite_df;
                  break;
     case P16APSVmode: fn = gen_P32PEDSwrite_P16APSV;
                  break;
     case P24PSVmode: fn = gen_P32PEDSwrite_P24PSV;
                  break;
     case P24PROGmode: fn = gen_P32PEDSwrite_P24PROG;
                  break;
     case P16PMPmode: fn = gen_P32PEDSwrite_P16PMP;
                  break;
     case P32EXTmode: fn = gen_P32PEDSwrite_P32EXT;
                  break;
     case P32EDSmode: fn = gen_P32PEDSwrite_P32EDS;
                  break;
     case P32PEDSmode: fn = gen_P32PEDSwrite_P32PEDS;
                  break;
   }
   if (!pic30_mode2_operand(op1, GET_MODE(op1))) {
     op1 = gen_reg_rtx(GET_MODE(op1));
     emit_move_insn(op1, operand1);
   }
   if (eds_target) {
     pic30_managed_psv = 1;
   }
   if (pic30_R_constraint(op0)) {
     emit_insn(
       gen_movP32PEDS_gen(op2, XEXP(op0,0))              /* copy pointer */
     );
   } else if (pic30_Q_constraint(op0) || pic30_S_constraint(op0)) {
     rtx inner = XEXP(operands[0],0);

     emit_insn(
       gen_addp32peds3(op2, XEXP(inner,0), XEXP(inner,1))

     );
   } else {
     emit_insn(
       gen_movP32PEDS_address(op2, XEXP(op0,0))         /* create pointer */
     );
   }
   if (eds_target) {
     if (!TARGET_EDS) {
       emit_insn(
         gen_copy_dsw(op3)                              /* preserve PSVPAG */
       );
     }
     emit_move_insn(page, gen_rtx_SUBREG(HImode,op2,2));
   }
   emit_move_insn(offset, gen_rtx_SUBREG(HImode,op2,0));
   if (eds_target) {
     emit_insn(
       gen_set_dsw(page)                                /* set PSVPAG */
     );
   }
   emit_insn(
     fn(gen_rtx_MEM(GET_MODE(op0),offset), op1)         /* write value */
   );
   if (eds_target) {
     if (!TARGET_EDS) {
       emit_insn(
         gen_set_dsw(op3)                               /* restore PSVPAG */
       );
     }
   }
   DONE;
}")

(define_insn "movhi_P24PROGaddress_low"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (subreg:HI
          (match_operand:P24PROG 1 "pic30_symbolic_address_operand" "g") 0))]
  ""
  "mov #tbloffset(%1),%0"
 [
  (set_attr "cc" "change0")
  (set_attr "type" "def")
 ]
)

(define_insn "movP16PMP_lit"
  [(set (match_operand:P16PMP 0 "pic30_register_operand"          "=r")
        (match_operand:P16PMP 1 "immediate_operand"               "i"))]
  ""
  "mov #%1,%0"
  [(set_attr "type" "def")]
)

(define_insn "movP32DF_lit"
  [(set (match_operand:P32DF 0 "pic30_register_operand"           "=r")
        (match_operand:P32DF 1 "immediate_operand"                "i"))]
  ""
  "*
{ static char buffer[80];
  long i = INTVAL(operands[1]);

  if (pic30_symbolic_address_operand(operands[1],P32DFmode)) {
    sprintf(buffer,\"mov #packed_lo(%%1),%%0\;mov #packed_hi(%%1),%%d0\");
  } else {
    sprintf(buffer,\"mov #%d,%%0\;mov #%d,%%d0\",(i & 0xFFFF),
                   (i & 0xFFFF0000) >> 15);
  }
  return buffer;
}"
  [(set_attr "type" "def")]
)


(define_insn "movP32EDS_lit"
  [(set (match_operand:P32EDS 0 "pic30_register_operand"          "=r")
        (match_operand:P32EDS 1 "immediate_operand"               "i"))]
  ""
  "*
{ static char buffer[80];
  long i = INTVAL(operands[1]);

  if (pic30_symbolic_address_operand(operands[1],P32EDSmode)) {
    sprintf(buffer,\"mov #edsoffset(%%1),%%0\;mov #edspage(%%1),%%d0\");
  } else {
    sprintf(buffer,\"mov #%d,%%0\;mov #%d,%%d0\",(i & 0x7FFF),
                   (i & 0xFF8000) >> 15);
  }
  return buffer;
}
"
  [(set_attr "type" "def")]
)

(define_insn "movP32PEDS_lit"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand"          "=r")
        (match_operand:P32PEDS 1 "immediate_operand"               "i"))]
  ""
  "*
{ static char buffer[80];
  long i = INTVAL(operands[1]);

  if (pic30_symbolic_address_operand(operands[1],P32PEDSmode)) {
    sprintf(buffer,\"mov #edsoffset(%%1),%%0\;mov #edspage(%%1),%%d0\");
  } else {
    sprintf(buffer,\"mov #%d,%%0\;mov #%d,%%d0\",(i & 0x7FFF),
                   (i & 0xFF8000) >> 15);
  }
  return buffer;
}
"
  [(set_attr "type" "def")]
)

(define_insn "movP24PSV_lit"
  [(set (match_operand:P24PSV 0 "pic30_register_operand"          "=r")
        (match_operand:P24PSV 1 "immediate_operand"               "i"))]
  ""
  "* 
{ static char buffer[80];
  long i = INTVAL(operands[1]);

  if (pic30_symbolic_address_operand(operands[1],P24PSVmode)) {
    sprintf(buffer,\"mov #tbloffset(%%1),%%0\;mov #tblpage(%%1),%%d0\");
  } else {
    sprintf(buffer,\"mov #%d,%%0\;mov #%d,%%d0\",(i & 0xFFFF), 
                   (i & 0xFF0000) >> 16);
  }
  return buffer;
}
"
  [(set_attr "type" "def")]
)

(define_insn "movP24PROG_lit"
  [(set (match_operand:P24PROG 0 "pic30_register_operand"          "=r")
        (match_operand:P24PROG 1 "immediate_operand"               "i"))]
  ""
  "*
{ static char buffer[80];
  long i = INTVAL(operands[1]);

  if (pic30_symbolic_address_operand(operands[1],P24PROGmode)) {
    sprintf(buffer,\"mov #tbloffset(%%1),%%0\;mov #tblpage(%%1),%%d0\");
  } else {
    sprintf(buffer,\"mov #%d,%%0\;mov #%d,%%d0\",(i & 0xFFFF),
                   (i & 0xFF0000) >> 16);
  }
  return buffer;
}
"
  [(set_attr "type" "def")]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 32-bit integer moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Invalid move

(define_insn "*movsi_invalid_1"
  [(set (match_operand:SI 0 "pic30_register_operand"  "=r")
        (match_operand:SI 1 "pic30_code_operand" "g"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}

")
(define_insn "*movsi_invalid_2"
  [(set (match_operand:SI 0 "pic30_code_operand" "=g")
        (match_operand:SI 1 "pic30_register_operand"    "r"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")

(define_insn "movsi_const0"
  [(set (match_operand:SI 0 "pic30_mode2res_operand" "=r,R,<,>")
	(const_int 0))]
  ""
  "@
   mul.uu %0,#0,%0
   clr %0\;mov %I0,%D0
   clr %0\;clr %0
   clr %0\;clr %0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,use,defuse,defuse")
  ]
)

(define_insn "*movsi_const1"
  [(set (match_operand:SI 0 "pic30_mode2res_operand" "=r,R,<,>")
	(const_int -1))]
  ""
  "@
   setm %0\;setm %d0
   setm %I0\;setm %D0
   setm %0\;setm %0
   setm %0\;setm %0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,use,defuse,defuse")
  ]
)

(define_insn "*movsi_const0sfr"
  [(set (match_operand:SI 0 "pic30_near_operand" "=U")
	(const_int 0))]
  ""
  "clr %0\;clr %0+2"
  [(set_attr "cc" "unchanged")])

(define_insn "*movsi_const1sfr"
  [(set (match_operand:SI 0 "pic30_near_operand" "=U")
	(const_int -1))]
  ""
  "setm %0\;setm %0+2"
  [(set_attr "cc" "unchanged")])

(define_insn "movsi_address"
  [(set (match_operand:SI 0 "pic30_register_operand"              "=r")
        (match_operand:SI 1 "pic30_symbolic_address_operand" "g"))]
  ""
  "mov #%z1,%0\;mov #%y1,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "movsi_rimm"
  [(set (match_operand:SI 0 "pic30_register_operand" "=r,r")
        (match_operand:SI 1 "immediate_operand" "O,i"))]
  ""
  "@
   mul.uu %0,#0,%0
   mov #%z1,%0\;mov #%y1,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "movsi_gen"
  [(set (match_operand:SI 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (match_operand:SI 1 "pic30_move_operand"  "r,R,>,Q,r,r,>,r,<,r,T,r"))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movsi_gen_APSV"
  [(set (match_operand:SI 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:SI [
          (match_operand:SI 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movP24PSV_gen_APSV"
  [(set (match_operand:P24PSV 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:P24PSV [
          (match_operand:P24PSV 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movP24PROG_gen_APSV"
  [(set (match_operand:P24PROG 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:P24PROG [
          (match_operand:P24PROG 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movP32EXT_gen_APSV"
  [(set (match_operand:P32EXT 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:P32EXT [
          (match_operand:P32EXT 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movP32EDS_gen_APSV"
  [(set (match_operand:P32EDS 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:P32EDS [
          (match_operand:P32EDS 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movP32PEDS_gen_APSV"
  [(set (match_operand:P32PEDS 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (unspec:P32PEDS [
          (match_operand:P32PEDS 1 "pic30_move_APSV_operand"  
            "r,R,>,Q,r,r,>,r,<,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "movp32ext_rimm"
  [(set (match_operand:P32EXT 0 "pic30_register_operand" "=r,r")
        (match_operand:P32EXT 1 "immediate_operand" "O,i"))]
  ""
  "@
   mul.uu %0,#0,%0
   mov #%z1,%0\;mov #%y1,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "movP32EXT_gen"
  [(set (match_operand:P32EXT 0 "pic30_move_operand" "=r,r,r,r,R,>,>,Q,r,<,r,T")
        (match_operand:P32EXT 1 "pic30_move_operand"  "r,R,>,Q,r,r,>,r,<,r,T,r"))]
  ""
  "*
{
        int idDst, idSrc, pre;

	switch (which_alternative)
	{
	case 0: /* r = r */
		return \"mov.d %1,%0\";
	case 1: /* r = R */
		return \"mov.d %1,%0\";
	case 2: /* r = > */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre increment */
			return \"add %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post increment */
			return \"mov.d %s1,%0\;add %r1,#4,%r1\";
	case 3: /* r = Q */
		idDst = REGNO(operands[0]);
		idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
		if (idDst == idSrc)
		{
			return \"mov %Q1,%d0\;mov %1,%0\";
		} else {
			return \"mov %1,%0\;mov %Q1,%d0\";
		}
	case 4: /* R = r */
		return \"mov.d %1,%0\";
	case 5: /* > = r */
		return \"mov.d %1,%0\";
	case 6: /* > = > */
		return \"mov %1,%0\;mov %1,%0\";
	case 7: /* Q = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	case 8: /* r = < */
		if ((pre = pic30_pp_modify_valid(operands[1])) == 0) 
			return \"mov.d %1,%0\";
 		else if (pre == -1) /* pre decrement */
			return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
		else if (pre == 1)  /* post decrement */
			return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
	case 9: /* < = r */
		return \"mov.d %1,%0\";
	case 10: /* r = T */
		return \"mov %1,%0\;mov %Q1,%d0\";
	case 11: /* T = r */
		return \"mov %1,%0\;mov %d1,%Q0\";
	default:
		return \";\";
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,def,etc,def,defuse,etc,defuse,def,def,etc")
  ]
)

(define_insn "*movsi_constnsfr"
  [(set (match_operand:SI 0 "pic30_near_operand" "=U")
        (match_operand:SI 1 "immediate_operand"   "i"))
	(clobber (match_scratch:HI 2             "=r"))]
  ""
  "*
{
	int imm = INTVAL(operands[1]);
	unsigned short msw, lsw;
	msw = (imm >> 16) & 0xffff;
	lsw = (imm) & 0xffff;
	if (msw == 0)
	{
		if (lsw == 0xffff)
		{
			return(\"setm %0\;clr %0+2\");
		}
		else
		{
			return(\"mov #%1,%2\;mov %2,%0\;clr %0+2\");
		}
	}
	else if (lsw == 0)
	{
		if (msw == 0xffff)
		{
			return(\"setm %0+2\;clr %0\");
		}
		else
		{
			return(\"mov #%y1,%2\;mov %2,%0+2\;clr %0\");
		}
	}
	else if (lsw == msw)
	{
		return(\"mov #%z1,%2\;mov %2,%0\;mov %2,%0+2\");
	}
	else
	{
		return(\"mov #%z1,%2\;mov %2,%0\;mov #%y1,%2\;mov %2,%0+2\");
	}

}"
  [(set_attr "cc" "clobber")])

(define_expand "movsi"
  [(set (match_operand:SI 0 "pic30_general_operand" "")
        (match_operand:SI 1 "pic30_general_operand" ""))]
  ""
  "
{
        if (pic30_emit_move_sequence(operands, SImode)) DONE;
}")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 64-bit integer moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Invalid move

(define_insn "*movdi_invalid_1"
  [(set (match_operand:DI 0 "pic30_register_operand"  "=r")
        (match_operand:DI 1 "pic30_code_operand" "g"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}

")
(define_insn "*movdi_invalid_2"
  [(set (match_operand:DI 0 "pic30_code_operand" "=g")
        (match_operand:DI 1 "pic30_register_operand"    "r"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")


(define_insn "*movdi_rimm"
  [(set (match_operand:DI 0 "pic30_register_operand" "=r,r")
        (match_operand:DI 1 "immediate_operand" "O,i"))]
  ""
  "*
{
	REAL_VALUE_TYPE r;
	long l[4] = { 0 };

	switch (which_alternative)
	{
        case 0:
          return(\"mul.uu %0,#0,%0\;\"
                 \"mul.uu %t0,#0,%t0\");
        default:
          if (GET_CODE(operands[1]) == CONST_DOUBLE)
          {
            REAL_VALUE_FROM_CONST_DOUBLE(r, operands[1]);
            switch (GET_MODE(operands[1]))
            {
              case VOIDmode:
                /*
                ** Integer
                */
                if (sizeof(HOST_WIDE_INT) == 4) {
                  l[0] = CONST_DOUBLE_LOW(operands[1]);
                  l[1] = CONST_DOUBLE_HIGH(operands[1]);
                } else if (sizeof(HOST_WIDE_INT) == 8) {
                  l[0] = CONST_DOUBLE_LOW(operands[1]) & 0xFFFFFFFF;
                  l[1] = CONST_DOUBLE_LOW(operands[1]) >> 32;
                } else {
                  gcc_unreachable();
                }
 
                break;
              default:
                REAL_VALUE_TO_TARGET_LONG_DOUBLE(r, l);
                break;
            }
          }
          else
          {
            if (sizeof(HOST_WIDE_INT) == 4) {
              l[0] = INTVAL(operands[1]);
              l[1] = l[0] < 0 ? -1 : 0;
            } else if (sizeof(HOST_WIDE_INT) == 8) {
              l[0] = CONST_DOUBLE_LOW(operands[1]) & 0xFFFFFFFF;
              l[1] = CONST_DOUBLE_LOW(operands[1]) >> 32;
            } else {
              gcc_unreachable();
            }
          }
          if (l[0] == 0)
          {
            return(\"mul.uu %0,#0,%0\;\"
                   \"mov #%x1,%t0\;\"
                   \"mov #%w1,%q0\");
          }
          else if (l[1] == 0)
          {
            return(\"mov #%z1,%0\;\"
                   \"mov #%y1,%d0\;\"
                   \"mul.uu %t0,#0,%t0\");
          }
          else if (l[0] == l[1])
          {
            return(\"mov #%z1,%0\;\"
                   \"mov #%y1,%d0\;\"
                   \"mov.d %0,%t0\");
          }
          return(\"mov #%z1,%0\;\"
                 \"mov #%y1,%d0\;\"
                 \"mov #%x1,%t0\;\"
                 \"mov #%w1,%q0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "*movdi_gen"
  [(set (match_operand:DI 0 "pic30_move_operand"
					"=r,r,r,r,R,>,>,Q,r,<,T,r")
        (match_operand:DI 1 "pic30_move_operand" 
					 "r,R,>,Q,r,r,>,r,<,r,r,T"))]
  ""
  "*
{
  int idSrc, idDst;
  char temp[64];
  char save[64];
  static char szInsn[64];

  szInsn[0] = 0;
  temp[0] = 0;
  save[0] = 0;

  switch (which_alternative) {
    case 0: /* r = r */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(operands[1]);
      if (idDst <= idSrc) {
        return \"mov.d %1,%0\;mov.d %t1,%t0\";
      } else {
        return \"mov.d %t1,%t0\;mov.d %1,%0\";
      }
    case 1: /* r = R */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(operands[1],0));
      if (pic30_pp_modify_valid(0) == 0) {
        if ((idDst > idSrc) || ((idDst+4) <= idSrc)) {
          /*
           ** source & dest don't overlap
           */
          return \"mov.d %I1,%0\;\" \"mov.d %D1,%t0\";
        }
        if ((idDst+2) > idSrc) {
          /*
           ** [wn] -> wn+2:wn+3:wn:wn+1
           */
          return \"mov.d %P1,%t0\;mov.d %p1,%0\";
        } else {
          /*
           ** [wn] -> wn-2:wn-1:wn:wn+1
           */
          return \"mov.d %I1,%0\;mov.d %1,%t0\";
        } 
      } else { 
        if ((idDst > idSrc) || ((idDst + 3) <= idSrc)) {
          /*  don't significantly overlap */ 
          return \"mov.d %1,%0\;mov [%r1+4],%t0\;mov [%r1+6],%q0\";
        }
        /* idDst <= idSrc < idDst+3 */
        switch (idDst + 4 - idSrc) {
          case 4:  /* idSrc == idDst+0 */
          case 3:  /* idSrc == idDst+1 */
            return \"mov [%r1+4],%t0\;mov [%r1+6],%q0\;mov.d %1,%0\";
          case 2:  /* idSrc == idDst+2 */
            return \"mov.d %1,%0\;mov [%r1+6],%q0\;mov [%r1+4],%t0\";
          default: abort();
        }
      }
    case 2: /* r = > */
      if (pic30_pp_modify_valid(0) == 0)
        return \"mov.d %1,%0\;mov.d %1,%t0\";
      else return \"mov %1,%0\;mov %1,%d0\;mov %1,%t0\;mov %1,%q0\";
    case 3: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      strcpy(temp, \"mov %1,%0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %Q1,%d0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %R1,%t0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %S1,%q0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      if (save[0]) {
        save[strlen(save)-2] = 0;
        strcat(szInsn, save);
      }
      return(szInsn);
    case 4: /* R = r */
      return \"mov.d %1,%I0\;\" \"mov.d %t1,%D0\";
    case 5: /* > = r */
      return \"mov.d %1,%0\;mov.d %t1,%0\";
    case 6: /* > = > */
      return \"mov %1,%0\;mov %1,%0\;mov %1,%0\;mov %1,%0\";
    case 7: /* Q = r */
      return \"mov %1,%0\;mov %d1,%Q0\;mov %t1,%R0\;mov %q1,%S0\";
    case 8: /* r = < */
      if (pic30_pp_modify_valid(0) == 0)
        return \"mov.d %1,%t0\;mov.d %1,%0\";
      else return \"mov %1,%q0\;mov %1,%t0\;mov %1,%d0\;mov %1,%0\";
    case 9: /* < = r */
      return \"mov.d %t1,%0\;mov.d %1,%0\";
    case 10: /* T = r */
      return \"mov %1,%0\;\"
             \"mov %d1,%Q0\;\"
             \"mov %t1,%R0\;\"
             \"mov %q1,%S0\";
    case 11: /* r = T */
      return \"mov %1,%0\;\"
             \"mov %Q1,%d0\;\"
             \"mov %R1,%t0\;\"
             \"mov %S1,%q0\";
    default:
      return \";\";
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,defuse,etc,def,defuse,etc,defuse,def,etc,def")
  ]
)

(define_insn "movdi_gen_APSV"
  [(set (match_operand:DI 0 "pic30_move_operand"
					"=r,r,r,r,R,>,>,Q,r,<,T,r")
        (unspec:DI [
          (match_operand:DI 1 "pic30_move_APSV_operand" 
					"r,R,>,Q,r,r,>,r,<,r,r,T")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
            
  ""
  "*
{
  int idSrc, idDst;
  char temp[64];
  char save[64];
  static char szInsn[64];

  szInsn[0] = 0;
  temp[0] = 0;
  save[0] = 0;

  switch (which_alternative) {
    case 0: /* r = r */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(operands[1]);
      if (idDst <= idSrc) {
        return \"mov.d %1,%0\;mov.d %t1,%t0\";
      } else {
        return \"mov.d %t1,%t0\;mov.d %1,%0\";
      }
    case 1: /* r = R */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(operands[1],0));
      if (pic30_pp_modify_valid(0) == 0) {
        if ((idDst > idSrc) || ((idDst+4) <= idSrc)) {
          /*
           ** source & dest don't overlap
           */
          return \"mov.d %I1,%0\;\" \"mov.d %D1,%t0\";
        }
        if ((idDst+2) > idSrc) {
          /*
           ** [wn] -> wn+2:wn+3:wn:wn+1
           */
          return \"mov.d %P1,%t0\;mov.d %p1,%0\";
        } else {
          /*
           ** [wn] -> wn-2:wn-1:wn:wn+1
           */
          return \"mov.d %I1,%0\;mov.d %1,%t0\";
        } 
      } else { 
        if ((idDst > idSrc) || ((idDst + 3) <= idSrc)) {
          /*  don't significantly overlap */ 
          return \"mov.d %1,%0\;mov [%r1+4],%t0\;mov [%r1+6],%q0\";
        }
        /* idDst <= idSrc < idDst+3 */
        switch (idDst + 4 - idSrc) {
          case 4:  /* idSrc == idDst+0 */
          case 3:  /* idSrc == idDst+1 */
            return \"mov [%r1+4],%t0\;mov [%r1+6],%q0\;mov.d %1,%0\";
          case 2:  /* idSrc == idDst+2 */
            return \"mov.d %1,%0\;mov [%r1+6],%q0\;mov [%r1+4],%t0\";
          default: abort();
        }
      }
    case 2: /* r = > */
      if (pic30_pp_modify_valid(0) == 0)
        return \"mov.d %1,%0\;mov.d %1,%t0\";
      else return \"mov %1,%0\;mov %1,%d0\;mov %1,%t0\;mov %1,%q0\";
    case 3: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      strcpy(temp, \"mov %1,%0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %Q1,%d0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %R1,%t0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %S1,%q0\;\");
      if (idDst != idSrc)
        strcat(szInsn, temp);
      else
        strcat(save, temp);
      idDst++;
      if (save[0]) {
        save[strlen(save)-2] = 0;
        strcat(szInsn, save);
      }
      return(szInsn);
    case 4: /* R = r */
      return \"mov.d %1,%I0\;\" \"mov.d %t1,%D0\";
    case 5: /* > = r */
      return \"mov.d %1,%0\;mov.d %t1,%0\";
    case 6: /* > = > */
      return \"mov %1,%0\;mov %1,%0\;mov %1,%0\;mov %1,%0\";
    case 7: /* Q = r */
      return \"mov %1,%0\;mov %d1,%Q0\;mov %t1,%R0\;mov %q1,%S0\";
    case 8: /* r = < */
      if (pic30_pp_modify_valid(0) == 0)
        return \"mov.d %1,%t0\;mov.d %1,%0\";
      else return \"mov %1,%q0\;mov %1,%t0\;mov %1,%d0\;mov %1,%0\";
    case 9: /* < = r */
      return \"mov.d %t1,%0\;mov.d %1,%0\";
    case 10: /* T = r */
      return \"mov %1,%0\;\"
             \"mov %d1,%Q0\;\"
             \"mov %t1,%R0\;\"
             \"mov %q1,%S0\";
    case 11: /* r = T */
      return \"mov %1,%0\;\"
             \"mov %Q1,%d0\;\"
             \"mov %R1,%t0\;\"
             \"mov %S1,%q0\";
    default:
      return \";\";
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type"
             "def,defuse,defuse,defuse,etc,def,defuse,etc,defuse,def,etc,def")
  ]
)

(define_expand "movdi"
  [(set (match_operand:DI 0 "pic30_general_operand" "")
        (match_operand:DI 1 "pic30_general_operand" ""))]
  ""
  "
{
	if (pic30_emit_move_sequence(operands, DImode)) DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Single Float (32 bit) moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Invalid move

(define_insn "*movsf_invalid_1"
  [(set (match_operand:SF 0 "pic30_register_operand"  "=r")
        (match_operand:SF 1 "pic30_code_operand" "g"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}

")
(define_insn "*movsf_invalid_2"
  [(set (match_operand:SF 0 "pic30_code_operand" "=g")
        (match_operand:SF 1 "pic30_register_operand"    "r"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")


(define_insn "*movsf_rimm"
  [(set (match_operand:SF 0 "pic30_register_operand" "=r,r")
        (match_operand:SF 1 "immediate_operand" "G,i"))]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
		return(\"mul.uu %0,#0,%0\;\");
	default:
		return(\"mov #%x1,%0\;mov #%w1,%d0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "movsf_general"
  [(set (match_operand:SF 0 "pic30_move_operand"
					"=r,r,r,r,R,R,>,>,Q,r,<,r,T")
        (match_operand:SF 1 "pic30_move_operand" 
					 "r,R,>,Q,r,R,r,>,r,<,r,T,r"))]
  ""
  "*
{
  int idDst, idSrc, pre;

  switch (which_alternative) {
    case 0: /* r = r */
      return \"mov.d %1,%0\";
    case 1: /* r = R */
      return \"mov.d %1,%0\";
    case 2: /* r = > */
      if ((pre = pic30_pp_modify_valid(operands[1])) == 0)
        return \"mov.d %1,%0\";
      else if (pre == -1) /* pre increment */
        return \"add %r1,#4,%r1\;mov.d %s1,%0\";
      else if (pre == 1)  /* post increment */
        return \"mov.d %s1,%0\;add %r1,#4,%r1\";
    case 3: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      if (idDst == idSrc) {
        return \"mov %Q1,%d0\;mov %1,%0\";
      } else {
        return \"mov %1,%0\;mov %Q1,%d0\";
      }
    case 4: /* R = r */
      return \"mov.d %1,%0\";
    case 5: /* R = R */
      return \"mov %I1,%I0\;mov %D1,%D0\";
    case 6: /* > = r */
      return \"mov.d %1,%0\";
    case 7: /* > = > */
      return \"mov %1,%0\;mov %1,%0\";
    case 8: /* Q = r */
      return \"mov %1,%0\;mov %d1,%Q0\";
    case 9: /* r = < */
      if ((pre = pic30_pp_modify_valid(operands[1])) == 0)
        return \"mov.d %1,%0\";
      else if (pre == -1) /* pre decrement */
        return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
      else if (pre == 1)  /* post decrement */
        return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
    case 10: /* < = r */
      return \"mov.d %1,%0\";
    case 11: /* r = T */
      return \"mov %1,%0\;mov %Q1,%d0\";
    case 12: /* T = r */
      return \"mov %1,%0\;mov %d1,%Q0\";
    default:
      return \";\";
  }
}"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "def,defuse,defuse,defuse,etc,use,def,defuse,etc,defuse,def,def,etc")
 ]
)

(define_insn "movsf_general_APSV"
  [(set (match_operand:SF 0 "pic30_move_operand"
					"=r,r,r,r,R,R,>,>,Q,r,<,r,T")
        (unspec:SF [
           (match_operand:SF 1 "pic30_move_APSV_operand" 
					 "r,R,>,Q,r,R,r,>,r,<,r,T,r")
           (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
  "*
{
  int idDst, idSrc, pre;

  switch (which_alternative) {
    case 0: /* r = r */
      return \"mov.d %1,%0\";
    case 1: /* r = R */
      return \"mov.d %1,%0\";
    case 2: /* r = > */
      if ((pre = pic30_pp_modify_valid(operands[1])) == 0)
        return \"mov.d %1,%0\";
      else if (pre == -1) /* pre increment */
        return \"add %r1,#4,%r1\;mov.d %s1,%0\";
      else if (pre == 1)  /* post increment */
        return \"mov.d %s1,%0\;add %r1,#4,%r1\";
    case 3: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      if (idDst == idSrc) {
        return \"mov %Q1,%d0\;mov %1,%0\";
      } else {
        return \"mov %1,%0\;mov %Q1,%d0\";
      }
    case 4: /* R = r */
      return \"mov.d %1,%0\";
    case 5: /* R = R */
      return \"mov %I1,%I0\;mov %D1,%D0\";
    case 6: /* > = r */
      return \"mov.d %1,%0\";
    case 7: /* > = > */
      return \"mov %1,%0\;mov %1,%0\";
    case 8: /* Q = r */
      return \"mov %1,%0\;mov %d1,%Q0\";
    case 9: /* r = < */
      if ((pre = pic30_pp_modify_valid(operands[1])) == 0)
        return \"mov.d %1,%0\";
      else if (pre == -1) /* pre decrement */
        return \"sub %r1,#4,%r1\;mov.d %s1,%0\";
      else if (pre == 1)  /* post decrement */
        return \"mov.d %s1,%0\;sub %r1,#4,%r1\";
    case 10: /* < = r */
      return \"mov.d %1,%0\";
    case 11: /* r = T */
      return \"mov %1,%0\;mov %Q1,%d0\";
    case 12: /* T = r */
      return \"mov %1,%0\;mov %d1,%Q0\";
    default:
      return \";\";
  }
}"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "def,defuse,defuse,defuse,etc,use,def,defuse,etc,defuse,def,def,etc")
 ]
)

(define_insn "*movsf_constnsfr"
  [(set (match_operand:SF 0 "pic30_near_operand" "=U,U")
        (match_operand:SF 1 "immediate_operand"   "G,i"))
	(clobber (match_scratch:HI 2             "=X,r"))]
  ""
  "*
{
	REAL_VALUE_TYPE r;
	long l = 0;

	switch (which_alternative)
	{
	case 0:
  		return(\"clr %0\;clr %0+2\");
	default:
		REAL_VALUE_FROM_CONST_DOUBLE(r, operands[1]);
		REAL_VALUE_TO_TARGET_SINGLE(r, l);
		if (l & 0xFFFF == 0)
		{
			return(	\"clr %0\;\"
				\"mov #%w1,%2\;\"
				\"mov %2,%0+2\");
		}
		else
		{
			return(	\"mov #%x1,%2\;\"
				\"mov %2,%0\;\"
				\"mov #%w1,%2\;\"
				\"mov %2,%0+2\");
		}
	}

}"
  [(set_attr "cc" "clobber")])

(define_expand "movsf"
  [(set (match_operand:SF 0 "pic30_general_operand" "")
        (match_operand:SF 1 "pic30_general_operand" ""))]
  ""
  "
{
	if (pic30_emit_move_sequence(operands, SFmode)) DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Double float (64 bit) moves
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Invalid move

(define_insn "*movdf_invalid_1"
  [(set (match_operand:DF 0 "pic30_register_operand"  "=r")
        (match_operand:DF 1 "pic30_code_operand" "g"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}

")
(define_insn "*movdf_invalid_2"
  [(set (match_operand:DF 0 "pic30_code_operand" "=g")
        (match_operand:DF 1 "pic30_register_operand"    "r"))]
  ""
  "*
{
        error(\"invalid address space for operand\");
        return(\"nop\");
}
")


(define_insn "*movdf_rimm"
  [(set (match_operand:DF 0 "pic30_register_operand" "=r,r")
        (match_operand:DF 1 "immediate_operand" "G,i"))]
  ""
  "*
{
	REAL_VALUE_TYPE r;
	long l[4] = { 0 };

	switch (which_alternative)
	{
	case 0:
		return(	\"mul.uu %0,#0,%0\;\"
			\"mul.uu %t0,#0,%t0\");
	default:
		REAL_VALUE_FROM_CONST_DOUBLE(r, operands[1]);
		REAL_VALUE_TO_TARGET_DOUBLE(r, l);
		if (l[0] == 0)
		{
			return( \"mul.uu %0,#0,%0\;\"
				\"mov #%x1,%t0\;\"
				\"mov #%w1,%q0\");
		}
		else
		{
			return( \"mov #%z1,%0\;\"
				\"mov #%y1,%d0\;\"
				\"mov #%x1,%t0\;\"
				\"mov #%w1,%q0\");
		}
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn_and_split "*movdf_rr"
  [(set (match_operand:DF 0 "pic30_register_operand" "=r")
        (match_operand:DF 1 "pic30_register_operand" " r"))]
  ""
  "mov.d %1,%0\;mov.d %t1,%t0"
  "reload_completed"
  [
   (set (subreg:SI (match_dup 0) 0) (subreg:SI (match_dup 1) 0))
   (set (subreg:SI (match_dup 0) 4) (subreg:SI (match_dup 1) 4))
  ]
  ""
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "movdf_gen"
  [(set (match_operand:DF 0 "pic30_move_operand"
					"=r,r,r,r,r,R,>,>,Q,<,r,T")
        (match_operand:DF 1 "pic30_move_operand" 
					 "r,R,<,>,Q,r,r,>,r,r,T,r"))]
  ""
{
  int idSrc, idDst;
  char temp[64];
  char save[64];
  static char insn[64];

  insn[0] = 0;
  temp[0] = 0;
  save[0] = 0;
  switch (which_alternative) {
    case 0: /* r = r */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(operands[1]);
      if (idDst <= idSrc) {
        return "mov.d %1,%0\;mov.d %t1,%t0";
      } else {
        return "mov.d %t1,%t0\;mov.d %1,%0";
      }
    case 1: /* r = R */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(operands[1],0));
      if (pic30_pp_modify_valid(0) == 0) {
        if ((idDst > idSrc) || ((idDst+4) <= idSrc)) {
          /*
          ** source & dest don't overlap
          */
          return "mov.d %I1,%0\;mov.d %D1,%t0";
        }
        if ((idDst+2) > idSrc) {
          /*
          ** [wn] -> wn+2:wn+3:wn:wn+1
          */
          return "mov.d %P1,%t0\;mov.d %p1,%0";
        } else {
          /*
          ** [wn] -> wn-2:wn-1:wn:wn+1
          */
          return "mov.d %I1,%0\;mov.d %1,%t0";
        }
      } else {
        if ((idDst > idSrc) || ((idDst + 3) <= idSrc)) {
          /*
          ** don't signifcantly overlap
          */
          return "mov.d %1,%0\;"
                 "mov [%r1+4],%t0\;"
                 "mov [%r1+6],%q0";
        }
        /* idDst <= idSrc < idDst+3 */
        switch (idDst + 4 - idSrc) {
          case 4:  /* idSrc == idDst+0 */
          case 3:  /* idSrc == idDst+1 */
            return "mov [%r1+4],%t0\;"
                   "mov [%r1+6],%q0\;"
                   "mov.d %1,%0";
          case 2:  /* idSrc == idDst+2 */
            return "mov.d %1,%0\;"
                   "mov [%r1+6],%q0\;"
                   "mov [%r1+4],%t0";
          default: abort();
        }
      }
    case 2: /* r = < */
      idDst = REGNO(operands[0]);
      if (pic30_pp_modify_valid(0) == 0) {
        return "mov.d %1,%t0\;mov.d %1,%0";
      } else {
        return "mov %1,%q0\;"
               "mov %1,%t0\;"
               "mov %1,%d0\;"
               "mov %1,%0";
      }
    case 3: /* r = > */
      idDst = REGNO(operands[0]);
      if (pic30_pp_modify_valid(0) == 0) {
        return "mov.d %1,%0\;mov.d %1,%t0";
      } else {
        return "mov %1,%0\;"
               "mov %1,%d0\;"
               "mov %1,%t0\;"
               "mov %1,%q0";
      }
    case 4: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      strcpy(temp, \"mov %1,%0\;\");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %Q1,%d0\;\");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %R1,%t0\;\");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, \"mov %S1,%q0\;\");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      if (save[0]) {
        save[strlen(save)-2] = 0;  /* delete last null, tab */
        strcat(insn, save);
      }
      return insn;
    case 5: /* R = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %1,%I0\;mov.d %t1,%D0";
    case 6: /* > = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %1,%0\;mov.d %t1,%0";
    case 7: /* > = > */
      return "mov %1,%0\;mov %1,%0\;mov %1,%0\;mov %1,%0";
    case 8: /* Q = r */
      return "mov %1,%0\;mov %d1,%Q0\;mov %t1,%R0\;mov %q1,%S0";
    case 9: /* < = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %t1,%0\;mov.d %1,%0";
    case 10: /* r = T */
      return "mov #%1,%0\;mov.d [++%0],%t0\;mov.d [--%0],%0";
    case 11: /* T = r */
      return "mov %1,%0\;mov %d1,%0+2\;mov %t1,%0+4\;mov %q1,%0+6";
    default: /* ? = ? */
      abort();
  }
  return "?mov? %1,%0";
}
 [(set_attr "cc" "clobber")
  (set_attr "type"
            "def,defuse,defuse,defuse,defuse,etc,def,defuse,etc,def,def,etc")
 ]
)

(define_insn "movdf_gen_APSV"
  [(set (match_operand:DF 0 "pic30_move_operand"
					"=r,r,r,r,r,R,>,>,Q,<,r,T")
        (unspec:DF [
          (match_operand:DF 1 "pic30_move_APSV_operand" 
					 "r,R,<,>,Q,r,r,>,r,r,T,r")
          (reg:HI PSVPAG)] UNSPECV_USEPSV))]
  ""
{
  int idSrc, idDst;
  char temp[64];
  char save[64];
  static char insn[64];

  insn[0] = 0;
  temp[0] = 0;
  save[0] = 0;
  switch (which_alternative) {
    case 0: /* r = r */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(operands[1]);
      if (idDst <= idSrc) {
        return "mov.d %1,%0\;mov.d %t1,%t0";
      } else {
        return "mov.d %t1,%t0\;mov.d %1,%0";
      }
    case 1: /* r = R */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(operands[1],0));
      if (pic30_pp_modify_valid(0) == 0) {
        if ((idDst > idSrc) || ((idDst+4) <= idSrc)) {
          /*
          ** source & dest don't overlap
          */
          return("mov.d %I1,%0\;mov.d %D1,%t0");
        }
        if ((idDst+2) > idSrc) {
          /*
          ** [wn] -> wn+2:wn+3:wn:wn+1
          */
          return "mov.d %P1,%t0\;mov.d %p1,%0";
        } else {
          /*
          ** [wn] -> wn-2:wn-1:wn:wn+1
          */
          return("mov.d %I1,%0\;mov.d %1,%t0");
        }
      } else {
        if ((idDst > idSrc) || ((idDst + 3) <= idSrc)) {
          /*
          ** don't signifcantly overlap
          */
          return "mov.d %1,%0\;"
                 "mov [%r1+4],%t0\;"
                 "mov [%r1+6],%q0";
        }
        /* idDst <= idSrc < idDst+3 */
        switch (idDst + 4 - idSrc) {
          case 4:  /* idSrc == idDst+0 */
          case 3:  /* idSrc == idDst+1 */
            return "mov [%r1+4],%t0\;"
                   "mov [%r1+6],%q0\;"
                   "mov.d %1,%0";
          case 2:  /* idSrc == idDst+2 */
            return "mov.d %1,%0\;"
                   "mov [%r1+6],%q0\;"
                   "mov [%r1+4],%t0";
          default: abort();
        }
      }
    case 2: /* r = < */
      idDst = REGNO(operands[0]);
      if (pic30_pp_modify_valid(0) == 0) {
        return "mov.d %1,%t0\;mov.d %1,%0";
      } else {
        return "mov %1,%q0\;"
               "mov %1,%t0\;"
               "mov %1,%d0\;"
               "mov %1,%0";
      }
    case 3: /* r = > */
      idDst = REGNO(operands[0]);
      if (pic30_pp_modify_valid(0) == 0) {
        return "mov.d %1,%0\;mov.d %1,%t0";
      } else {
        return "mov %1,%0\;"
               "mov %1,%d0\;"
               "mov %1,%t0\;"
               "mov %1,%q0";
      }
    case 4: /* r = Q */
      idDst = REGNO(operands[0]);
      idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
      strcpy(temp, "mov %1,%0\;");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
        strcpy(temp, "mov %Q1,%d0\;");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, "mov %R1,%t0\;");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      strcpy(temp, "mov %S1,%q0\;");
      if (idDst != idSrc)
        strcat(insn, temp);
      else
        strcat(save, temp);
      idDst++;
      if (save[0]) {
        save[strlen(save)-2] = 0;
        strcat(insn, save);
      }
      return insn;
    case 5: /* R = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %1,%I0\;mov.d %t1,%D0";
    case 6: /* > = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %1,%0\;mov.d %t1,%0";
    case 7: /* > = > */
      return "mov %1,%0\;mov %1,%0\;mov %1,%0\;mov %1,%0";
    case 8: /* Q = r */
      return "mov %1,%0\;mov %d1,%Q0\;mov %t1,%R0\;mov %q1,%S0";
    case 9: /* < = r */
      idSrc = REGNO(operands[1]);
      return "mov.d %t1,%0\;mov.d %1,%0";
    case 10: /* r = T */
      return "mov #%1,%0\;mov.d [++%0],%t0\;mov.d [--%0],%0";
    case 11: /* T = r */
      return "mov %1,%0\;mov %d1,%0+2\;mov %t1,%0+4\;mov %q1,%0+6";
    default: /* ? = ? */
      abort();
  }
  return "?mov? %1,%0";
}
 [(set_attr "cc" "clobber")
  (set_attr "type"
            "def,defuse,defuse,defuse,defuse,etc,def,defuse,etc,def,def,etc")
 ]
)

(define_expand "movdf"
  [(set (match_operand:DI 0 "pic30_general_operand" "")
        (match_operand:DI 1 "pic30_general_operand" ""))]
  ""
  "
{
	if (pic30_emit_move_sequence(operands, DFmode)) DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; add instructions 
;;
;; During the instruction canonicalization phase,
;; (minus x (const_int n)) is converted to (plus x (const_int -n)).
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "*addqi3_incdec_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r<>,R,r<>,R")
        (plus:QI 
           (match_operand:QI 1 "pic30_math_operand" "r,  r,R<>,R<>")
           (match_operand:QI 2 "immediate_operand" "i,  i,i,  i")))]
 "(-2<=INTVAL(operands[2]))&&(INTVAL(operands[2])!=0)&&(INTVAL(operands[2])<=2)"
  "*
{
	switch (INTVAL(operands[2]))
	{
	case -2:
		return(\"dec2.b %1,%0\");
	case -1:
		return(\"dec.b %1,%0\");
	case 1:
		return(\"inc.b %1,%0\");
	case 2:
		return(\"inc2.b %1,%0\");
	default:
		abort();
		return(\"nop\");
	}
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc,defuse,use")
  ]
)

(define_insn "*addqi3_incdecsfr"
  [(set (match_operand:QI 0 "pic30_wreg_or_near_operand"  "=U,a")
        (plus:QI (match_operand:QI 1 "pic30_near_operand" "%0,U")
                 (match_operand:QI 2 "immediate_operand"   "i,i")))]
 "(-2<=INTVAL(operands[2]))&&(INTVAL(operands[2])!=0)&&(INTVAL(operands[2])<=2)"
  "*
{
  switch (INTVAL(operands[2])) {
    case -2: switch (which_alternative) {
               case 0: return(\"dec2.b %0\");
               case 1: return(\"dec2.b %1,WREG\");
             }
    case -1: switch (which_alternative) {
               case 0: return(\"dec.b %0\");
               case 1: return(\"dec.b %1,WREG\");
             }
    case 1: switch (which_alternative) {
               case 0: return(\"inc.b %0\");
               case 1: return(\"inc.b %1,WREG\");
             }
    case 2: switch (which_alternative) {
               case 0: return(\"inc2.b %0\");
               case 1: return(\"inc2.b %1,WREG\");
             }
    default: abort();
  }
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "etc,def")
  ]
)

(define_insn "addqi3_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,r<>,R,  r<>,R,r<>,R,r,r")
        (plus:QI 
           (match_operand:QI 1 "pic30_math_operand"
              "%r,  r,  r,r,  r,  r,r,  r,R<>,R<>,N,  N,P,  P,0,J")
           (match_operand:QI 2 "pic30_mode1JN_operand"
              "r,  R<>,r,R<>,N,  N,P,  P,r,  r,  r,  r,r,  r,J,0")))]
  ""
  "@
   add.b %1,%2,%0
   add.b %1,%2,%0
   add.b %1,%2,%0
   add.b %1,%2,%0
   sub.b %1,#%J2,%0
   sub.b %1,#%J2,%0
   add.b %1,#%2,%0
   add.b %1,#%2,%0
   add.b %2,%1,%0
   add.b %2,%1,%0
   sub.b %2,#%J1,%0
   sub.b %2,#%J1,%0
   add.b %2,#%1,%0
   add.b %2,#%1,%0
   add.b #%2,%0
   add.b #%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,def,etc,defuse,use,def,etc,def,etc,def,def")
  ]
)

(define_insn "addqi3_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
               "=r<>,r<>,R,R,  r<>,R,r<>,R,r<>,R,  r<>,R,r<>,R,r,r")
        (plus:QI 
           (match_operand:QI 1 "pic30_math_APSV_operand"
               "%r,  r,  r,r,  r,  r,r,  r,R<>,R<>,N,  N,P,  P,0,J")
           (match_operand:QI 2 "pic30_mode1JN_APSV_operand"
               "r,  R<>,r,R<>,N,  N,P,  P,r,  r,  r,  r,r,  r,J,0")))]
  ""
  "@
   add.b %1,%2,%0
   add.b %1,%2,%0
   add.b %1,%2,%0
   add.b %1,%2,%0
   sub.b %1,#%J2,%0
   sub.b %1,#%J2,%0
   add.b %1,#%2,%0
   add.b %1,#%2,%0
   add.b %2,%1,%0
   add.b %2,%1,%0
   sub.b %2,#%J1,%0
   sub.b %2,#%J1,%0
   add.b %2,#%1,%0
   add.b %2,#%1,%0
   add.b #%2,%0
   add.b #%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,def,etc,defuse,use,def,etc,def,etc,def,def")
  ]
)

(define_expand "addqi3"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
               "=r<>,r<>,R,R,  r<>,R,r<>,R,r<>,R,  r<>,R,r<>,R,r,r")
        (plus:QI
           (match_operand:QI 1 "pic30_math_APSV_operand"
               "%r,  r,  r,r,  r,  r,r,  r,R<>,R<>,N,  N,P,  P,0,J")
           (match_operand:QI 2 "pic30_mode1JN_APSV_operand"
               "r,  R<>,r,R<>,N,  N,P,  P,r,  r,  r,  r,r,  r,J,0")))]
  ""
  "
{
  if (pic30_math_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1JN_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_addqi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_addqi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*addqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_near_operand"         "=U")
        (plus:QI (match_dup 0)
                 (match_operand:QI 1 "pic30_wreg_operand" "a")))]
  ""
  "add.b %0"
  [(set_attr "cc" "math")])

(define_insn "*addqi3_sfr1"
  [(set (match_operand:QI 0 "pic30_near_operand"         "=U")
        (plus:QI (match_operand:QI 1 "pic30_wreg_operand" "a")
                 (match_dup 0)))]
  ""
  "add.b %0"
  [(set_attr "cc" "math")])

(define_insn_and_split "*addqi3_sfr2"
  [(set (match_operand:QI 0 "pic30_register_operand"            "=a ,a, d")
        (plus:QI (match_operand:QI 1 "pic30_near_operand" "%U ,U, U")
                 (match_operand:QI 2 "pic30_register_operand"   " a ,d, d")))
   (clobber (match_scratch:HI 3                           "=X ,X,&r"))]
  ""
  "@
   add.b %1,WREG
   mov.b %2,w0\;add.b %1,WREG
   mov #%1,%3\;add.b %2,[%3],%0"
  "reload_completed"
  [
   (const_int 0)
  ]
"
{
  if (!pic30_wreg_operand(operands[0], QImode) &&
      !pic30_wreg_operand(operands[2], QImode))
  {
  	rtx pop = gen_rtx_MEM(QImode, operands[3]);
	emit_insn(gen_movhi_address(operands[3], XEXP(operands[1],0)));
	emit_insn(gen_addqi3(operands[0], operands[2], pop));
  	DONE;
  }
  else
  {
  	FAIL;
  }
}
"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;
(define_predicate "new_pic30_inc_imm_operand"
  (and (match_operand 0 "immediate_operand")
       (match_test "(-2<=INTVAL(op)) && (INTVAL(op)!=0) && (INTVAL(op)<=2)")))
  
(define_mode_iterator A16BITADD [HI P16APSV P16PMP])

(define_insn "add<mode>3"
  [(set (match_operand:A16BITADD 0 "pic30_mode2_operand"
              "=r<>,R,  r<>,R,r<>,r<>,R,R,  r<>,R,r<>,R,R,r<>,R,  r<>,r<>,R,r<>,R,r,r,r,  !?r,!?r")
        (plus:A16BITADD 
           (match_operand:A16BITADD 1 "pic30_JMmath_operand"
              "%r<>,r<>,R,  R,r,  r,  r,r,  r,  r,r,  r,r,r,  R<>,R<>,N,  N,P,  P,0,J,NPJ,JM, r")
           (match_operand:A16BITADD 2 "pic30_JMmath_operand"
              "L,   L,  L,  L,r,  R<>,r,R<>,N,  N,P,  P,r,r,  r,  r,  r,  r,r,  r,J,0,NPJ,r,  JM"))
  )]
  ""
  "*
{
   switch (which_alternative) {
     case  0:
     case  1:
     case  2:
     case  3:
       switch (INTVAL(operands[2])) {
         case -2:  return(\"dec2 %1,%0\");
         case -1:  return(\"dec %1,%0\");
         case 1:  return(\"inc %1,%0\");
         case 2:  return(\"inc2 %1,%0\");
         default: error(\"unknown L value: addhi3\");
                  return \"bad adhhi3\";
       }
       break;
     case  4: return \"add %1,%2,%0\";
     case  5: return \"add %1,%2,%0\";
     case  6: return \"add %1,%2,%0\";
     case  7: return \"add %1,%2,%0\";
     case  8: return \"sub %1,#%J2,%0\";
     case  9: return \"sub %1,#%J2,%0\";
     case 10: return \"add %1,#%2,%0\";
     case 11: return \"add %1,#%2,%0\";
     case 12: return \"add %2,%1,%0\";
     case 13: return \"add %2,%1,%0\";
     case 14: return \"add %2,%1,%0\";
     case 15: return \"add %2,%1,%0\";
     case 16: return \"sub %2,#%J1,%0\";
     case 17: return \"sub %2,#%J1,%0\";
     case 18: return \"add %2,#%1,%0\";
     case 19: return \"add %2,#%1,%0\";
     case 20: return \"add #%2,%0\";
     case 21: return \"add #%1,%0\";
     case 22: return \"mov #(%1+%2),%0\";
     case 23: if (REGNO(operands[0]) != REGNO(operands[2]))
                return \"mov #%1,%0\;add %0,%2,%0\";
              else {
                if (INTVAL(operands[1]) < 0)
                  return \"sub #%J1,%0\";
                else 
                  return \"add #%1,%0\";
              }
     case 24: if (REGNO(operands[0]) != REGNO(operands[1])) 
                return \"mov #%2,%0\;add %0,%1,%0\";
              else {
                if (INTVAL(operands[2]) < 0)
                  return \"sub #%J2,%0\";
                else 
                  return \"add #%2,%0\";
              }
     default: error(\"unknown alternative: addhi3\");
              return \"bad addhi3\";
   }
}"
 
  [(set_attr "cc" "math")
   (set_attr "type" "def,defuse,def,defuse,def,defuse,etc,use,def,etc,def,etc,etc,def,use,defuse,def,etc,def,etc,def,def,def,def,def")
  ])

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "*addhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"   "+U,r")
        (plus:HI (match_dup 0)
                 (match_operand:HI 1 "pic30_wreg_operand" "a,a")))]
  ""
  "@
   add %0
   add %0,%1,%0"
  [(set_attr "cc" "math")])

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "*addhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"   "+U,r")
        (plus:HI (match_operand:HI 1 "pic30_wreg_operand" "a,a")
                 (match_dup 0)))]
  ""
  "@
   add %0
   add %0,%1,%0"
  [(set_attr "cc" "math")])

(define_insn "*addhi3_sfr2"
  [(set (match_operand:HI 0 "pic30_wreg_operand"          "=a")
        (plus:HI (match_operand:HI 1 "pic30_near_operand" "%U")
                 (match_operand:HI 2 "pic30_wreg_operand" " a")))]
  ""
  "*
{
   switch (0) {
     case 0: return \"add %1,WREG\";
     case 1: return \"mov %2,w0\;add %1,WREG\";
     case 2: if (REGNO(operands[0]) != REGNO(operands[2])) 
               return \"mov #%1,%0\;add %2,[%0],%0\";
             else {
               gcc_assert(0);
             }
   }
}"
;  "reload_completed"
;  [
;   (set (match_dup 3) (match_dup 1))
;   (set (match_dup 0) (plus:HI (match_dup 2) (match_dup 3)))
;  ]
;
;{
;  if (!pic30_wreg_operand(operands[0], HImode) &&
;      !pic30_wreg_operand(operands[2], HImode))
;  {
;  }
;  else
;  {
;  	FAIL;
;  }
;}
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addhi3_incdecsfr"
  [(set (match_operand:HI 0 "pic30_wreg_or_near_operand"  "=U,a,d")
        (plus:HI (match_operand:HI 1 "pic30_near_operand" "%0,U,U")
                 (match_operand:HI 2 "immediate_operand"  " i,i,i")))]
 "(-2<=INTVAL(operands[2]))&&(INTVAL(operands[2])!=0)&&(INTVAL(operands[2])<=2)"
  "*
{
  switch (INTVAL(operands[2])) {
    case -2: switch (which_alternative) {
               case 0: return(\"dec2 %0\");
               case 1: return(\"dec2 %1,WREG\");
               case 2: return(\"mov %1,%0\;dec2 %0,%0\");
               default: abort();
             }
    case -1: switch (which_alternative) {
               case 0: return(\"dec %0\");
               case 1: return(\"dec %1,WREG\");
               case 2: return(\"mov %1,%0\;dec %0,%0\");
               default: abort();
             }
    case 1: switch (which_alternative) {
               case 0: return(\"inc %0\");
               case 1: return(\"inc %1,WREG\");
               case 2: return(\"mov %1,%0\;inc %0,%0\");
               default: abort();
             }
    case 2: switch (which_alternative) {
               case 0: return(\"inc2 %0\");
               case 1: return(\"inc2 %1,WREG\");
               case 2: return(\"mov %1,%0\;inc2 %0,%0\");
               default: abort();
             }
    default: abort();
  }
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "etc,def,def")
  ]
)

;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;

(define_insn "*addsihi3"
  [(set (match_operand:SI 0 "pic30_register_operand"         "=r")
        (plus:SI 
           (match_operand:SI 1 "pic30_register_operand" "r")
           (zero_extend:SI (match_operand:HI 2 "pic30_register_operand" "r"))))]
  ""
  "add %1,%2,%0\;addc %d1,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addhisi3"
  [(set (match_operand:SI 0 "pic30_register_operand"                       "=r")
        (plus:SI 
           (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "r"))
           (match_operand:SI 2 "pic30_register_operand" "r")))]
  ""
  "add %2,%1,%0\;addc %d2,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addsi3_imm"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r,r,r,r")
        (plus:SI (match_operand:SI 1 "pic30_register_operand" "%r,r,0,0")
                 (match_operand:SI 2 "pic30_JM_operand"       " P,N,J,M")))]
  ""
  "@
   add %1,#%2,%0\;addc %d1,#0,%d0
   sub %1,#%J2,%0\;subb %d1,#0,%d0
   add #%2,%0\;addc #0,%d0
   sub %0,#%J2\;subb %d0,#0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addsi3_immmsw"
  [(set (match_operand:SI          0 "pic30_register_operand" "=r,r")
        (plus:SI (match_operand:SI 1 "pic30_register_operand" "%0,r")
                 (match_operand:SI 2 "immediate_operand"      " i,i")))]
  "((INTVAL(operands[2]) & 0x0000FFFF) == 0) && 
   (-31 < (INTVAL(operands[2]) >> 16)) && ((INTVAL(operands[2]) >> 16) < 31)"
  "*
{
  int i = INTVAL(operands[2]) >> 16;
  static char szInsn[48];
  switch (which_alternative)
  {
    case 0:
      if (i < 0) sprintf(szInsn, \"sub %%d1,#%d,%%d0\", -i);
      else sprintf(szInsn, \"add %%d1,#%d,%%d0\", i);
      break;
    case 1:
      if (i < 0) sprintf(szInsn, \"sub %%1,#0,%%0\;subb %%d1,#%d,%%d0\",-i);
      else sprintf(szInsn, \"add %%1,#0,%%0\;addc %%d1,#%d,%%d0\", i);
      break;
  }
  return(szInsn);
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "addsi3_errata_APSV"
 [(set (match_operand:SI 0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:SI 
         (match_operand:SI 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:SI 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")
  ))
  (clobber (match_scratch:HI 3                         "=X,X,X,&r,X,X,X,X,X,&r")
  )
 ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;mov %D1,%3\;addc %d2,%3,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;mov %D2,%3\;addc %d1,%3,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addsi3_DATA"
 [(set (match_operand:SI   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:SI  
         (match_operand:SI 1 "pic30_rR_or_near_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:SI 2 "pic30_rR_or_JN_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  ""
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addsi3_noerrata_APSV"
 [(set (match_operand:SI   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:SI  
         (match_operand:SI 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:SI 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_expand "addsi3"
  [(set (match_operand:SI 0 "pic30_rR_or_near_operand" "")
        (plus:SI
          (match_operand:SI 1 "pic30_rR_or_near_APSV_operand" "")
          (match_operand:SI 2 "pic30_rR_or_JN_APSV_operand" "")))]
  ""
  "
{ 
  if (pic30_rR_or_near_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_rR_or_JN_APSV_operand(operands[2],GET_MODE(operands[2]))) {
    emit_insn(gen_addsi3_DATA(operands[0], operands[1], operands[2]));
  } else if (pic30_errata_mask & psv_errata) {
    emit_insn(gen_addsi3_errata_APSV(operands[0], operands[1], operands[2]));
  } else {
    emit_insn(gen_addsi3_noerrata_APSV(operands[0], operands[1], operands[2]));
  }
  DONE;
}")

(define_insn "addsi3x_DATA"
  [(set (match_operand:SI   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:SI 
          (match_operand:SI 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:SI 2 "pic30_mode2mres_operand" "r,  R, >,r,R,>,r,R,>")))
  ]
  ""
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_insn "addsi3x_noerrata_APSV"
  [(set (match_operand:SI   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:SI 
          (match_operand:SI 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:SI 2 "pic30_mode2mres_APSV_operand" "r,  R, >,r,R,>,r,R,>")))
  ]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_insn "addsi3x_errata_APSV"
  [(set (match_operand:SI   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:SI 
          (match_operand:SI 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:SI 2 "pic30_mode2mres_APSV_operand" " r, R, >,r,R,>,r,R,>")
   ))
   (clobber (match_scratch:HI 3                     "=X,&r,&r,X,&r,&r,X,&r,&r"))
  ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;mov %D2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%0\;mov %d2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;mov %D2,%3\;addc %d1,%3,%D0\",
      \"add %1,%2,%I0\;mov %d2,%3\;addc %d1,%3,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;mov %D2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%0\;mov %d2,%3\;addc %d1,%3,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

;;;;;;;;;;;;;;;;;
;; P32DF integer
;;;;;;;;;;;;;;;;;

(define_insn "*addp32dfhi3"
  [(set (match_operand:P32DF 0 "pic30_register_operand"         "=r")
        (plus:P32DF 
           (match_operand:P32DF 1 "pic30_register_operand" "r")
           (zero_extend:P32DF (match_operand:HI 2 "pic30_register_operand" "r"))))]
  ""
  "add %1,%2,%0\;addc %d1,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addhip32df3"
  [(set (match_operand:P32DF 0 "pic30_register_operand"                       "=r")
        (plus:P32DF 
           (zero_extend:P32DF (match_operand:HI 1 "pic30_register_operand" "r"))
           (match_operand:P32DF 2 "pic30_register_operand" "r")))]
  ""
  "add %2,%1,%0\;addc %d2,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addp32df3_imm"
  [(set (match_operand:P32DF 0 "pic30_register_operand"          "=r,r,r,r")
        (plus:P32DF (match_operand:SI 1 "pic30_register_operand" "%r,r,0,0")
                 (match_operand:P32DF 2 "pic30_JM_operand"       " P,N,J,M")))]
  ""
  "@
   add %1,#%2,%0\;addc %d1,#0,%d0
   sub %1,#%J2,%0\;subb %d1,#0,%d0
   add #%2,%0\;addc #0,%d0
   sub %0,#%J2\;subb %d0,#0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*addp32df3_immmsw"
  [(set (match_operand:P32DF          0 "pic30_register_operand" "=r,r")
        (plus:P32DF (match_operand:SI 1 "pic30_register_operand" "%0,r")
                 (match_operand:P32DF 2 "immediate_operand"      " i,i")))]
  "((INTVAL(operands[2]) & 0x0000FFFF) == 0) && 
   (-31 < (INTVAL(operands[2]) >> 16)) && ((INTVAL(operands[2]) >> 16) < 31)"
  "*
{
  int i = INTVAL(operands[2]) >> 16;
  static char szInsn[48];
  switch (which_alternative)
  {
    case 0:
      if (i < 0) sprintf(szInsn, \"sub %%d1,#%d,%%d0\", -i);
      else sprintf(szInsn, \"add %%d1,#%d,%%d0\", i);
      break;
    case 1:
      if (i < 0) sprintf(szInsn, \"sub %%1,#0,%%0\;subb %%d1,#%d,%%d0\",-i);
      else sprintf(szInsn, \"add %%1,#0,%%0\;addc %%d1,#%d,%%d0\", i);
      break;
  }
  return(szInsn);
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "addp32df3_errata_APSV"
 [(set (match_operand:P32DF 0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:P32DF 
         (match_operand:P32DF 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:P32DF 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")
  ))
  (clobber (match_scratch:HI 3                         "=X,X,X,&r,X,X,X,X,X,&r")
  )
 ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;mov %D1,%3\;addc %d2,%3,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;mov %D2,%3\;addc %d1,%3,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addp32df3_DATA"
 [(set (match_operand:P32DF   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:P32DF  
         (match_operand:P32DF 1 "pic30_rR_or_near_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:P32DF 2 "pic30_rR_or_JN_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  ""
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addp32df3_noerrata_APSV"
 [(set (match_operand:P32DF   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:P32DF  
         (match_operand:P32DF 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:P32DF 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_expand "addp32df3"
  [(set (match_operand:P32DF 0 "pic30_rR_or_near_operand" "")
        (plus:P32DF
          (match_operand:P32DF 1 "pic30_rR_or_near_APSV_operand" "")
          (match_operand:P32DF 2 "pic30_rR_or_JN_APSV_operand" "")))]
  ""
  "
{ 
  if (pic30_rR_or_near_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_rR_or_JN_APSV_operand(operands[2],GET_MODE(operands[2]))) {
    emit_insn(gen_addp32df3_DATA(operands[0], operands[1], operands[2]));
  } else if (pic30_errata_mask & psv_errata) {
    emit_insn(gen_addp32df3_errata_APSV(operands[0], operands[1], operands[2]));
  } else {
    emit_insn(gen_addp32df3_noerrata_APSV(operands[0], operands[1], operands[2]));
  }
  DONE;
}")

(define_insn "addp32df3x_DATA"
  [(set (match_operand:P32DF   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:P32DF 
          (match_operand:P32DF 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:P32DF 2 "pic30_mode2mres_operand" "r,  R, >,r,R,>,r,R,>")))
  ]
  ""
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_insn "addp32df3x_noerrata_APSV"
  [(set (match_operand:P32DF   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:P32DF 
          (match_operand:P32DF 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:P32DF 2 "pic30_mode2mres_APSV_operand" "r,  R, >,r,R,>,r,R,>")))
  ]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;addc %d1,%D2,%d0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_insn "addp32df3x_errata_APSV"
  [(set (match_operand:P32DF   0 "pic30_mode2mres_operand" "=r,&r,&r,R,R,R,>,>,>")
        (plus:P32DF 
          (match_operand:P32DF 1 "pic30_register_operand"  "%r, r, r,r,r,r,r,r,r")
          (match_operand:P32DF 2 "pic30_mode2mres_APSV_operand" " r, R, >,r,R,>,r,R,>")
   ))
   (clobber (match_scratch:HI 3                     "=X,&r,&r,X,&r,&r,X,&r,&r"))
  ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
   static char *patterns[] = {
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;mov %D2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%0\;mov %d2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
      \"add %1,%I2,%I0\;mov %D2,%3\;addc %d1,%3,%D0\",
      \"add %1,%2,%I0\;mov %d2,%3\;addc %d1,%3,%D0\",
      \"add %1,%2,%0\;addc %d1,%d2,%d0\",
      \"add %1,%I2,%0\;mov %D2,%3\;addc %d1,%3,%d0\",
      \"add %1,%2,%0\;mov %d2,%3\;addc %d1,%3,%d0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

;; P32EXT

(define_insn "addp32ext3_imm"
  [(set (match_operand:P32EXT 0 "pic30_register_operand"          "=r,r,r,r")
        (plus:P32EXT 
                 (match_operand:P32EXT 1 "pic30_register_operand" "%r,r,0,0")
                 (match_operand:P32EXT 2 "pic30_JM_operand"       " P,N,J,M")))]
  ""
  "@
   add %1,#%2,%0\;addc %d1,#0,%d0
   sub %1,#%J2,%0\;subb %d1,#0,%d0
   add #%2,%0\;addc #0,%d0
   sub %0,#%J2\;subb %d0,#0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "addp32ext3_errata_APSV"
 [(set (match_operand:P32EXT   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
   (plus:P32EXT
     (match_operand:P32EXT 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
     (match_operand:P32EXT 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")
  ))
  (clobber (match_scratch:HI 3                         "=X,X,X,&r,X,X,X,X,X,&r")
  )
 ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;mov %D1,%3\;addc %d2,%3,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;mov %D2,%3\;addc %d1,%3,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addp32ext3_DATA"
 [(set (match_operand:P32EXT   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:P32EXT
         (match_operand:P32EXT 1 "pic30_rR_or_near_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:P32EXT 2 "pic30_rR_or_JN_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  ""
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_insn "addp32ext3_noerrata_APSV"
 [(set (match_operand:P32EXT   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
       (plus:P32EXT
         (match_operand:P32EXT 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
         (match_operand:P32EXT 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
 ]
  "(!(pic30_errata_mask & psv_errata))"
  "*
{
  static char *patterns[] = {
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
    \"add #%2,%0\;addc #%y2,%d0\",
    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
    0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
)

(define_expand "addp32ext3"
  [(set (match_operand:P32EXT 0 "pic30_rR_or_near_operand" "")
        (plus:P32EXT
          (match_operand:P32EXT 1 "pic30_rR_or_near_APSV_operand" "")
          (match_operand:P32EXT 2 "pic30_rR_or_JN_APSV_operand" "")))]
  ""
  "
{
  if (pic30_rR_or_near_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_rR_or_JN_operand(operands[2],GET_MODE(operands[2]))) {
    emit_insn(gen_addp32ext3_DATA(operands[0], operands[1], operands[2]));
  } else if (pic30_errata_mask & psv_errata) {
    emit_insn(gen_addp32ext3_errata_APSV(operands[0], operands[1], operands[2]));
  } else {
    emit_insn(gen_addp32ext3_noerrata_APSV(operands[0], operands[1], operands[2]));
  }
  DONE;
}")

;; P32EDS

;(define_insn "*addp32eds3_imm"
;  [(set (match_operand:P32EDS 0 "pic30_register_operand"          "=r,r")
;        (plus:P32EDS
;                 (match_operand:P32EDS 1 "pic30_register_operand" "%r,r")
;                 (match_operand:P32EDS 2 "pic30_PN_opearnd"       " P,N")))]
;  ""
;  "@
;   sl %1,%0\;add #%o2,%0\;bset _SR,#1\;addc %d1,#0,%d0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0
;   sl %1,%0\;sub #%O2,%0\;bset _SR,#1\;subb %d1,#0,%d0\;bclr _SR,#0\;btss _SR,#1\;bset _SR,#0\;rrc %0,%0"
;  [
;   (set_attr "cc" "math")
;   (set_attr "type" "def")
;  ]
;)
;
;(define_insn "addp32eds3_errata_APSV"
; [(set (match_operand:P32EDS   0 "pic30_reg_or_R_operand" "=r,r,r,&r,r,r,r,R,R,R")
;   (plus:P32EDS
;     (match_operand:P32EDS 1 "pic30_reg_or_R_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
;     (match_operand:P32EDS 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")
;  ))
;  (clobber (match_scratch:HI 3                         "=X,X,X,&r,X,X,X,X,X,&r")
;  )
; ]
;  "(pic30_errata_mask & psv_errata)"
;  "*
;{
;  static char *patterns[] = {
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%I1,%0\;btsc _SR,#0\;bset %0,#15\;mov %D1,%3\;addc %d2,%3,%d0\",
;    \"add %1,#%2,%0\;btsc _SR,#0\;bset %0,#15\;addc %d1,#0,%d0\",
;    \"sub %1,#%J2,%0\;btsc _SR,#0\;bset %0,#15\;subb %d1,#0,%d0\",
;    \"add #%2,%0\;btsc _SR,#0\;bset %0,#15\;addc #%y2,%d0\",
;    \"add %1,%2,%I0\;btsc _SR,#0\;bset %0,#15\;addc %d1,%d2,%D0\",
;    \"add %1,%2,%0\;btsc _SR,#0\;bset %0,#15\;addc %d1,%P2,%D0\",
;    \"add %1,%I2,%I0\;btsc _SR,#0\;bset %0,#15\;mov %D2,%3\;addc %d1,%3,%D0\",
;    0};
;
;  return patterns[which_alternative];
;}"
;  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
;)
;
;(define_insn "addp32eds3_DATA"
; [(set (match_operand:P32EDS   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
;       (plus:P32EDS
;         (match_operand:P32EDS 1 "pic30_rR_or_near_operand" "%r,0,r, R,r,r,0,r,r,r")
;         (match_operand:P32EDS 2 "pic30_rR_or_JN_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
; ]
;  ""
;  "*
;{
;  static char *patterns[] = {
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%d1,%d0\",
;    \"add %2,%I1,%0\;btsc _SR,#0\;bset %0,#15\;addc %d2,%D1,%d0\",
;    \"add %1,#%2,%0\;btsc _SR,#0\;bset %0,#15\;addc %d1,#0,%d0\",
;    \"sub %1,#%J2,%0\;btsc _SR,#2\;dec %0,%0\;cp0 %0\;btsc _SR,#1\;bset %0,#15\",
;    \"add #%2,%0\;btsc _SR,#0\;bset %0,#15\;addc #%y2,%d0\",
;    \"add %1,%2,%I0\;btsc _SR,#0\;bset %0,#15\;addc %d1,%d2,%D0\",
;    \"add %1,%2,%0\;btsc _SR,#0\;bset %0,#15\;addc %d1,%P2,%D0\",
;    \"add %1,%I2,%I0\;btsc _SR,#0\;bset %0,#15\;addc %d1,%D2,%D0\",
;    0};
;
;  return patterns[which_alternative];
;}"
;  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
;)
;
;(define_insn "addp32eds3_noerrata_APSV"
; [(set (match_operand:P32EDS   0 "pic30_rR_or_near_operand" "=r,r,r,&r,r,r,r,R,R,R")
;       (plus:P32EDS
;         (match_operand:P32EDS 1 "pic30_rR_or_near_APSV_operand" "%r,0,r, R,r,r,0,r,r,r")
;         (match_operand:P32EDS 2 "pic30_rR_or_JN_APSV_operand"    "r,r,0, r,P,N,J,r,0,R")  ))
; ]
;  "(!(pic30_errata_mask & psv_errata))"
;  "*
;{
;  static char *patterns[] = {
;    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
;    \"add %2,%1,%0\;addc %d2,%d1,%d0\",
;    \"add %2,%I1,%0\;addc %d2,%D1,%d0\",
;    \"add %1,#%2,%0\;addc %d1,#0,%d0\",
;    \"sub %1,#%J2,%0\;subb %d1,#0,%d0\",
;    \"add #%2,%0\;addc #%y2,%d0\",
;    \"add %1,%2,%I0\;addc %d1,%d2,%D0\",
;    \"add %1,%2,%0\;addc %d1,%P2,%D0\",
;    \"add %1,%I2,%I0\;addc %d1,%D2,%D0\",
;    0};
;
;  return patterns[which_alternative];
;}"
;  [(set_attr "cc" "math,math,math,math,math,math,math,math,math,math")]
;)

;(define_expand "addp32eds"
;  [(set (match_operand:P32EDS 0 "pic30_rR_or_near_operand" "")
;        (plus:P32EDS
;          (match_operand:P32EDS 1 "pic30_rR_or_near_APSV_operand" "")
;          (match_operand:P32EDS 2 "pic30_rR_or_JN_APSV_operand" "")))]
;""
;"
;{
;  if (pic30_rR_or_near_operand(operands[1],GET_MODE(operands[1])) &&
;      pic30_rR_or_JN_operand(operands[2],GET_MODE(operands[2]))) {
;    emit_insn(gen_addp32eds3_DATA(operands[0], operands[1], operands[2]));
;  } else if (pic30_errata_mask & psv_errata) {
;    emit_insn(gen_addp32eds3_errata_APSV(operands[0], operands[1], operands[2]));
;  } else {
;    emit_insn(gen_addp32eds3_noerrata_APSV(operands[0], operands[1], operands[2]));
;  }
;  DONE;
;}")

;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;

(define_insn "*adddi3_imm"
  [(set (match_operand:DI          0 "pic30_register_operand" "=r,r,r,r")
        (plus:DI (match_operand:DI 1 "pic30_register_operand" "%r,r,0,0")
                 (match_operand:DI 2 "pic30_JM_operand"       " P,N,J,M")))]
  ""
  "@
   add %1,#%2,%0\;addc %d1,#0,%d0\;addc %t1,#0,%t0\;addc %q1,#0,%q0
   sub %1,#%J2,%0\;subb %d1,#0,%d0\;subb %t1,#0,%t0\;subb %q1,#0,%q0
   add #%2,%0\;addc #%y2,%d0\;addc #%x2,%t0\;addc #%w2,%q0
   sub %0,#%J2\;subb %d0,#0\;subb %t0,#0\;subb %q0,#0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "adddi3"
  [(set (match_operand:DI          0 "pic30_register_operand" "=r")
        (plus:DI (match_operand:DI 1 "pic30_register_operand" "%r")
                 (match_operand:DI 2 "pic30_register_operand" " r")))]
  ""
  "add %2,%1,%0\;addc %d2,%d1,%d0\;addc %t2,%t1,%t0\;addc %q2,%q1,%q0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;
;; float
;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; subtract instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "*subqi3_imm"
  [(set (match_operand:QI 0 "pic30_register_operand"          "=r,r")
        (minus:QI (match_operand:QI 1 "pic30_register_operand" "0,0")
                  (match_operand:QI 2 "pic30_JM_operand" "J,M")))]
  ""
  "@
   sub.b %0,#%2
   add.b #%J2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "subqi3_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r<>,r<>,R,R,  r<>,R,r<>,R")
        (minus:QI 
          (match_operand:QI 1 "pic30_register_operand" "r, r, r,r, r, r,r, r")
          (match_operand:QI 2 "pic30_mode1PN_operand"  "r,R<>,r,R<>,N,N,P,P")))]
  ""
  "@
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   add.b %1,#%J2,%0
   add.b %1,#%J2,%0
   sub.b %1,#%2,%0
   sub.b %1,#%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,def,etc")
  ]
)

(define_insn "subqi3_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r<>,r<>,R,R,  r<>,R,r<>,R")
        (minus:QI 
           (match_operand:QI 1 "pic30_register_operand" "r,r,r,r,r,r,r,r")
           (match_operand:QI 2 "pic30_mode1PN_APSV_operand" 
                               "r,R<>,r,R<>,N,N,P,P")))]
  ""
  "@
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   sub.b %1,%2,%0
   add.b %1,#%J2,%0
   add.b %1,#%J2,%0
   sub.b %1,#%2,%0
   sub.b %1,#%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,def,etc")
  ]
)

(define_expand "subqi3"
  [(set (match_operand:QI 0 "pic30_mode2_operand" "=r<>,r<>,R,R,  r<>,R,r<>,R")
        (minus:QI 
           (match_operand:QI 1 "pic30_register_operand" "r,r,r,r,r,r,r,r")
           (match_operand:QI 2 "pic30_mode1PN_APSV_operand" 
                               "r,R<>,r,R<>,N,N,P,P")))]
  ""
  "
{
  if (pic30_mode1PN_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_subqi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_subqi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*subqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_near_operand"          "=U")
        (minus:QI (match_dup 0)
                  (match_operand:QI 1 "pic30_wreg_operand" "a")))]
  ""
  "sub.b %0"
  [(set_attr "cc" "math")])

(define_insn "*subqi3_sfr1"
  [(set (match_operand:QI 0 "pic30_register_operand"             "=a,a,d")
        (minus:QI (match_operand:QI 1 "pic30_near_operand" " U,U,U")
                  (match_operand:QI 2 "pic30_register_operand"   " a,d,d")))
   (clobber (match_scratch:QI 3                            "=X,X,&r"))]
  ""
  "@
   sub.b %1,WREG
   mov.b %2,w0\;sub.b %1,WREG
   mov #%1,%3\;subr.b %2,[%3],%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "*subhi3_imm"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=r,r")
        (minus:HI (match_dup 0)
                  (match_operand:HI 1 "pic30_JM_operand" "J,M")))]
  ""
  "@
   sub %0,#%1
   add #%J1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "subhi3_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,R,r<>,R,  r<>,R,r<>,R,  r<>,R,r<>,R,r<>,R")
        (minus:HI 
           (match_operand:HI 1 "pic30_mode1P_operand"
              "r,  r,r,  r,  r,  r,R<>,R<>,r,  r,r,  r,P,  P")
           (match_operand:HI 2 "pic30_mode1PN_operand"
              "r,  r,R<>,R<>,r,  r,r,  r,  N,  N,P,  P,r,  r")))]
  ""
  "@
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   add %1,#%J2,%0
   add %1,#%J2,%0
   sub %1,#%2,%0
   sub %1,#%2,%0
   subr %2,#%1,%0
   subr %2,#%1,%0"
  [(set_attr "cc" "math")
   (set_attr "type"
  	 "def,etc,defuse,use,def,etc,defuse,use,def,etc,def,etc,def,etc")
  ])

(define_insn "subhi3_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,R,r<>,R,  r<>,R,r<>,R,  r<>,R,r<>,R,r<>,R")
        (minus:HI 
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "r,  r,r,  r,  r,  r,R<>,R<>,r,  r,r,  r,P,  P")
           (match_operand:HI 2 "pic30_mode1PN_APSV_operand"
              "r,  r,R<>,R<>,r,  r,r,  r,  N,  N,P,  P,r,  r")))]
  ""
  "@
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   add %1,#%J2,%0
   add %1,#%J2,%0
   sub %1,#%2,%0
   sub %1,#%2,%0
   subr %2,#%1,%0
   subr %2,#%1,%0"
  [(set_attr "cc" "math")
   (set_attr "type"
  	 "def,etc,defuse,use,def,etc,defuse,use,def,etc,def,etc,def,etc")
  ])

(define_insn "subp16apsv3"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand"
              "=r<>,R,r<>,R,  r<>,R,r<>,R,  r<>,R,r<>,R,r<>,R")
        (minus:P16APSV 
           (match_operand:P16APSV 1 "pic30_mode1P_operand"
              "r,  r,r,  r,  r,  r,R<>,R<>,r,  r,r,  r,P,  P")
           (match_operand:P16APSV 2 "pic30_mode1PN_operand"
              "r,  r,R<>,R<>,r,  r,r,  r,  N,  N,P,  P,r,  r")))]
  ""
  "@
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   sub %1,%2,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   subr %2,%1,%0
   add %1,#%J2,%0
   add %1,#%J2,%0
   sub %1,#%2,%0
   sub %1,#%2,%0
   subr %2,#%1,%0
   subr %2,#%1,%0"
  [(set_attr "cc" "math")
   (set_attr "type"
  	 "def,etc,defuse,use,def,etc,defuse,use,def,etc,def,etc,def,etc")
  ])

(define_expand "subhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,R,r<>,R,  r<>,R,r<>,R,  r<>,R,r<>,R,r<>,R")
        (minus:HI
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "r,  r,r,  r,  r,  r,R<>,R<>,r,  r,r,  r,P,  P")
           (match_operand:HI 2 "pic30_mode1PN_APSV_operand"
              "r,  r,R<>,R<>,r,  r,r,  r,  N,  N,P,  P,r,  r")))]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1PN_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_subhi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_subhi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "subhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"    "+U,r")
        (minus:HI (match_dup 0)
                  (match_operand:HI 1 "pic30_wreg_operand" "a,a")))]
  ""
  "@
   sub %0
   sub %0,%1,%0"
  [(set_attr "cc" "math")])

(define_insn_and_split "*subhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_register_operand"             "=a ,a, d")
        (minus:HI (match_operand:HI 1 "pic30_near_operand" " U ,U, U")
                  (match_operand:HI 2 "pic30_register_operand"   " a ,d, d")))
   (clobber (match_scratch:HI 3                            "=X ,X,&r"))]
  ""
  "@
   sub %1,WREG
   mov %2,w0\;sub %1,WREG
   mov #%1,%3\;subr %2,[%3],%0"
  "reload_completed"
  [
   (set (match_dup 3) (match_dup 1))
   (set (match_dup 0) (minus:HI (match_dup 3) (match_dup 2)))
  ]
"
{
  if (!pic30_wreg_operand(operands[0], HImode) &&
      !pic30_wreg_operand(operands[2], HImode))
  {
  }
  else
  {
  	FAIL;
  }
}
"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;

(define_insn "*subsihi3"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r")
        (minus:SI (match_operand:SI 1 "pic30_register_operand" "r")
                  (zero_extend:SI (match_operand:HI 2 "pic30_register_operand" "r"))))
  ]
  ""
  "sub %1,%2,%0\;subb %d1,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*subsi3_imm0"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r,r")
        (minus:SI (match_operand:SI 1 "pic30_register_operand" "r,r")
                  (match_operand:SI 2 "pic30_PN_operand"       "N,P")))]
  ""
  "@
   add %1,#%J2,%0\;addc %d1,#0,%d0
   sub %1,#%2,%0\;subb %d1,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*subsi3_imm1"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r")
        (minus:SI (match_operand:SI 1 "pic30_P_operand"  "P")
                  (match_operand:SI 2 "pic30_register_operand" "r")))]
  ""
  "subr %2,#%1,%0\;subbr %d2,#0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*subsi3_imm2"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r,r")
        (minus:SI (match_operand:SI 1 "pic30_register_operand" "0,0")
                  (match_operand:SI 2 "pic30_JM_operand"       "J,M")))]
  ""
  "@
   sub %0,#%2\;subb %d0,#0
   add #%J2,%0\;addc #0,%d0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "subsi3_errata"
  [(set (match_operand:SI   0 "pic30_reg_or_R_operand" "=r,r,r,&r,R,R,R,&r,R,R")
        (minus:SI 
          (match_operand:SI 1 "pic30_reg_or_R_operand" " r,0,r, r,r,r,r, R,0,R")
          (match_operand:SI 2 "pic30_reg_or_R_operand" " r,r,0, R,r,0,R, r,r,r")
   ))
   (clobber (match_scratch:HI 3                       "=X,X,X,&r,X,&r,&r,X,X,X")
   )
  ]
  "(pic30_errata_mask & psv_errata)"
  "*
{
   static char *patterns[] = {
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%I2,%0\;mov %D2,%3\;subb %d1,%3,%d0\",
      \"sub %1,%2,%I0\;subb %d1,%d2,%D0\",
      \"sub %1,%2,%I0\;mov %2,%3\;subb %d1,%3,%D0\",
      \"sub %1,%I2,%I0\;mov %D2,%3\;subb %d1,%3,%D0\",
      \"subr %2,%I1,%0\;subbr %d2,%D1,%d0\",
      \"subr %2,%1,%I0\;subbr %d2,%1,%D0\",
      \"subr %2,%I1,%I0\;subbr %d2,%D1,%D0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_insn "subsi3_noerrata"
  [(set (match_operand:SI   0 "pic30_reg_or_R_operand" "=r,r,r,&r,R,R,R,&r,R,R")
        (minus:SI 
          (match_operand:SI 1 "pic30_reg_or_R_operand" "r,0,r, r,r,r,r,  R,0,R")
          (match_operand:SI 2 "pic30_reg_or_R_operand" "r,r,0, R,r,0,R, r,r,r")
   ))
  ]
  ""
  "*
{
   static char *patterns[] = {
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%2,%0\;subb %d1,%d2,%d0\",
      \"sub %1,%I2,%0\;subb %d1,%D2,%d0\",
      \"sub %1,%2,%I0\;subb %d1,%d2,%D0\",
      \"sub %1,%2,%I0\;subb %d1,%2,%D0\",
      \"sub %1,%I2,%I0\;subb %d1,%D2,%D0\",
      \"subr %2,%I1,%0\;subbr %d2,%D1,%d0\",
      \"subr %2,%1,%I0\;subbr %d2,%1,%D0\",
      \"subr %2,%I1,%I0\;subbr %d2,%D1,%D0\",
      0};

  return patterns[which_alternative];
}"
  [(set_attr "cc" "math")])

(define_expand "subsi3"
  [(set (match_operand:SI   0 "pic30_reg_or_R_operand" "")
        (minus:SI 
          (match_operand:SI 1 "pic30_reg_or_R_operand" "")
          (match_operand:SI 2 "pic30_reg_or_R_operand" "")))
  ]
  ""
  "
{
  if (pic30_errata_mask & psv_errata) {
    emit(gen_subsi3_errata(operands[0],operands[1],operands[2]));
  } else {
    emit(gen_subsi3_noerrata(operands[0],operands[1],operands[2]));
  }
  DONE;
}")

;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;

(define_insn "*subdi3_imm"
  [(set (match_operand:DI 0 "pic30_register_operand"           "=r,r")
        (minus:DI (match_operand:DI 1 "pic30_register_operand" " 0,0")
                  (match_operand:DI 2 "pic30_JM_operand"       " J,M")))]
  ""
  "@
   sub %0,#%2\;subb %d0,#0\;subb %t0,#0\;subb %q0,#0
   add #%J2,%0\;addc #0,%d0\;addc #0,%t0\;addc #0,%q0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "subdi3"
  [(set (match_operand:DI 0 "pic30_register_operand"          "=r")
        (minus:DI (match_operand:DI 1 "pic30_register_operand" "r")
                  (match_operand:DI 2 "pic30_register_operand" "r")))]
  ""
  "sub %1,%2,%0\;subb %d1,%d2,%d0\;subb %t1,%t2,%t0\;subb %q1,%q2,%q0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; multiply instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 16-bit product

;; 8 x 8 => 16 (unsigned)

(define_expand "umulqihi3"
  [(set (match_operand:HI 0 "pic30_register_operand"
						"")
    (mult:HI (zero_extend:HI (match_operand:QI 1 "pic30_register_operand"
						""))
             (zero_extend:HI (match_operand:QI 2 "pic30_reg_imm_or_near_operand"
						 ""))))]
;; NULLSTONE CSE fails when enabled
  "0"
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		int pow2 = INTVAL(operands[2]);
		if (pic30_one_bit_set_p(pow2))
		{
			emit_insn(gen_umulqihi3pow2(operands[0],
						operands[1], operands[2]));
		}
		else
		{
			emit_insn(gen_umulqihi3imm(operands[0],
						operands[1], operands[2]));
		}
	}
	else
	{
		emit_insn(gen_umulqihi3gen(operands[0],
						operands[1], operands[2]));
	}
	DONE;
}")

(define_insn "umulqihi3gen"
  [(set (match_operand:HI 0 "pic30_creg_operand"
						"=c,c")
    (mult:HI (zero_extend:HI (match_operand:QI 1 "pic30_wreg_operand"
						"%a,a"))
             (zero_extend:HI (match_operand:QI 2 "pic30_reg_or_near_operand"
						 "r,U"))))]
  ""
  "@
   mul.b %m2
   mul.b %2"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulqihi3imm"
  [(set (match_operand:HI 0 "pic30_creg_operand"
						"=c")
    (mult:HI (zero_extend:HI (match_operand:QI 1 "pic30_wreg_operand"
						"%a"))
             (match_operand:HI 2 "immediate_operand" "i")))]
; Ensure the operand fits in a byte
  "(((INTVAL(operands[2])) & 0xFF) == (INTVAL(operands[2])))"
  "mov.b #%2,%0\;mul.b %m0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulqihi3pow2"
  [(set (match_operand:HI 0 "pic30_register_operand"                      "=r")
    (mult:HI (zero_extend:HI (match_operand:QI 1 "pic30_register_operand" "%r"))
             (match_operand:HI 2 "immediate_operand"                 "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[2]))"
  "ze %1,%0\;sl %0,#%b2,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;; 16 x 16 => 16

;; this expansion is helpful in constant propagation
;; (but in general, it leads to code expansion, since
;; it causes all integer multiplication to be inline)

(define_expand "mulhi3"
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (mult:HI (match_operand:HI 1 "pic30_register_operand" "")
                 (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{ rtx temp_reg = 0, temp_opnd1 = 0, temp_opnd2 = 0;
  int valid_immediate = 0;

  if (immediate_operand(operands[2], VOIDmode)) {
    if (INTVAL(operands[2]) == 2) {
      emit_insn(gen_mulhi3imm(operands[0], operands[1], operands[2]));
      DONE;
    } else if (pic30_P_operand(operands[2],VOIDmode)) {
      valid_immediate = 1;
    } else {
      temp_opnd2  = gen_reg_rtx(HImode);

      emit_move_insn(temp_opnd2, operands[2]);
    }
  }
  if (pic30_ecore_target()) {
    if (valid_immediate) {
      emit_insn(
        gen_mulhi3imm(operands[0],
                        temp_opnd1 ? temp_opnd1 : operands[1],
                        temp_opnd2 ? temp_opnd2 : operands[2])
      );
    } else {
      emit_insn(
        gen_emulhi3(operands[0],
                    temp_opnd1 ? temp_opnd1 : operands[1],
                    temp_opnd2 ? temp_opnd2 : operands[2])
      );
    }
  } else {
    temp_reg = gen_reg_rtx(SImode);
    if (valid_immediate) {
      emit_insn(
        gen_mulhisi3_imm(temp_reg,
                         temp_opnd1 ? temp_opnd1 : operands[1],
                         temp_opnd2 ? temp_opnd2 : operands[2])
      );
    } else {
      emit_insn(
        gen_mulhisi3(temp_reg,
                     temp_opnd1 ? temp_opnd1 : operands[1],
                     temp_opnd2 ? temp_opnd2 : operands[2])
      );
    }
    convert_move(operands[0], temp_reg, 0);
  }
  DONE;
}")

(define_expand "mulp16apsv3"
  [(set (match_operand:P16APSV 0 "pic30_register_operand" "")
        (mult:P16APSV 
          (match_operand:P16APSV 1 "pic30_reg_or_near_operand" "")
          (match_operand:P16APSV 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{ rtx temp_reg = 0, temp_opnd1 = 0, temp_opnd2 = 0;
  int valid_immediate = 0;

  if (immediate_operand(operands[2], VOIDmode)) {
    if (INTVAL(operands[2]) == 2) {
      emit_insn(gen_mulp16apsvsi3_imm(operands[0], operands[1], operands[2]));
      DONE;
    } else if (pic30_P_operand(operands[2],VOIDmode)) {
      valid_immediate = 1;
    } else {
      temp_opnd2  = gen_reg_rtx(HImode);

      emit_move_insn(temp_opnd2, operands[2]);
    }
  }
  if (pic30_near_operand(operands[1], HImode)) {
    temp_opnd1 = gen_reg_rtx(HImode);
    
    emit_move_insn(temp_opnd1, operands[1]);
  }
  if (pic30_ecore_target()) {
    if (valid_immediate) {
      emit_insn(
        gen_emulp16apsv3_imm(operands[0],
                             temp_opnd1 ? temp_opnd1 : operands[1],
                             temp_opnd2 ? temp_opnd2 : operands[2])
      );
    } else {
      emit_insn(
        gen_emulp16apsv3(operands[0],
                         temp_opnd1 ? temp_opnd1 : operands[1],
                         temp_opnd2 ? temp_opnd2 : operands[2])
      );
    }
  } else {
    temp_reg = gen_reg_rtx(SImode);
    if (valid_immediate) {
      emit_insn(
        gen_mulp16apsvsi3_imm(temp_reg,
                              temp_opnd1 ? temp_opnd1 : operands[1],
                              temp_opnd2 ? temp_opnd2 : operands[2])
      );
    } else {
      emit_insn(
        gen_mulp16apsvsi3(temp_reg,
                          temp_opnd1 ? temp_opnd1 : operands[1],
                          temp_opnd2 ? temp_opnd2 : operands[2])
      );
    }
    convert_move(operands[0], temp_reg, 0);
  }
  DONE;
}")

(define_insn "mulhi3imm_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=a,r<>,r<>,R,R")
        (mult:HI 
            (match_operand:HI 1 "pic30_near_mode2_operand" "U,r,  R<>,r,R<>")
            (match_operand:HI 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2)"
  "@
     sl %1,WREG
     sl %1,%0
     sl %1,%0
     sl %1,%0
     sl %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,def,defuse,etc,use")
  ]
)

(define_insn "mulhi3imm_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=a,r<>,r<>,R,R")
        (mult:HI 
            (match_operand:HI 1 "pic30_near_mode2_APSV_operand" "U,r,R<>,r,R<>")
            (match_operand:HI 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2)"
  "@
     sl %1,WREG
     sl %1,%0
     sl %1,%0
     sl %1,%0
     sl %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,def,defuse,etc,use")
  ]
)

(define_expand "mulhi3imm"
  [(set (match_operand:HI 0 "pic30_mode2_operand" "=a,r<>,r<>,R,R")
        (mult:HI
            (match_operand:HI 1 "pic30_near_mode2_APSV_operand" "U,r,R<>,r,R<>")
            (match_operand:HI 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2)"
  "
{
  if (pic30_near_mode2_operand(operands[1],GET_MODE(operands[1])))
    emit(gen_mulhi3imm_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulhi3imm_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; 32-bit product
; /* *_extend of an immediate_operand is illegal, apparantly 
;    so need two separate sequences */

(define_insn "umulhisi3imm"
  [(set (match_operand:SI 0 "pic30_register_operand"                      "=r")
    (mult:SI (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "%r"))
             (match_operand:SI 2 "pic30_P_operand"                   "P")))]
  ""
  "mul.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulhisi3_DATA"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (zero_extend:SI 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI 
            (match_operand:HI 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulhisi3_APSV"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (zero_extend:SI 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI 
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "umulhisi3"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI
          (zero_extend:SI
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2], GET_MODE(operands[2]))) {
    emit(
      gen_umulhisi3_DATA(operands[0], operands[1], operands[2])
    );
  } else {
    emit(
      gen_umulhisi3_APSV(operands[0], operands[1], operands[2])
    );
  }
  DONE;
}")

(define_insn "mulp16apsv3imm_DATA"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=a,r<>,r<>,R,R")
        (mult:P16APSV 
            (match_operand:P16APSV 1 "pic30_near_mode2_operand" "U,r,  R<>,r,R<>")
            (match_operand:P16APSV 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2)"
  "@
     sl %1,WREG
     sl %1,%0
     sl %1,%0
     sl %1,%0
     sl %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,def,defuse,etc,use")
  ]
)

(define_insn "mulp16apsv3imm_APSV"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=a,r<>,r<>,R,R")
        (mult:P16APSV 
            (match_operand:P16APSV 1 "pic30_near_mode2_APSV_operand" "U,r,R<>,r,R<>")
            (match_operand:P16APSV 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2)"
  "@
     sl %1,WREG
     sl %1,%0
     sl %1,%0
     sl %1,%0
     sl %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,def,defuse,etc,use")
  ]
)

(define_expand "emulp16apsv3imm"
  [(set (match_operand:P16APSV 0 "pic30_mode2_operand" "=a,D<>,D<>,R,R")
        (mult:P16APSV
          (match_operand:P16APSV 1 "pic30_near_mode2_APSV_operand" "U,D,R<>,D,R<>")
          (match_operand:P16APSV 2 "immediate_operand" "i,i,  i,  i,i")))]
  "(INTVAL(operands[2]) == 2) && pic30_ecore_target()"
  "
{
  if (pic30_near_mode2_operand(operands[1],GET_MODE(operands[1])))
    emit(gen_mulp16apsv3imm_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulp16apsv3imm_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; 32-bit product
; /* *_extend of an immediate_operand is illegal, apparantly 
;    so need two separate sequences */

(define_insn "umulp16apsvsi3imm"
  [(set (match_operand:SI 0 "pic30_register_operand"                      "=r")
    (mult:SI (zero_extend:SI (match_operand:P16APSV 1 "pic30_register_operand" "%r"))
             (match_operand:SI 2 "pic30_P_operand"                   "P")))]
  ""
  "mul.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulp16apsvsi3_DATA"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (zero_extend:SI 
            (match_operand:P16APSV 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI 
            (match_operand:P16APSV 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulp16apsvsi3_APSV"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (zero_extend:SI 
            (match_operand:P16APSV 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI 
            (match_operand:P16APSV 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "umulp16apsvsi3"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI
          (zero_extend:SI
            (match_operand:P16APSV 1 "pic30_register_operand" "%r,r"))
          (zero_extend:SI
            (match_operand:P16APSV 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_umulp16apsvsi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_umulp16apsvsi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "*umulp16apsvsi3sfr"
  [(set (match_operand:SI     0 "pic30_creg_operand"        "=C,C")
        (mult:SI 
          (zero_extend:SI 
            (match_operand:P16APSV 1 "pic30_wreg_operand"        "%a,a"))
          (zero_extend:SI 
            (match_operand:P16APSV 2 "pic30_reg_or_near_operand" "r,U"))))]
  ""
  "@
   mul.w %m2
   mul.w %2"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;; 16 x 16 => 32 (signed)

;; /* *_extend of an immediate_operand is illegal, apparantly -
;;   so need separate patterns */

(define_insn "mulhisi3_imm"
  [(set (match_operand:SI 0 "pic30_register_operand"    "=r")
        (mult:SI 
          (sign_extend:SI (match_operand:HI 1 "pic30_register_operand" "%r"))
                          (match_operand:SI 2 "pic30_P_operand"        "P")))]
  ""
  "mul.su %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "mulhisi3_DATA"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (sign_extend:SI 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI 
            (match_operand:HI 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulhisi3_APSV"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (sign_extend:SI 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI 
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "mulhisi3"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI
          (sign_extend:SI
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulhisi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulhisi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "mulp16apsvsi3_imm"
  [(set (match_operand:SI 0 "pic30_register_operand"    "=r")
        (mult:SI 
          (sign_extend:SI (match_operand:P16APSV 1 "pic30_register_operand" "%r"))
                          (match_operand:SI 2 "pic30_P_operand"        "P")))]
  ""
  "mul.su %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "mulp16apsvsi3_DATA"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (sign_extend:SI 
            (match_operand 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI 
            (match_operand 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulp16apsvsi3_APSV"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI 
          (sign_extend:SI 
            (match_operand 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI 
            (match_operand 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "mulp16apsvsi3"
  [(set (match_operand:SI     0 "pic30_register_operand" "=r,r")
        (mult:SI
          (sign_extend:SI
            (match_operand:P16APSV 1 "pic30_register_operand" "%r,r"))
          (sign_extend:SI
            (match_operand:P16APSV 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulp16apsvsi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulp16apsvsi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")


(define_insn "umulhip32eds3_imm"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "=r")
    (mult:P32EDS 
      (zero_extend:P32EDS 
        (match_operand:HI     1 "pic30_register_operand" "%r"))
      (match_operand:P32EDS   2 "pic30_P_operand"                   "P")))]
  ""
  "mul.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulhip32eds3_DATA"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "=r,r")
        (mult:P32EDS 
          (zero_extend:P32EDS 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32EDS 
            (match_operand:HI 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulhip32eds3_APSV"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "=r,r")
        (mult:P32EDS 
          (zero_extend:P32EDS 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32EDS 
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "umulhip32eds3"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "=r,r")
        (mult:P32EDS
          (zero_extend:P32EDS
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32EDS
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_umulhip32eds3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_umulhip32eds3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "umulhip32peds3_imm"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand" "=r")
    (mult:P32PEDS 
      (zero_extend:P32PEDS 
        (match_operand:HI     1 "pic30_register_operand" "%r"))
      (match_operand:P32PEDS   2 "pic30_P_operand"                   "P")))]
  ""
  "mul.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulhip32peds3_DATA"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand" "=r,r")
        (mult:P32PEDS 
          (zero_extend:P32PEDS 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32PEDS 
            (match_operand:HI 2 "pic30_mode2_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulhip32peds3_APSV"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand" "=r,r")
        (mult:P32PEDS 
          (zero_extend:P32PEDS 
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32PEDS 
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "mul.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "umulhip32peds3"
  [(set (match_operand:P32PEDS 0 "pic30_register_operand" "=r,r")
        (mult:P32PEDS
          (zero_extend:P32PEDS
            (match_operand:HI 1 "pic30_register_operand" "%r,r"))
          (zero_extend:P32PEDS
            (match_operand:HI 2 "pic30_mode2_APSV_operand"    "r,R<>"))))]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_umulhip32peds3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_umulhip32peds3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; ECORE
; /* *_extend of an immediate_operand is illegal, apparently
;    so need two separate sequences */

(define_insn "umulhi3imm"
  [(set (match_operand:HI 0 "pic30_D_register_operand"                    "=D")
        (mult:HI (match_operand:HI 1 "pic30_register_operand"           "%r")
                 (match_operand:HI 2 "pic30_P_operand"                   "P")))]
  "pic30_ecore_target()"
  "mulw.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulhi3_DATA"
  [(set (match_operand:HI     0 "pic30_D_register_operand" "=D,D")
        (mult:HI
            (match_operand:HI 1 "pic30_register_operand" "%r,r")
            (match_operand:HI 2 "pic30_mode2_operand"      "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulhi3_APSV"
  [(set (match_operand:HI     0 "pic30_D_register_operand"   "=D,D")
        (mult:HI
            (match_operand:HI 1 "pic30_register_operand"   "%r,r")
            (match_operand:HI 2 "pic30_mode2_APSV_operand"   "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulp16apsv3imm"
  [(set (match_operand:P16APSV 0 "pic30_D_register_operand"              "=D")
        (mult:P16APSV (match_operand:P16APSV 1 "pic30_register_operand" "%r")
                      (match_operand:P16APSV 2 "pic30_P_operand"         "P")))]
  "pic30_ecore_target()"
  "mulw.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "umulp16apsv3_DATA"
  [(set (match_operand:P16APSV     0 "pic30_D_register_operand" "=D,D")
        (mult:P16APSV
            (match_operand:P16APSV 1 "pic30_register_operand" "%r,r")
            (match_operand:P16APSV 2 "pic30_mode2_operand"    "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "umulp16apsv3_APSV"
  [(set (match_operand:P16APSV     0 "pic30_D_register_operand"   "=D,D")
        (mult:P16APSV
            (match_operand:P16APSV 1 "pic30_register_operand"   "%r,r")
            (match_operand:P16APSV 2 "pic30_mode2_APSV_operand" "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.uu  %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "umulp16apsv3"
  [(set (match_operand:P16APSV     0 "pic30_D_register_operand"   "=D,D")
        (mult:P16APSV
            (match_operand:P16APSV 1 "pic30_register_operand"   "%r,r")
            (match_operand:P16APSV 2 "pic30_mode2_APSV_operand" "r,R<>")))]
  "pic30_ecore_target()"
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_umulp16apsv3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_umulp16apsv3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "*umulp16apsv3sfr"
  [(set (match_operand:P16APSV     0 "pic30_creg_operand"        "=C,C")
        (mult:P16APSV
            (match_operand:P16APSV 1 "pic30_wreg_operand"        "%a,a")
            (match_operand:P16APSV 2 "pic30_reg_or_near_operand" "r,U")))]
  "pic30_ecore_target()"
  "@
   mulw.w %m2
   mulw.w %2"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;; 16 x 16 => 16 (signed)

;(define_insn "emulhi3_imm"
;  [(set (match_operand:HI 0 "pic30_D_register_operand"   "=D")
;        (mult:HI
;          (match_operand:HI 1 "pic30_register_operand" "%r")
;          (match_operand:HI 2 "pic30_P_operand"        "P")))]
;  "pic30_ecore_target()"
;  "mulw.su %1,#%2,%0"
;  [
;   (set_attr "cc" "change0")
;   (set_attr "type" "def")
;  ]
;)

(define_insn "mulhi3_DATA"
  [(set (match_operand:HI     0 "pic30_D_register_operand" "=D,D")
        (mult:HI
            (match_operand:HI 1 "pic30_register_operand" "%r,r")
            (match_operand:HI 2 "pic30_mode2_operand"    "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulhi3_APSV"
  [(set (match_operand:HI     0 "pic30_D_register_operand"   "=D,D")
        (mult:HI
            (match_operand:HI 1 "pic30_register_operand"   "%r,r")
            (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "emulhi3"
  [(set (match_operand:HI     0 "pic30_D_register_operand"   "=D,D")
        (mult:HI
            (match_operand:HI 1 "pic30_register_operand"   "%r,r")
            (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")))]
  "pic30_ecore_target()"
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulhi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulhi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "emulp16apsv3_imm"
  [(set (match_operand:P16APSV 0 "pic30_D_register_operand"    "=D")
        (mult:P16APSV
          (match_operand:P16APSV 1 "pic30_register_operand" "%r")
          (match_operand:P16APSV 2 "pic30_P_operand"        "P")))]
  "pic30_ecore_target()"
  "mulw.su %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

(define_insn "mulp16apsv3_DATA"
  [(set (match_operand:P16APSV     0 "pic30_D_register_operand" "=D,D")
        (mult:P16APSV
          (match_operand:P16APSV 1 "pic30_register_operand" "%r,r")
          (match_operand:P16APSV 2 "pic30_mode2_operand"    "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)
(define_insn "mulp16apsv3_APSV"
  [(set (match_operand:P16APSV   0 "pic30_D_register_operand" "=D,D")
        (mult:P16APSV
          (match_operand:P16APSV 1 "pic30_register_operand" "%r,r")
          (match_operand:P16APSV 2 "pic30_mode2_APSV_operand" "r,R<>")))]
  "pic30_ecore_target()"
  "mulw.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "emulp16apsv3"
  [(set (match_operand:P16APSV     0 "pic30_D_register_operand" "=D,D")
        (mult:P16APSV
          (match_operand:P16APSV 1 "pic30_register_operand" "%r,r")
          (match_operand:P16APSV 2 "pic30_mode2_APSV_operand"    "r,R<>")))]
  "pic30_ecore_target()"
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulp16apsv3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulp16apsv3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; builtin 16x16->32 instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "mulsu_DATA"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r,  r,r")
        (unspec:SI [
                    (match_operand:HI 1 "pic30_mode2_or_P_operand" "r ,r,  r,P")
                    (match_operand:HI 2 "pic30_mode2_or_P_operand" "r ,R<>,P,r")
                   ]
                    UNSPECV_MULSU))
  ]
  ""
  "@
   mul.su %1,%2,%0
   mul.su %1,%2,%0
   mul.su %1,#%2,%0
   mul.uu %2,#%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "mulsu_APSV"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r,  r")
        (unspec:SI [
	  (match_operand:HI 1 "pic30_register_operand"   "r,r,  r")
	  (match_operand:HI 2 "pic30_mode2_or_P_APSV_operand" "r,R<>,P")
	] UNSPECV_MULSU))
  ]
  ""
  "@
   mul.su %1,%2,%0
   mul.su %1,%2,%0
   mul.su %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def")
  ]
)

(define_expand "mulsu"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r,r,  r,r")
        (unspec:SI [
          (match_operand:HI 1 "pic30_mode2_or_P_APSV_operand" "r ,r,  r,P")
          (match_operand:HI 2 "pic30_mode2_or_P_APSV_operand" "r ,R<>,P,r")
        ] UNSPECV_MULSU))
  ]
  ""
  "
{
  if (pic30_mode2_or_P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulsu_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulsu_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "mulsu_acc"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w,w,w,w")
        (subreg:HI (unspec:SI [
                    (match_operand:HI 1 "pic30_mode2_or_P_operand" "r,r,  r,P")
                    (match_operand:HI 2 "pic30_mode2_or_P_operand" "r,R<>,P,r")
                   ]
                    UNSPECV_MULSU) 0
         ))
  ]
  ""
  "@
   mul.su %1,%2,%0
   mul.su %1,%2,%0
   mul.su %1,#%2,%0
   mul.uu %2,#%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "muluu_DATA"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r,  r,r")
        (unspec:SI [
                    (match_operand:HI 1 "pic30_mode2_or_P_operand" "r ,r,  r,P")
                    (match_operand:HI 2 "pic30_mode2_or_P_operand" "r ,R<>,P,r")
                   ]
                    UNSPECV_MULUU))
  ]
  ""
  "@
   mul.uu %1,%2,%0
   mul.uu %1,%2,%0
   mul.uu %1,#%2,%0
   mul.uu %2,#%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "muluu_APSV"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r,  r")
        (unspec:SI [
	  (match_operand:HI 1 "pic30_register_operand"   "r,r,  r")
	  (match_operand:HI 2 "pic30_mode2_or_P_APSV_operand" "r,R<>,P")
	] UNSPECV_MULUU))
  ]
  ""
  "@
   mul.uu %1,%2,%0
   mul.uu %1,%2,%0
   mul.uu %1,#%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def")
  ]
)

(define_expand "muluu"
  [(set (match_operand:SI 0 "pic30_register_operand"          "=r,r,  r,r")
        (unspec:SI [
          (match_operand:HI 1 "pic30_mode2_or_P_APSV_operand" "r ,r,  r,P")
          (match_operand:HI 2 "pic30_mode2_or_P_APSV_operand" "r ,R<>,P,r")
        ] UNSPECV_MULUU))
  ]
  ""
  "
{
  if (pic30_mode2_or_P_APSV_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_muluu_APSV(operands[0],operands[1],operands[2]));
  else
    emit(gen_muluu_DATA(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "muluu_acc"
  [(set (match_operand:HI 0 "pic30_accumulator_operand"  "=w,w,w,w")
        (subreg:HI (unspec:SI [
                    (match_operand:HI 1 "pic30_mode2_or_P_operand" "r,r,  r,P")
                    (match_operand:HI 2 "pic30_mode2_or_P_operand" "r,R<>,P,r")
                   ]
                    UNSPECV_MULUU) 0
        ))
  ]
  ""
  "@
   mul.uu %1,%2,%0
   mul.uu %1,%2,%0
   mul.uu %1,#%2,%0
   mul.uu %2,#%1,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse,def,def")
  ]
)

(define_insn "mulus_DATA"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
	            (match_operand:HI 1 "pic30_register_operand"    "r,r")
	            (match_operand:HI 2 "pic30_mode2_operand" "r,R<>")
	           ]
		    UNSPECV_MULUS))
  ]
  ""
  "mul.us %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulus_APSV"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
	  (match_operand:HI 1 "pic30_register_operand"    "r,r")
	  (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")
	] UNSPECV_MULUS))
  ]
  ""
  "mul.us %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "mulus"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
          (match_operand:HI 1 "pic30_register_operand"    "r,r")    
          (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")
        ] UNSPECV_MULUS))
  ]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulus_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulus_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "mulus_acc"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w,w")
        (subreg:HI (unspec:SI [
                    (match_operand:HI 1 "pic30_register_operand"    "r,r")
                    (match_operand:HI 2 "pic30_mode2_operand" "r,R<>")
                   ]
                    UNSPECV_MULUS) 0
                  ))
  ]
  ""
  "mul.us %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulss_DATA"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
	            (match_operand:HI 1 "pic30_register_operand"    "r,r")
	            (match_operand:HI 2 "pic30_mode2_operand" "r,R<>")
	           ]
		    UNSPECV_MULSS))
  ]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "mulss_APSV"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
	  (match_operand:HI 1 "pic30_register_operand"    "r,r")
	  (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")
	] UNSPECV_MULSS))
  ]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

(define_expand "mulss"
  [(set (match_operand:SI 0 "pic30_register_operand"               "=r,r")
        (unspec:SI [
          (match_operand:HI 1 "pic30_register_operand"    "r,r")    
          (match_operand:HI 2 "pic30_mode2_APSV_operand" "r,R<>")
        ] UNSPECV_MULSS))
  ]
  ""
  "
{
  if (pic30_mode2_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_mulss_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_mulss_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "mulss_acc"
  [(set (match_operand:HI 0 "pic30_accumulator_operand" "=w,w")
        (subreg:HI  (unspec:SI [
            (match_operand:HI 1 "pic30_register_operand" "r,r")
            (match_operand:HI 2 "pic30_mode2_operand" "r,R<>")
          ] UNSPECV_MULSS) 0
         ))
  ]
  ""
  "mul.ss %1,%2,%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def,defuse")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; 32 x 32 => 32 (signed / unsigned) ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;
;;  For e-core machine we don't want to use the normal mechanism because
;;    the normal mulhi wants an even register (which is better for)
;;    16-bit mul but not for composing 32-bit muls
;;

(define_expand "mulsi3"
  [(set (match_operand:SI 0 "pic30_register_operand" "")
        (mult:SI (match_operand:SI 1 "pic30_register_operand" "")
                 (match_operand:SI 2 "pic30_reg_or_imm_operand" "")))]
  "pic30_ecore_target() && 0"
  "
{
   int intval = 0;
   rtx (*mulhisi)(rtx, rtx, rtx);
   rtx (*umulhisi)(rtx, rtx, rtx);

   rtx high_2 = 0, low_2 =0;
   rtx high_1, low_1;
   rtx partial;

   /*                  */
   /*           a b    */
   /*x          c d    */
   /*--------------    */
   /*       ext d.b    */
   /*+  xxx d.a        */
   /*+  xxx c.b        */
   /*                  */

   low_1 = gen_reg_rtx(HImode);
   high_1 = gen_reg_rtx(HImode);
   low_2 = gen_reg_rtx(HImode);
   high_2 = gen_reg_rtx(HImode);
   intval = immediate_operand(operands[2], VOIDmode);
   if (intval) {
     rtx temp;

     intval = pic30_P_operand(operands[2], VOIDmode);
     temp = GEN_INT((INTVAL(operands[2])) & 0xFFFF);
     if (!intval) {
       emit_move_insn(low_2, temp);
       temp = GEN_INT((INTVAL(operands[2]) >> 16) & 0xFFFF);
       emit_move_insn(high_2, temp);
     } else low_2 = temp;
   } else {
     emit_move_insn(low_2,gen_rtx_SUBREG(HImode,operands[2], 0));
     emit_move_insn(high_2,gen_rtx_SUBREG(HImode,operands[2], 2));
   }
   emit_move_insn(low_1,gen_rtx_SUBREG(HImode,operands[1], 0));
   emit_move_insn(high_1,gen_rtx_SUBREG(HImode,operands[1], 2));
   if (intval) {
     mulhisi = gen_mulhisi3_imm;
     umulhisi = gen_umulhisi3imm;
   } else {
     mulhisi = gen_mulhisi3;
     umulhisi = gen_umulhisi3;
   }

   partial = gen_reg_rtx(SImode);

   emit(
     /* d.a */
     mulhisi(partial,
             high_1,
             low_2)
   );
   if (!intval) {
     emit(
       /* c.b */
       mulhisi(operands[0],
               low_1,
               high_2)
     );
     emit (
       gen_addhi3(gen_rtx_SUBREG(HImode,partial,0),
                  gen_rtx_SUBREG(HImode,operands[0],0),
                  gen_rtx_SUBREG(HImode,partial,0))
     );
   }
   emit(
     /* d.b */
     umulhisi(operands[0],
              low_1,
              low_2)
   );
   emit (
     gen_addhi3(gen_rtx_SUBREG(HImode, operands[0], 2),
                gen_rtx_SUBREG(HImode,operands[0],2),
                gen_rtx_SUBREG(HImode,partial,0))
   );
   DONE;
}")

(define_expand "umulsi3"
  [(set (match_operand:SI 0 "pic30_register_operand" "")
        (mult:SI (match_operand:SI 1 "pic30_register_operand" "")
                 (match_operand:SI 2 "pic30_reg_or_imm_operand" "")))]
  "pic30_ecore_target()"
  "
{  rtx low_2, low_1, high_2, high_1, temp_1, temp_2;

   /*          */
   /*      a b */
   /*x     c d */
   /*--------- */
   /*  d.a d.b */
   /*  c.b     */
   /*          */

   low_1 = gen_rtx_SUBREG(HImode, operands[1], 0);
   low_2 = gen_rtx_SUBREG(HImode, operands[2], 0);
   high_1 = gen_rtx_SUBREG(HImode, operands[1], 2);
   high_2 = gen_rtx_SUBREG(HImode, operands[2], 2);
   temp_1 = gen_reg_rtx(SImode);
   emit(
     gen_umulhisi3(temp_1, low_1, high_2)        /* c.b */
   );
   emit(
     gen_umulhisi3(operands[0], high_1, low_2)   /* d.a */
   );
   emit (
     gen_addhi3(temp_1, gen_rtx_SUBREG(HImode,operands[0],0), temp_1)
   );
   emit(
     gen_umulhisi3(operands[0], low_1, low_2)   /* d.b */
   );
   emit (
     gen_addhi3(temp_1, temp_1, gen_rtx_SUBREG(HImode,operands[0],2))
   );
  emit (
     gen_movhi(gen_rtx_SUBREG(HImode,operands[0],2), temp_1)
  );
  DONE;
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; divide instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; builtin 32/16 instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "divsd"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=a,?b,??e")
        (unspec_volatile:HI [
	            (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
	            (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
	           ]
		    UNSPECV_DIVSD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 0:  /*
              ** wm/wn -> w0
              */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "div.sd %1,%2\;mov [--w15],w1");
              }
       case 1:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w0,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.sd %1,%2\;"
                       "mov w0,%0\;mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w0,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.sd %1,%2\;mov w0,%0\;mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have a new CC of div
    (set_attr "type" "def")
  ]
)

(define_insn "modsd"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=b,?a,??e")
        (unspec_volatile:HI [
                    (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
                    (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
                   ]
                    UNSPECV_MODSD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 1:  /*
                ** wm/wn -> w0
                */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,%0");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "div.sd %1,%2\;mov w1,%0\;mov [--w15],w1");
                }
       case 0:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.sd %1,%2\;"
                         "mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.sd %1,%2\;mov w1,%0\;mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have a new CC of div
    (set_attr "type" "def")
  ]
)

(define_insn "divf"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=a,?b,??e")
        (unspec_volatile:HI [
                    (match_operand:HI 1 "pic30_register_operand"   "r, r,  r")
                    (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
                   ] UNSPECV_DIVF))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 0:  /*
              ** wm/wn -> w0
              */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;divf %1,%2");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "divf %1,%2\;mov [--w15],w1");
              }
       case 1:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;divf %1,%2\;mov w0,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;divf %1,%2\;"
                       "mov w0,%0\;mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;divf %1,%2\;mov w0,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "divf %1,%2\;mov w0,%0\;mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have new cc of div
    (set_attr "type" "def")
  ]
)

(define_insn "divud"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=a,?b,??e")
        (unspec_volatile:HI [
	            (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
	            (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
	           ]
		    UNSPECV_DIVUD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 0:  /*
              ** wm/wn -> w0
              */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "div.ud %1,%2\;mov [--w15],w1");
              }
       case 1:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w0,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.ud %1,%2\;"
                       "mov w0,%0\;mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w0,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.ud %1,%2\;mov w0,%0\;mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have new CC of div
    (set_attr "type" "def")
  ]
)

(define_insn "modud"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=b,?a,??e")
        (unspec_volatile:HI [
                    (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
                    (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
                   ]
                    UNSPECV_MODUD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 1:  /*
                ** wm/wn -> w0
                */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w1,%0");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "div.ud %1,%2\;mov w1,%0\;mov [--w15],w1");
                }
       case 0:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w1,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.ud %1,%2\;"
                         "mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w1,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.ud %1,%2\;mov w1,%0\;mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have new CC of div
    (set_attr "type" "def")
  ]
)

(define_insn "divmodsd"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=a,?b,??e")
        (unspec_volatile:HI [
                  (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
                  (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
                 ] UNSPECV_DIVMODSD))
   (set (mem:HI (match_operand:HI 3 "pic30_register_operand" "e,e,e"))
        (unspec_volatile:HI [
                  (match_dup 1)
                  (match_dup 2)
                 ] UNSPECV_DIVMODSD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 0:  /*
                ** wm/wn -> w0
                */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,[%3]");
                } else {
                  return("mov w1,[w15++]\;repeat #18-1\;"
                         "div.sd %1,%2\;mov w1,[%3]\;mov [--w15],w1");
                }
       case 1:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,[%3]\;mov w0,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.sd %1,%2\;"
                         "mov w1,[%3]\;mov w0,%0\;mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.sd %1,%2\;mov w1,[%3]\;mov w0,%0");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.sd %1,%2\;mov w1,[%3]\;mov w0,%0\;"
                         "mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")   ; should have new CC of div
    (set_attr "type" "def")
  ]
)

(define_insn "divmodud"
  [(set (match_operand:HI 0 "pic30_register_operand"              "=a,?b,??e")
        (unspec_volatile:HI [
                  (match_operand:SI 1 "pic30_register_operand"   "r, r,  r")
                  (match_operand:HI 2 "pic30_ereg_operand" "e, e,  e")
                 ] UNSPECV_DIVMODUD))
   (set (mem:HI (match_operand:HI 3 "pic30_register_operand" "e,e,e"))
        (unspec_volatile:HI [
                  (match_dup 1)
                  (match_dup 2)
                 ] UNSPECV_DIVMODUD))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  {
     rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
     rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
     switch (which_alternative) {
       case 0:  /*
                ** wm/wn -> w0
                */
                if (pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w1,[%3]");
                } else {
                    return("mov w1,[w15++]\;repeat #18-1\;"
                           "div.ud %1,%2\;mov w1,[%3]\;mov [--w15],w1");
                }
       case 1:  /*
                ** wm/wn -> w1
                */
                if (pic30_dead_or_set_p(insn, w0)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w1,[%3]\;mov w0,%0");
                } else {
                  return("mov w0,[w15++]\;repeat #18-1\;div.ud %1,%2\;"
                         "mov w1,[%3]\;mov w0,%0\;mov [--w15],w0");
                }
       default: /*
                ** wm/wn -> we
                */
                if (pic30_dead_or_set_p(insn, w0) &&
                    pic30_dead_or_set_p(insn, w1)) {
                  return("repeat #18-1\;div.ud %1,%2\;mov w0,%0\;mov w1,[%3]");
                } else {
                  return("mov.d w0,[w15++]\;repeat #18-1\;"
                         "div.ud %1,%2\;mov w0,%0\;mov w1,[%3]\;"
                         "mov.d [--w15],w0");
                }
     }
  }
  [
    (set_attr "cc" "clobber")  ; should have new CC of div
    (set_attr "type" "def")
  ]
)

;; Divide/Remainder instructions.
(define_insn "divmodhi4"
  [(set (match_operand:HI 0 "pic30_register_operand"             "=a,b,r")
      (div:HI (match_operand:HI 1 "pic30_register_operand"       "r,r,r")
              (match_operand:HI 2 "pic30_ereg_operand"     "e,e,e")))
   (set (match_operand:HI 3 "pic30_register_operand"             "=b,a,r")
      (mod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
   int rquo = REGNO(operands[0]);
   int rnum = REGNO(operands[1]);
   int rden = REGNO(operands[2]);
   int rrem = REGNO(operands[3]);
   static char instr_sequence[120];
   char *f = instr_sequence;
   rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
   struct fred {
     unsigned int q:1;
     unsigned int r:1;
     unsigned int p_w0:1;
     unsigned int p_w1:1;
     int e_w0;
     int e_w1;
   } status = { 0 };

   switch (which_alternative) {
   case 2:
      /*
      ** wm/wn -> wq, wr
      */ 
      status.e_w0 = -1;
      status.e_w1 = -1;
      status.p_w0 = 1;
      status.p_w1 = 1;
      if (dead_or_set_p(insn,w0) ||
          pic30_dead_or_set_p(NEXT_INSN(insn), w0)) {
        /* preserve w1 */
        status.p_w0 = 0;
      } 
      if (dead_or_set_p(insn,w1) ||
          pic30_dead_or_set_p(NEXT_INSN(insn), w1)) {
        status.p_w1 = 0;
      } 
      if (find_reg_note(insn, REG_UNUSED, operands[3]))
      {
         status.q = 1;
      }
      else if (find_reg_note(insn, REG_UNUSED, operands[0]))
      {
         status.r = 1;
      }
      else
      {
         status.q = 1;
         status.r = 1;
      }
      if (status.p_w0 || status.p_w1) {
         /* is there an exchange register ? */
        if ((status.p_w0) && !(pic30_errata_mask & exch_errata)) {
          if ((rquo!=WR0_REGNO) && (rquo!=WR1_REGNO) && /* clobber by div */
              (rquo!=rnum) && (rquo!=rden) && status.q)          /* input */
             status.e_w0 = rquo;
        }
        if ((status.p_w1) && !(pic30_errata_mask & exch_errata)) {
          if ((rrem!=WR0_REGNO) && (rrem!=WR1_REGNO) && /* clobber by div */
              (rrem!=rnum) && (rrem!=rden) && status.r)          /* input */
             status.e_w1 = rrem;
        }
        if ((status.e_w0 == -1) && (status.e_w1 == -1) && 
            (status.p_w0) && (status.p_w1)) {
          f += sprintf(f, \"mov.d w0,[w15++]\;\");
        } else {
          if (status.p_w0) {
            if (status.e_w0 > 0) 
               f += sprintf(f, \"mov w0,%s\;\", reg_names[status.e_w0]);
            else
               f += sprintf(f, \"mov w0,[w15++]\;\");
          }
          if (status.p_w1) {
            if (status.e_w1 > 0) 
               f += sprintf(f, \"mov w1,%s\;\", reg_names[status.e_w1]);
            else
               f += sprintf(f, \"mov w1,[w15++]\;\");
          }
        }
      }
      f += sprintf(f, \"repeat #18-1\;div.sw %%1,%%2\");
      while (status.q || status.r) {
        if (status.q) {
          if (rquo != WR0_REGNO) {
            if (status.e_w0 > 0) {
              f += sprintf(f, \"\;exch w0,%s\", reg_names[status.e_w0]);
              status.q = 0;
            } else if ((rquo != WR1_REGNO) || (!status.r)) {
              f += sprintf(f, \"\;mov w0,%%0\");
              status.q = 0;
            }
          } else status.q = 0;
        }
        if (status.r) {
          if (rrem != WR1_REGNO) {
            if (status.e_w1 > 0) {
              f += sprintf(f, \"\;exch w1,%s\", reg_names[status.e_w1]);
              status.r = 0;
            } else if ((rrem != WR0_REGNO) || (!status.q)) {
              f += sprintf(f, \"\;mov w1,%%3\");
              status.r = 0;
            }
          } else status.r = 0;
        }
        if ((status.r) && (status.q)) {
          if ((rquo == WR1_REGNO) && (rrem == WR0_REGNO)) {
            if (pic30_errata_mask & exch_errata)
              f += sprintf(f, \"\;push w0\;mov w1,w0\;pop w1\");
            else
              f += sprintf(f, \"\;exch w0,w1\");
            status.r = 0;
            status.q = 0;
          } else {
            internal_error(\"deadlock\");
          }
        }
      }
      if (status.p_w0 && status.p_w1 && (status.e_w0 == -1) && 
          (status.e_w1 == -1)) {
        f += sprintf(f, \"\;mov.d [--w15],w0\");
      } else {
        if (status.p_w1 && (status.e_w1 == -1)) {
          f += sprintf(f, \"\;mov [--w15],w1\");
        }
        if (status.p_w0 && (status.e_w0 == -1)) {
          f += sprintf(f, \"\;mov [--w15],w0\");
        }
      }
      return instr_sequence;
      break;
    case 0:
      return   (
       \"repeat #18-1\;\"
       \"div.sw %1,%2\"
      );
      break;
    case 1:
     if (pic30_errata_mask & exch_errata)
       return \"repeat #18-1\;\"
              \"div.sw %1,%2\;\"
              \"push w0\;\"
              \"mov w1,w0\;\"
              \"pop w1\;\";
      else
        return \"repeat #18-1\;\"
               \"div.sw %1,%2\;\"
               \"exch w0,w1\";
      break;
   }
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

(define_insn "divmodhi4_w0"
  [(set (match_operand:HI 0 "pic30_wreg_operand"           "=a")
      (div:HI (match_operand:HI 1 "pic30_register_operand"       "r")
              (match_operand:HI 2 "pic30_ereg_operand"     "e")))
   (set (match_operand:HI 3 "pic30_breg_operand"           "=b")
      (mod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
  return   (
         \"repeat #18-1\;\"
         \"div.sw %1,%2\"
         );
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

(define_insn "divmodhi4_w1"
  [(set (match_operand:HI 0 "pic30_breg_operand"           "=b")
      (div:HI (match_operand:HI 1 "pic30_register_operand"       "r")
              (match_operand:HI 2 "pic30_ereg_operand"     "e")))
   (set (match_operand:HI 3 "pic30_wreg_operand"           "=a")
      (mod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
  if (pic30_errata_mask & exch_errata)
    return \"repeat #18-1\;\"
           \"div.sw %1,%2\;\"
           \"push w0\;\"
           \"mov w1,w0\;\"
           \"pop w1\;\";
  else
    return  \"repeat #18-1\;\"
            \"div.sw %1,%2\"
            \"exch w0,w1\";
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

(define_insn "udivmodhi4"
  [(set (match_operand:HI 0 "pic30_register_operand"             "=a,b,r")
      (udiv:HI (match_operand:HI 1 "pic30_register_operand"      " r,r,r")
               (match_operand:HI 2 "pic30_ereg_operand"          " e,e,e")))
   (set (match_operand:HI 3 "pic30_register_operand"             "=b,a,r")
      (umod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
   int rquo = REGNO(operands[0]);
   int rnum = REGNO(operands[1]);
   int rden = REGNO(operands[2]);
   int rrem = REGNO(operands[3]);
   static char instr_sequence[120];
   char *f = instr_sequence;
   rtx w0 = gen_rtx_REG(HImode, WR0_REGNO);
   rtx w1 = gen_rtx_REG(HImode, WR1_REGNO);
   struct fred {
     unsigned int q:1;
     unsigned int r:1;
     unsigned int p_w0:1;
     unsigned int p_w1:1;
     int e_w0;
     int e_w1;
   } status = { 0 };

   switch (which_alternative) {
   case 2:
      /*
      ** wm/wn -> wq, wr
      */ 
      status.e_w0 = -1;
      status.e_w1 = -1;
      status.p_w0 = 1;
      status.p_w1 = 1;
      if (dead_or_set_p(insn,w0) ||
          pic30_dead_or_set_p(NEXT_INSN(insn), w0)) {
        /* preserve w1 */
        status.p_w0 = 0;
      } 
      if (dead_or_set_p(insn,w1) ||
          pic30_dead_or_set_p(NEXT_INSN(insn), w1)) {
        status.p_w1 = 0;
      } 
      if (find_reg_note(insn, REG_UNUSED, operands[3]))
      {
         status.q = 1;
      }
      else if (find_reg_note(insn, REG_UNUSED, operands[0]))
      {
         status.r = 1;
      }
      else
      {
         status.q = 1;
         status.r = 1;
      }
      if (status.p_w0 || status.p_w1) {
         /* is there an exchange register ? */
        if ((status.p_w0) && !(pic30_errata_mask & exch_errata)) {
          if ((rquo!=WR0_REGNO) && (rquo!=WR1_REGNO) && /* clobber by div */
              (rquo!=rnum) && (rquo!=rden) && status.q)          /* input */
             status.e_w0 = rquo;
        }
        if ((status.p_w1) && !(pic30_errata_mask & exch_errata)) {
          if ((rrem!=WR0_REGNO) && (rrem!=WR1_REGNO) && /* clobber by div */
              (rrem!=rnum) && (rrem!=rden) && status.r)          /* input */
             status.e_w1 = rrem;
        }
        if ((status.e_w0 == -1) && (status.e_w1 == -1) && 
            (status.p_w0) && (status.p_w1)) {
          f += sprintf(f, \"mov.d w0,[w15++]\;\");
        } else {
          if (status.p_w0) {
            if (status.e_w0 > 0) 
               f += sprintf(f, \"mov w0,%s\;\", reg_names[status.e_w0]);
            else
               f += sprintf(f, \"mov w0,[w15++]\;\");
          }
          if (status.p_w1) {
            if (status.e_w1 > 0) 
               f += sprintf(f, \"mov w1,%s\;\", reg_names[status.e_w1]);
            else
               f += sprintf(f, \"mov w1,[w15++]\;\");
          }
        }
      }
      f += sprintf(f, \"repeat #18-1\;div.uw %%1,%%2\");
      while (status.q || status.r) {
        if (status.q) {
          if (rquo != WR0_REGNO) {
            if (status.e_w0 > 0) {
              f += sprintf(f, \"\;exch w0,%s\", reg_names[status.e_w0]);
              status.q = 0;
            } else if ((rquo != WR1_REGNO) || (!status.r)) {
              f += sprintf(f, \"\;mov w0,%%0\");
              status.q = 0;
            }
          } else status.q = 0;
        }
        if (status.r) {
          if (rrem != WR1_REGNO) {
            if (status.e_w1 > 0) {
              f += sprintf(f, \"\;exch w1,%s\", reg_names[status.e_w1]);
              status.r = 0;
            } else if ((rrem != WR0_REGNO) || (!status.q)) {
              f += sprintf(f, \"\;mov w1,%%3\");
              status.r = 0;
            }
          } else status.r = 0;
        }
        if ((status.r) && (status.q)) {
          if ((rquo == WR1_REGNO) && (rrem == WR0_REGNO)) {
            if (pic30_errata_mask & exch_errata)
              f += sprintf(f, \"\;push w0\;mov w1,w0\;pop w1\");
            else
              f += sprintf(f, \"\;exch w0,w1\");
            status.r = 0;
            status.q = 0;
          } else {
            internal_error(\"deadlock\");
          }
        }
      }
      if (status.p_w0 && status.p_w1 && (status.e_w0 == -1) && 
          (status.e_w1 == -1)) {
        f += sprintf(f, \"\;mov.d [--w15],w0\");
      } else {
        if (status.p_w1 && (status.e_w1 == -1)) {
          f += sprintf(f, \"\;mov [--w15],w1\");
        }
        if (status.p_w0 && (status.e_w0 == -1)) {
          f += sprintf(f, \"\;mov [--w15],w0\");
        }
      }
      return instr_sequence;
      break;
    case 0:
      return   (
       \"repeat #18-1\;\"
       \"div.uw %1,%2\"
      );
      break;
    case 1:
      if (pic30_errata_mask & exch_errata)
        return \"repeat #18-1\;\"
               \"div.uw %1,%2\;\"
               \"push w0\;\"
               \"mov w1,w0\;\"
               \"pop w1\;\";
      else
        return \"repeat #18-1\;\"
               \"div.uw %1,%2\;\"
               \"exch w0,w1\";
      break;
   }
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

(define_insn "udivmodhi4_w0"
  [(set (match_operand:HI 0 "pic30_wreg_operand"           "=a")
      (div:HI (match_operand:HI 1 "pic30_register_operand"       "r")
              (match_operand:HI 2 "pic30_ereg_operand"     "e")))
   (set (match_operand:HI 3 "pic30_breg_operand"           "=b")
      (mod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
  return   (
         \"repeat #18-1\;\"
         \"div.uw %1,%2\"
         );
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

(define_insn "udivmodhi4_w1"
  [(set (match_operand:HI 0 "pic30_breg_operand"           "=b")
      (div:HI (match_operand:HI 1 "pic30_register_operand"       "r")
              (match_operand:HI 2 "pic30_ereg_operand"     "e")))
   (set (match_operand:HI 3 "pic30_wreg_operand"           "=a")
      (mod:HI (match_dup 1) (match_dup 2)))
   (clobber (reg:HI RCOUNT))
  ]
  ""
  "*
{
  if (pic30_errata_mask & exch_errata)
    return \"repeat #18-1\;\"
           \"div.uw %1,%2\;\"
           \"push w0\;\"
           \"mov w1,w0\;\"
           \"pop w1\;\";
  else
    return \"repeat #18-1\;\"
           \"div.uw %1,%2\"
           \"exch w0,w1\";
}"
  [
   (set_attr "cc" "clobber")  ; should have new CC of div
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; square root instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Other arithmetic instructions:
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Absolute value
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; Half integer
;;;;;;;;;;;;;;;;;;

;; (define_insn "abshi2"
;;   [(set (match_operand:HI 0 "pic30_register_operand"        "=r")
;;         (abs:HI (match_operand:HI 1 "pic30_register_operand" "r")))]
;;   ""
;;   "btsc %1,#15\;neg %1,%0"
;;   [(set_attr "cc" "clobber")])

;;;;;;;;;;;;;;;;;;
;; Single float
;;;;;;;;;;;;;;;;;;

(define_insn "abssf2"
  [(set (match_operand:SF 0 "pic30_register_operand"        "=r")
        (abs:SF (match_operand:SF 1 "pic30_register_operand" "0")))]
  ""
  "bclr %d0,#15"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;
;; Double float
;;;;;;;;;;;;;;;;;;

(define_insn "absdf2"
  [(set (match_operand:DF 0 "pic30_register_operand"        "=r")
        (abs:DF (match_operand:DF 1 "pic30_register_operand" "0")))]
  ""
  "bclr %q0,#15"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Negation
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; Quarter integer 
;;;;;;;;;;;;;;;;;;

(define_insn "negqi2_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")))]
  ""
  "neg.b %1,%0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "negqi2_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "neg.b %1,%0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_expand "negqi2"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "
{
  if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
    emit(gen_negqi2_DATA(operands[0],operands[1]));
  else
    emit(gen_negqi2_APSV(operands[0],operands[1]));
  DONE;
}")

(define_insn "*negqi2_sfr0"
  [(set (match_operand:QI 0 "pic30_wreg_operand"        "=a")
        (neg:QI (match_operand:QI 1 "pic30_near_operand" "U")))]
  ""
  "neg.b %1,WREG"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*negqi2_sfr1"
  [(set (match_operand:QI 0 "pic30_near_operand"        "=U")
        (neg:QI (match_dup 0)))]
  ""
  "neg.b %0"
  [(set_attr "cc" "set")])

;;;;;;;;;;;;;;;;;;
;; Half integer 
;;;;;;;;;;;;;;;;;;

(define_insn "neghi2_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")))]
  ""
  "neg %1,%0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "neghi2_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:HI 
          (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "neg %1,%0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_expand "neghi2"
  [(set (match_operand:HI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (neg:HI 
          (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "
{
  if (pic30_mode2_operand(operands[1],GET_MODE(operands[1])))
    emit(gen_neghi2_DATA(operands[0],operands[1]));
  else
    emit(gen_neghi2_DATA(operands[0],operands[1]));
  DONE;
}")

(define_insn "*neghi2_sfr0"
  [(set (match_operand:HI 0 "pic30_wreg_operand"                "=a,a")
        (neg:HI (match_operand:HI 1 "pic30_reg_or_near_operand" " U,r")))]
  ""
  "@
   neg %1,WREG
   neg %1,%0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, there is no operand 2 to interfere with a reload (CAW)
(define_insn "*neghi2_sfr1"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"        "=Ur")
        (neg:HI (match_dup 0)))]
  ""
  "neg %0"
  [
   (set_attr "cc" "set")
  ]
)

;;;;;;;;;;;;;;;;;;
;; Single integer 
;;;;;;;;;;;;;;;;;;

(define_insn "negsi2"
  [(set (match_operand:SI 0 "pic30_register_operand"        "=r")
        (neg:SI (match_operand:SI 1 "pic30_register_operand" "r")))]
  ""
  "subr %1,#0,%0\;subbr %d1,#0,%d0"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;
;; Single float
;;;;;;;;;;;;;;;;;;

(define_insn "negsf2"
  [(set (match_operand:SF 0        "pic30_register_operand" "=r")
        (neg:SF (match_operand:SF 1 "pic30_register_operand" "0")))]
  ""
  "btg %d0,#15"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*negsf2sfr"
  [(set (match_operand:SF 0        "pic30_near_operand" "=U")
        (neg:SF (match_dup 0)))]
  ""
  "btg.b %0+3,#7"
  [(set_attr "cc" "clobber")])

;;;;;;;;;;;;;;;;;;
;; Double float
;;;;;;;;;;;;;;;;;;

(define_insn "negdf2"
  [(set (match_operand:DF 0        "pic30_register_operand" "=r")
        (neg:DF (match_operand:DF 1 "pic30_register_operand" "0")))]
  ""
  "btg %q0,#15"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; bit-logical instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; Quarter integer 
;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;
;; Set Bit ;;
;;;;;;;;;;;;;

(define_insn "bitsetqi"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r,R,<>")
        (ior:QI (match_operand:QI 1 "pic30_mode2_operand" "0,0,0")
                (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[2])&0x00ff))"
  "bset.b %0,#%b2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bitsetqi_sfr"
  [(set (match_operand:QI 0 "pic30_near_operand"        "=U")
        (ior:QI  (match_dup 0)
                 (match_operand 1 "const_int_operand" "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]) & 0x00ff)"
  "bset.b %0,#%b1")

;;;;;;;;;;;;;;;
;; Reset Bit ;;
;;;;;;;;;;;;;;;

(define_insn "bitclrqi"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r,R,<>")
        (and:QI (match_operand:QI 1 "pic30_mode2_operand" "0,0,0")
                (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p((~INTVAL(operands[2])&0xff)))"
  "bclr.b %0,#%B2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bitclrqi_sfr"
  [(set (match_operand:QI 0 "pic30_near_operand"        "=U")
        (and:QI  (match_dup 0)
                 (match_operand 1 "const_int_operand" "i")))]
  "(pic30_one_bit_set_p((~INTVAL (operands[1])) & 0x00ff))"
  "bclr.b %0,#%B1")


;;;;;;;;;;;;;;;;
;; Toggle Bit ;;
;;;;;;;;;;;;;;;;

(define_insn "bittogqi"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r,R,<>")
        (xor:QI (match_operand:QI 1 "pic30_mode2_operand" "0,0,0")
                (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[2])&0xffff))"
  "btg.b %0,#%b2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bittogqi_sfr"
  [(set (match_operand:QI 0 "pic30_near_operand"        "=U")
        (xor:QI  (match_dup 0)
                 (match_operand 1 "const_int_operand" "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]) & 0x00ff)"
  "btg.b %0,#%b1")

;;;;;;;;;;;;;;
;; Test Bit ;;
;;;;;;;;;;;;;;

(define_insn "*bittstqi_and"
  [(set (cc0)
        (and (match_operand:QI 0 "pic30_reg_or_near_operand" "r,U")
             (match_operand 1 "immediate_operand" "i,i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[1])))"
  "@
   btst %0,#%b1
   btst.b %0,#%b1"
  [(set_attr "cc" "set")])
 
          
(define_insn "*bittstqi"
  [(set (cc0)
        (zero_extract (match_operand:QI 0 "pic30_reg_or_near_operand" "r,U")
		      (const_int 1)
		      (match_operand 1 "const_int_operand" "i,i")))]
  ""
  "@
   btst %0,#%1
   btst.b %0,#%1"
  [(set_attr "cc" "set")])

;;;;;;;;;;;;;;;;;;
;; Half integer ;;
;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;
;; Set Bit ;;
;;;;;;;;;;;;;

;; the pre/post modify modes have been removed due to errata in Rev A silicon
;;   the pre/post modify modes are valid for REV_A3

(define_insn "bitsethi"
  [(set (match_operand:HI 0 "pic30_mode2_operand"         "=r,R,<>")
        (ior:HI  (match_operand:HI 1 "pic30_mode2_operand" "0,0,0")
                 (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[2])))"
  "bset %0,#%b2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bitsethi_sfr"
  [(set (match_operand:HI 0 "pic30_near_operand"         "=U")
        (ior:HI  (match_dup 0)
                 (match_operand 1 "const_int_operand"  "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "*
{
	if (INTVAL(operands[1]) >= 256)
		return(\"bset.b %0+1,#%b1-8\");
	else
		return(\"bset.b %0,#%b1\");
}")

;;;;;;;;;;;;;;;
;; Reset Bit ;;
;;;;;;;;;;;;;;;

;; the pre/post modify modes have been removed due to errata in Rev A silicon
;;   the pre/post modify modes are valid for REV_A3

(define_insn "bitclrhi"
  [(set (match_operand:HI 0 "pic30_mode2_operand"         "=r,R,<>")
        (and:HI  (match_operand:HI 1 "pic30_mode2_operand" "0,0,0")
                 (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p((~INTVAL(operands[2])) & 0xffff))"
  "bclr %0,#%B2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
;        (and:HI  (match_operand:HI 1 "pic30_near_operand"  "0")
(define_insn "bitclrhi_sfr"
  [(set (match_operand:HI 0 "pic30_near_operand"           "=U")
        (and:HI  (match_dup 0)
                 (match_operand 1 "const_int_operand" "i")))]
  "(pic30_one_bit_set_p((~INTVAL(operands[1])) & 0xffff))"
  "*
{
	if (pic30_which_bit(~INTVAL(operands[1])) > 7)
		return(\"bclr.b %0+1,#%B1-8\");
	else
		return(\"bclr.b %0,#%B1\");
}")

;;;;;;;;;;;;;;;;
;; Toggle Bit ;;
;;;;;;;;;;;;;;;;

;; the pre/post modify modes have been removed due to errata in Rev A silicon
;;   the pre/post modify modes are valid for REV_A3

(define_insn "bittoghi"
  [(set (match_operand:HI 0 "pic30_mode2_operand"         "=r,R,<>")
        (xor:HI  (match_operand:HI 1 "pic30_mode2_operand" "0,0,0")
                 (match_operand 2 "const_int_operand"      "i,i,i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[2])))"
  "btg %0,#%b2"
  [
   (set_attr "type" "def,use,defuse")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bittoghi_sfr"
  [(set (match_operand:HI 0 "pic30_near_operand"           "=U")
        (xor:HI  (match_dup 0)
                 (match_operand 1 "const_int_operand" "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "*
{
	if (INTVAL(operands[1]) >= 256)
		return(\"btg.b %0+1,#%b1-8\");
	else
		return(\"btg.b %0,#%b1\");
}")

;;;;;;;;;;;;;;
;; Test Bit ;;
;;;;;;;;;;;;;;

(define_insn "*bittsthi_and_DATA"
  [(set (cc0)
        (and (match_operand:HI 0 "pic30_near_mode2_operand" "r,R,<>,U")
             (match_operand 1 "immediate_operand"           "i,i,i, i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[1])))"
  "@
   btst %0,#%b1
   btst %0,#%b1
   btst %0,#%b1
   btst.b %0+%b1/8,#%b1%%8"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,defuse,etc")
  ]
)

(define_insn "*bittsthi_and_APSV"
  [(set (cc0)
        (and (match_operand:HI 0 "pic30_near_mode2_APSV_operand" "r,R,<>,U")
             (match_operand 1 "immediate_operand"           "i,i,i, i")))]
  "(pic30_one_bit_set_p(INTVAL(operands[1])))"
  "@
   btst %0,#%b1
   btst %0,#%b1
   btst %0,#%b1
   btst.b %0+%b1/8,#%b1%%8"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,defuse,etc")
  ]
)

(define_insn "*bittsthi_DATA"
  [(set (cc0) 
        (zero_extract:HI
          (match_operand:HI 0 "pic30_near_mode2_operand" "r,R,<>,U")
          (const_int 1)
          (match_operand 1 "const_int_operand"           "i,i,i, i")))]
  ""
  "@
   btst %0,#%1
   btst %0,#%1
   btst %0,#%1
   btst.b %0+%1/8,#%1%%8"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,defuse,etc")
  ]
)

(define_insn "*bittsthi_APSV"
  [(set (cc0) 
        (zero_extract:HI
           (match_operand:HI 0 "pic30_near_mode2_APSV_operand" "r,R,<>,U")
           (const_int 1)
           (match_operand 1 "const_int_operand"           "i,i,i, i")))]
  ""
  "@
   btst %0,#%1
   btst %0,#%1
   btst %0,#%1
   btst.b %0+%1/8,#%1%%8"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use,defuse,etc")
  ]
)

;;;;;;;;;;;;;;;;;;;;
;; Single integer ;;
;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;
;; Set Bit ;;
;;;;;;;;;;;;;

(define_insn "*bitsetsiR"
  [(set (match_operand:SI 0 "pic30_reg_or_R_operand"         "=r,R")
        (ior:SI  (match_operand:SI 1 "pic30_reg_or_R_operand" "0,0")
                 (match_operand 2 "const_int_operand"         "i,i")))]
  "pic30_one_bit_set_p(INTVAL(operands[2]))"
  "*
{
	switch (which_alternative)
	{
	case 0:
		if (pic30_which_bit(INTVAL(operands[2])) < 16)
		{
			return(\"bset %0,#%b2\");
		}
		else
		{
			return(\"bset %d0,#%b2-16\");
		}
	case 1:
		if (pic30_which_bit(INTVAL(operands[2])) < 16)
		{
			return(\"bset %0,#%b2\");
		}
		else
		{
			return(\"btst %I0,#%b2-16\;bset %D0,#%b2-16\");
		}
	default:
		return(\";\");
	}
}"
  [
   (set_attr "type" "def,use")
  ]
)

(define_insn "*bitsetsir"
  [(set (match_operand:SI 0 "pic30_register_operand"         "=r")
        (ior:SI  (match_operand:SI 1 "pic30_register_operand" "0")
                 (match_operand 2 "const_int_operand"   "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[2]))"
  "*
{
	if (pic30_which_bit(INTVAL(operands[2])) < 16)
	{
		return(\"bset %0,#%b2\");
	}
	else
	{
		return(\"bset %d0,#%b2-16\");
	}
}"
  [
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "*bitsetsi_sfr"
  [(set (match_operand:SI 0 "pic30_near_operand"         "=U")
        (ior:SI  (match_dup 0)
                 (match_operand 1 "const_int_operand"  "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "*
{
	unsigned int n = INTVAL(operands[1]);

	if (n >= 0x1000000)
		return(\"bset.b %0+3,#%b1-24\");
	else if (n >= 0x10000)
		return(\"bset.b %0+2,#%b1-16\");
	else if (n >= 0x0100)
		return(\"bset.b %0+1,#%b1-8\");
	else
		return(\"bset.b %0+0,#%b1\");
}")

;;;;;;;;;;;;;;;
;; Reset Bit ;;
;;;;;;;;;;;;;;;

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "*bitclrsi_sfr"
  [(set (match_operand:SI 0 "pic30_near_operand"         "=U")
        (and:SI  (match_dup 0)
                 (match_operand 1 "const_int_operand"  "i")))]
  "pic30_one_bit_set_p((~INTVAL(operands[1])))"
  "*
{
	int n = pic30_which_bit(~INTVAL(operands[1]));

	if (n > 23)
		return(\"bclr.b %0+3,#%B1-24\");
	else if (n > 15)
		return(\"bclr.b %0+2,#%B1-16\");
	else if (n > 7)
		return(\"bclr.b %0+1,#%B1-8\");
	else
		return(\"bclr.b %0+0,#%B1\");
}")

(define_insn "*bitclrsi"
  [(set (match_operand:SI 0 "pic30_register_operand"         "=r")
        (and:SI  (match_operand:SI 1 "pic30_register_operand" "0")
                 (match_operand 2 "const_int_operand"   "i")))]
  "pic30_one_bit_set_p((~INTVAL(operands[2])))"
  "*
{
	int n = pic30_which_bit(~INTVAL(operands[2]));

	if (n > 15)
		return(\"bclr %d0,#%B2-16\");
	else
		return(\"bclr %0,#%B2\");
}"
  [
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;
;; Toggle Bit ;;
;;;;;;;;;;;;;;;;

(define_insn "bittogsi"
  [(set (match_operand:SI 0 "pic30_register_operand"         "=r")
        (xor:SI  (match_operand:SI 1 "pic30_register_operand" "0")
                 (match_operand 2 "const_int_operand"   "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[2]))"
  "*
{
	unsigned int n = INTVAL(operands[2]);
	if (n >= 65536)
		return(\"btg %d0,#%b2-16\");
	else
		return(\"btg %0,#%b2\");
}"
  [
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "bittogsi_sfr"
  [(set (match_operand:SI 0 "pic30_near_operand"         "=U")
        (xor:SI  (match_dup 0)
                 (match_operand 1 "const_int_operand"  "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "*
{
	unsigned int n = INTVAL(operands[1]);
	if (n >= 0x01000000)
		return(\"btg.b %0+3,#%b1-24\");
	else if (n >= 0x00010000)
		return(\"btg.b %0+2,#%b1-16\");
	else if (n >= 0x00000100)
		return(\"btg.b %0+1,#%b1-8\");
	else
		return(\"btg.b %0+0,#%b1-0\");
}")

;;;;;;;;;;;;;;
;; Test Bit ;;
;;;;;;;;;;;;;;

(define_insn "*bittstsir_and"
  [(set (cc0) 
        (and (match_operand:SI 0 "pic30_register_operand"  "r")
             (match_operand 1 "const_int_operand" "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "*
{
        if (INTVAL(operands[1]) < (1<<16))
        {
                return(\"btst %0,#%b1\");
        }
        else
        {
                return(\"btst %d0,#%b1-16\");
        }
}"
  [(set_attr "cc" "set")])

(define_insn "*bittstsi_sfr_and"
  [(set (cc0) 
        (and (match_operand:SI 0 "pic30_near_operand" "U")
             (match_operand 1 "const_int_operand"  "i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1]))"
  "btst.b %0+%b1/8,#%b1%%8"
  [(set_attr "cc" "set")])

(define_insn "*bittstsiR"
  [(set (cc0) (zero_extract (match_operand:SI 0 "pic30_reg_or_R_operand" "r,R")
			    (const_int 1)
			    (match_operand 1 "const_int_operand" "i,i")))]
  "INTVAL(operands[1]) < 16"
  "btst %0,#%1"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use")
  ]
)

(define_insn "*bittstsiR_and"
  [(set (cc0)
        (and (match_operand:SI 0 "pic30_reg_or_R_operand"  "r,R")
             (match_operand 1 "const_int_operand"          "i,i")))]
  "pic30_one_bit_set_p(INTVAL(operands[1])) &&
   pic30_which_bit(INTVAL(operands[1])) < 16"
  "btst %0,#%b1"
  [
   (set_attr "cc" "set")
   (set_attr "type" "etc,use")
  ]
)

(define_insn "*bittstsir"
  [(set (cc0) (zero_extract (match_operand:SI 0 "pic30_register_operand" "r")
			    (const_int 1)
			    (match_operand 1 "const_int_operand" "i")))]
  ""
  "*
{
	if (INTVAL(operands[1]) < 16)
	{
		return(\"btst %0,#%1\");
	}
	else
	{
		return(\"btst %d0,#%1-16\");
	}
}"
  [(set_attr "cc" "set")])

(define_insn "*bittstsi_sfr"
  [(set (cc0) (zero_extract (match_operand:SI 0 "pic30_near_operand" "U")
			    (const_int 1)
			    (match_operand 1 "const_int_operand"  "i")))]
  ""
  "btst.b %0+%1/8,#%1%%8"
  [(set_attr "cc" "set")])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; AND
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "andqi3_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
               "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R,r")
        (and:QI 
           (match_operand:QI 1 "pic30_mode1J_operand"
               "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P,0")
           (match_operand:QI 2 "pic30_mode1J_operand"
               "r,  R<>,r,R<>,P,  P,r,  r,  r,  r,J")))]
  ""
  "@
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,#%2,%0
   and.b %1,#%2,%0
   and.b %2,%1,%0
   and.b %2,%1,%0
   and.b %2,#%1,%0
   and.b %2,#%1,%0
   and.b #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc,def")
  ]
)

(define_insn "andqi3_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
               "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R,r")
        (and:QI 
           (match_operand:QI 1 "pic30_mode1J_APSV_operand"
               "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P,0")
           (match_operand:QI 2 "pic30_mode1J_APSV_operand"
               "r,  R<>,r,R<>,P,  P,r,  r,  r,  r,J")))]
  ""
  "@
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,%2,%0
   and.b %1,#%2,%0
   and.b %1,#%2,%0
   and.b %2,%1,%0
   and.b %2,%1,%0
   and.b %2,#%1,%0
   and.b %2,#%1,%0
   and.b #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc,def")
  ]
)

(define_expand "andqi3"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
               "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R,r")
        (and:QI
           (match_operand:QI 1 "pic30_mode1J_APSV_operand"
               "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P,0")
           (match_operand:QI 2 "pic30_mode1J_APSV_operand"
               "r,  R<>,r,R<>,P,  P,r,  r,  r,  r,J")))]
  ""
  "
{
  if (pic30_mode1J_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1J_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_andqi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_andqi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn_and_split "*andqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_register_operand"           "=a,a, d")
        (and:QI (match_operand:QI 1 "pic30_register_operand"   "%a,d, d")
                (match_operand:QI 2 "pic30_near_operand" " U,U, U")))
   (clobber (match_scratch:HI 3                          "=X,X,&r"))]
  ""
  "@
   and.b %2,WREG
   mov.b %1,w0\;and.b %2,WREG
   mov #%2,%3\;and.b %1,[%3],%0"
  "reload_completed"
  [
   (const_int 0)
  ]
"
{
  if (!pic30_wreg_operand(operands[0], QImode) &&
      !pic30_wreg_operand(operands[1], QImode))
  {
    rtx pop = gen_rtx_MEM(QImode, operands[3]);
    emit_insn(gen_movhi_address(operands[3], XEXP(operands[2],0)));
    emit_insn(gen_andqi3(operands[0], operands[1], pop));
    DONE;
  }
  else
  {
    FAIL;
  }
}
"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

(define_insn_and_split "*andqi3_sfr2"
  [(set (match_operand:QI 0 "pic30_near_operand"             "=U,U")
        (and:QI (match_operand:QI 1 "pic30_register_operand" "%a,d")
                (match_dup 0)))
   (clobber (match_scratch:HI 2                              "=X,&r"))]
  ""
  "@
   and.b %0
   mov #%0,%2\;and.b %1,[%2],[%2]"
  "reload_completed"
  [
   (const_int 0)
  ]
"
{
  if (!pic30_wreg_operand(operands[1], QImode))
  {
    rtx pop = gen_rtx_MEM(QImode, operands[2]);
    emit_insn(gen_movhi_address(operands[2], XEXP(operands[0],0)));
    emit_insn(gen_andqi3(pop, operands[1], pop));
    DONE;
  }
  else
  {
    FAIL;
  }
}
"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;

(define_insn "andhi3_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  R<>,r,R,  r<>,R,r<>")
        (and:HI 
          (match_operand:HI 1 "pic30_mode1P_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,P")
          (match_operand:HI 2 "pic30_mode1P_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,r")))]
  ""
  "@
   and %1,%2,%0
   and %1,%2,%0
   and %1,%2,%0
   and %1,%2,%0
   and %1,#%2,%0
   and %1,#%2,%0
   and %2,%1,%0
   and %2,%1,%0
   and %2,#%1,%0
   and %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,use,defuse,etc,def")
  ]
)

(define_insn "andhi3_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  R<>,r,R,  r<>,R,r<>")
        (and:HI 
          (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,P")
          (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,r")))]
  ""
  "@
   and %1,%2,%0
   and %1,%2,%0
   and %1,%2,%0
   and %1,%2,%0
   and %1,#%2,%0
   and %1,#%2,%0
   and %2,%1,%0
   and %2,%1,%0
   and %2,#%1,%0
   and %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,use,defuse,etc,def")
  ]
)

(define_expand "andhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  R<>,r,R,  r<>,R,r<>")
        (and:HI
          (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,P")
          (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,r")))]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_andhi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_andhi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn "*andhi3_imm"
  [(set (match_operand:HI 0 "pic30_register_operand"         "=r")
        (and:HI (match_operand:HI 1 "pic30_register_operand" "%0")
                 (match_operand:HI 2 "pic30_J_operand"  "J")))]
  ""
  "and #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*andhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_near_operand"         "=U,??U,??U")
        (and:HI (match_operand:HI 1 "pic30_register_operand"   "%a,??d,??r")
                (match_operand:HI 2 "pic30_near_operand"  "0,  0,??U")))
		(clobber (match_scratch:HI 3             "=X,  X, &r"))]
  ""
  "*
   switch (which_alternative) {
     case 0: return \"and %0\";
     case 1: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %1,w0\;and %0\;pop w0\";
             else
               return \"exch w0,%1\;and %0\;exch w0,%1\";
     case 2: return \"mov %2,%3\;and %3,%1,%3\;mov %3,%0\";
   }
  "
  [(set_attr "cc" "math")])

(define_insn "*andhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_near_operand"         "=U,??U,??U")
        (and:HI (match_operand:HI 1 "pic30_near_operand" "%0,  0,??U")
                (match_operand:HI 2 "pic30_register_operand"    "a,??d,??r")))
		(clobber (match_scratch:HI 3             "=X,  X, &r"))]
  ""
  "*
   switch (which_alternative) {
     case 0: return \"and %0\";
     case 1: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %2,w0\;and %0\;pop w0\";
             else
               return \"exch w0,%2\;and %0\;exch w0,%2\";
     case 2: return \"mov %1,%3\;and %3,%2,%3\;mov %3,%0\";
   }
  "
  [(set_attr "cc" "math")])

;;;;
;;    gcc will FORCE a reload for a memory operation if it is a SUBREG -
;;    if the pattern doesn't have a register option, then reload will fail
;;    even though some machines (like dsPIC) can read a byte or hi mode just
;;    fine
;;;;
(define_insn_and_split "*andhi3_sfr2"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=a,  r, r")
        (and:HI (match_operand:HI 1 "pic30_register_operand"  " 0,  r, r")
                (match_operand:HI 2 "pic30_reg_or_near_operand"
                                                              " U,  U, r")))
                (clobber (match_scratch:HI 3                  "=X,  &r,X"))]
  ""
  "@
   and %2,WREG
   mov %2,%3\;and %3,%1,%0
   and %1,%2,%0"
  "reload_completed"
  [
   (set (match_dup 3) (match_dup 2))
   (set (match_dup 0) (and:HI (match_dup 3) (match_dup 1)))
  ]
"
{
  if ((pic30_wreg_operand(operands[0], HImode) &&
      (pic30_wreg_operand(operands[1], HImode))) ||
      (pic30_register_operand(operands[2], HImode)))
  {
      FAIL;
  }
}
"
  [
   (set_attr "cc" "set")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;
;  _APSV_ odd pattern
(define_expand "andsi3"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
			"=r<>,r<>,&r<>,R,R,R,R, R, r<>")
       (and:SI (match_operand:SI 1 "pic30_mode1P_operand"
			"%r,  r,   r,  r,r,r,r, r, r")
               (match_operand:SI 2 "pic30_mode1P_operand"
			 "r  ,<>,  R,  0,R,r,<>,P, P")))]
 ""
 "
{ rtx operand0_lo,operand0_hi;
  rtx operand1_lo,operand1_hi;
  rtx operand2_lo,operand2_hi;
  rtx operand_0;

  rtx operands_1 = force_reg(SImode, operands[1]);
  rtx operands_2 = force_reg(SImode, operands[2]);

  if (GET_CODE(operands_2) == CONST_INT) {
    operand0_lo = gen_reg_rtx(HImode);
    operand1_lo = gen_reg_rtx(HImode);
    operand1_hi = gen_reg_rtx(HImode);
    operand_0 = gen_reg_rtx(SImode);
  
    emit_insn(gen_movhi(operand1_lo, gen_rtx_SUBREG(HImode,operands_1,0)));
    emit_insn(gen_movhi(operand1_hi, gen_rtx_SUBREG(HImode,operands_1,2)));
    emit_insn(gen_andhi3(operand0_lo, operand1_lo, operands_2));
    emit_insn(gen_movsi_const0(operands[0]));
    emit_insn(gen_movsi(operand_0,gen_rtx_ZERO_EXTEND(SImode, operand0_lo)));
    emit_insn(gen_iorsi3(operands[0], operands[0], operand_0));
    DONE;
  } else {
    operand0_lo = gen_reg_rtx(HImode);
    operand0_hi = gen_reg_rtx(HImode);
    operand1_lo = gen_reg_rtx(HImode);
    operand1_hi = gen_reg_rtx(HImode);
    operand2_lo = gen_reg_rtx(HImode);
    operand2_hi = gen_reg_rtx(HImode);
    operand_0 = gen_reg_rtx(SImode);

    emit_insn(gen_movhi(operand1_lo, gen_rtx_SUBREG(HImode,operands_1,0)));
    emit_insn(gen_movhi(operand1_hi, gen_rtx_SUBREG(HImode,operands_1,2)));
    emit_insn(gen_movhi(operand2_lo, gen_rtx_SUBREG(HImode,operands_2,0)));
    emit_insn(gen_movhi(operand2_hi, gen_rtx_SUBREG(HImode,operands_2,2)));
    emit_insn(gen_andhi3(operand0_lo, operand1_lo, operand2_lo));
    emit_insn(gen_andhi3(operand0_hi, operand1_hi, operand2_hi));
    emit_insn(gen_movsi(operand_0,gen_rtx_ZERO_EXTEND(SImode, operand0_hi)));
    /* ashlsi3 does not accept a mode2 operand */
    emit_insn(gen_ashlsi3(operand_0, operand_0, GEN_INT(16)));
    emit_insn(gen_movsi(operands[0], operand_0));
    emit_insn(gen_movsi(operand_0,gen_rtx_ZERO_EXTEND(SImode, operand0_lo)));
    emit_insn(gen_iorsi3(operands[0], operands[0], operand_0));
    DONE;
  }

}")

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn_and_split "*andsi3_sfr0"
  [(set (match_operand:SI 0 "pic30_data_operand"      "=T")
        (and:SI (match_dup 0)
                (match_operand:SI 1 "pic30_register_operand" "r")))
   (clobber (match_scratch:HI 2 "=&r"))]
  ""
  "mov #%0,%2\;and %1,[%2],[%2]\;and %d1,[++%2],[%2]"
  "reload_completed"
  [
   (const_int 0)
  ]
"
  {
  	rtx pop = gen_rtx_MEM(SImode, operands[2]);
	emit_insn(gen_movhi_address(operands[2], XEXP(operands[0],0)));
	emit_insn(gen_andsi3(pop, operands[1], pop));
	DONE;
  }
"
  [
   (set_attr "cc" "clobber")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn_and_split "*andsi3_sfr1"
  [(set (match_operand:SI 0 "pic30_data_operand"      "=T")
        (and:SI (match_operand:SI 1 "pic30_register_operand" "r")
                (match_dup 0)))
   (clobber (match_scratch:HI 2         "=&r"))]
  ""
  "mov #%0,%2\;and %1,[%2],[%2]\;and %d1,[++%2],[%2]"
  "reload_completed"
  [
   (const_int 0)
  ]
"
  {
  	rtx pop = gen_rtx_MEM(SImode, operands[2]);
	emit_insn(gen_movhi_address(operands[2], XEXP(operands[0],0)));
	emit_insn(gen_andsi3(pop, operands[1], pop));
	DONE;
  }
"
  [
   (set_attr "cc" "clobber")
  ]
)

;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;

(define_insn "anddi3"
  [(set (match_operand:DI 0 "pic30_register_operand"         "=r")
        (and:DI (match_operand:DI 1 "pic30_register_operand" "%r")
                (match_operand:DI 2 "pic30_register_operand"  "r")))]
  ""
  "and %2,%1,%0\;and %d2,%d1,%d0\;and %t2,%t1,%t0\;and %q2,%q1,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; OR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "*iorqi3_imm"
  [(set (match_operand:QI 0 "pic30_register_operand"         "=r")
        (ior:QI (match_operand:QI 1 "pic30_register_operand" "%0")
                (match_operand:QI 2 "pic30_J_operand"   "J")))]
  ""
  "ior.b #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "iorqi3_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (ior:QI 
           (match_operand:QI 1 "pic30_mode1P_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,#%2,%0
   ior.b %1,#%2,%0
   ior.b %2,%1,%0
   ior.b %2,%1,%0
   ior.b %2,#%1,%0
   ior.b %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_insn "iorqi3_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (ior:QI 
           (match_operand:QI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,%2,%0
   ior.b %1,#%2,%0
   ior.b %1,#%2,%0
   ior.b %2,%1,%0
   ior.b %2,%1,%0
   ior.b %2,#%1,%0
   ior.b %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_expand "iorqi3"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (ior:QI
           (match_operand:QI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_iorqi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_iorqi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn_and_split "*iorqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_register_operand"           "=a,a,r")
        (ior:QI (match_operand:QI 1 "pic30_register_operand"   "%a,d,r")
                (match_operand:QI 2 "pic30_near_operand" " U,U,U")))
   (clobber (match_scratch: HI 3                         "=X,X,&r"))]
  ""
  "@
   ior.b %2,WREG
   mov.b %1,w0\;ior.b %2,WREG
   mov #%2,%3\;ior.b %1,[%3],%0"
  "reload_completed"
  [
   (const_int 0)
  ]
"
{
  if (!pic30_wreg_operand(operands[0], QImode) &&
      !pic30_wreg_operand(operands[1], QImode))
  {
  	rtx pop = gen_rtx_MEM(QImode, operands[3]);
	emit_insn(gen_movhi_address(operands[3], XEXP(operands[2],0)));
	emit_insn(gen_iorqi3(operands[0], operands[1], pop));
  	DONE;
  }
  else
  {
  	FAIL;
  }
}
"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
 (define_insn "*iorqi3_sfr1"
   [(set (match_operand:QI 0 "pic30_near_operand"      "=U, U")
         (ior:QI (match_dup 0)
                 (match_operand:QI 1 "pic30_register_operand" "a, !d")))
    (clobber (match_scratch:HI 2          "=X, &r"))]
   ""
   "@
    ior.b %0
    mov #%0,%2\;ior.b %1,[%2],[%2]"
   [(set_attr "cc" "math")])

;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "iorhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"        "+U,r")
        (ior:HI (match_operand:HI 1 "pic30_register_operand"   "a,r")
                (match_dup 0)))
  ]
  ""
  "@
   ior %0
   ior %0,%1,%0"
  [(set_attr "cc" "math")])

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "iorhi3_sfr0a"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand"        "+U,r")
        (ior:HI (match_dup 0)
                (match_operand:HI 1 "pic30_register_operand"   "a,r")))
  ]
  ""
  "@
   ior %0
   ior %0,%1,%0"
  [(set_attr "cc" "math")])


(define_insn "*iorhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=a, a, ?r")
        (ior:HI (match_operand:HI 1 "pic30_register_operand"  "0, r, r")
                (match_operand:HI 2 "pic30_near_operand" "U,  U, U")))
                (clobber (match_scratch:HI 3            "=X,  X, &r"))]
  ""
  "@
   ior %2,WREG
   mov %1,w0\;ior %2,WREG
   mov %2,%3\;ior %3,%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 1 cannot interfere with reload (CAW)
(define_insn "*iorhi3_sfr1a"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=a, ?r")
        (ior:HI (match_operand:HI 1 "pic30_near_operand" "U,  U")
                (match_dup 0)))
   (clobber (match_scratch:HI 2 "=X,  &r"))]
  ""
  "@
   ior %1,WREG
   mov %1,%2\;ior %2,%0,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*iorhi3_imm"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=r")
        (ior:HI (match_operand:HI 1 "pic30_register_operand" "0")
                (match_operand:HI 2 "pic30_J_operand" "J")))]
  ""
  "ior #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "*iorhi3_imma"
  [(set (match_operand:HI 0 "pic30_register_operand"          "=r")
        (ior:HI (match_operand:HI 1 "pic30_J_operand" "J")
                (match_operand:HI 2 "pic30_register_operand" "0")))]
  ""
  "ior #%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "iorhi3_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,  R,r<>,R,r<>,R,  r<>,R")
        (ior:HI 
           (match_operand:HI 1 "pic30_mode1P_operand"
              "%r,  r,  r,  r,r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_operand"
              "R<>,r,  R<>,r,P,  P,r,  r,  r,  r")))
  ]
  ""
  "@
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,#%2,%0
   ior %1,#%2,%0
   ior %2,%1,%0
   ior %2,%1,%0
   ior %2,#%1,%0
   ior %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "defuse,def,use,etc,def,etc,defuse,use,def,etc")
  ]
)

(define_insn "iorhi3_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,  R,r<>,R,r<>,R,  r<>,R")
        (ior:HI 
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,  r,r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "R<>,r,  R<>,r,P,  P,r,  r,  r,  r")))
  ]
  ""
  "@
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,%2,%0
   ior %1,#%2,%0
   ior %1,#%2,%0
   ior %2,%1,%0
   ior %2,%1,%0
   ior %2,#%1,%0
   ior %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "defuse,def,use,etc,def,etc,defuse,use,def,etc")
  ]
)

(define_expand "iorhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,  R,r<>,R,r<>,R,  r<>,R")
        (ior:HI
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,  r,r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "R<>,r,  R<>,r,P,  P,r,  r,  r,  r")))
  ]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_iorhi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_iorhi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;

(define_insn "iorsi3_DATA"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r, r,<>")
       (ior:SI 
          (match_operand:SI 1 "pic30_mode1P_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "@
  ior %1,%2,%0\;ior %d1,%d2,%d0
  ior %1,%I2,%0\;ior %d1,%D2,%d0
  ior %1,%2,%I0\;ior %d1,%2,%D0
  ior %1,%I2,%I0\;ior %d1,%D2,%D0
  ior %1,%2,%I0\;ior %d1,%d2,%D0
  ior %1,#%2,%I0\;ior %d1,#0,%D0
  ior %1,#%2,%0
  ior %1,#%2,%0\;ior %d1,#0,%d0
  ior %1,#%2,%0\;ior %d1,#0,%0"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "defuse,defuse,etc,use,use,etc,def,def,def")
 ]
)

(define_insn "iorsi3_APSV"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r, r,<>")
       (ior:SI 
          (match_operand:SI 1 "pic30_mode1P_APSV_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_APSV_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "@
  ior %1,%2,%0\;ior %d1,%d2,%d0
  ior %1,%I2,%0\;ior %d1,%D2,%d0
  ior %1,%2,%I0\;ior %d1,%2,%D0
  ior %1,%I2,%I0\;ior %d1,%D2,%D0
  ior %1,%2,%I0\;ior %d1,%d2,%D0
  ior %1,#%2,%I0\;ior %d1,#0,%D0
  ior %1,#%2,%0
  ior %1,#%2,%0\;ior %d1,#0,%d0
  ior %1,#%2,%0\;ior %d1,#0,%0"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "defuse,defuse,etc,use,use,etc,def,def,def")
 ]
)

(define_expand "iorsi3"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r, r,<>")
       (ior:SI
          (match_operand:SI 1 "pic30_mode1P_APSV_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_APSV_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_iorsi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_iorsi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*iorsi3_sfr0"
  [(set (match_operand:SI 0 "pic30_data_operand" "=T")
        (ior:SI (match_dup 0)
                (match_operand:SI 1 "pic30_register_operand" "r")))
   (clobber (match_scratch:HI 2 "=&r"))]
  ""
  "mov #%0,%2\;ior %1,[%2],[%2]\;ior %d1,[++%2],[%2]"
  [(set_attr "cc" "clobber")])

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*iorsi3_sfr1"
  [(set (match_operand:SI 0 "pic30_data_operand"        "=T")
        (ior:SI (match_operand:SI 1 "pic30_register_operand"  "r")
                (match_dup 0)))
   (clobber (match_scratch:HI 2            "=&r"))]
  ""
  "
   mov #%0,%2\;ior %1,[%2],[%2]\;ior %d1,[++%2],[%2]"
  [(set_attr "cc" "clobber")])

;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;

(define_insn "iordi3"
  [(set (match_operand:DI 0 "pic30_register_operand"         "=r")
        (ior:DI (match_operand:DI 1 "pic30_register_operand" "%r")
                (match_operand:DI 2 "pic30_register_operand"  "r")))]
  ""
  "ior %2,%1,%0\;ior %d2,%d1,%d0\;ior %t2,%t1,%t0\;ior %q2,%q1,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; XOR
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;
;; quarter integer
;;;;;;;;;;;;;;;;;;

(define_insn "*xorqi3_imm"
  [(set (match_operand:QI 0 "pic30_register_operand"         "=r")
        (xor:QI (match_operand:QI 1 "pic30_register_operand" "%0")
                (match_operand:QI 2 "pic30_J_operand"   "J")))]
  ""
  "xor.b #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "xorqi3_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:QI 
           (match_operand:QI 1 "pic30_mode1P_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_operand"
              " r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,#%2,%0
   xor.b %1,#%2,%0
   xor.b %2,%1,%0
   xor.b %2,%1,%0
   xor.b %2,#%1,%0
   xor.b %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_insn "xorqi3_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:QI 
           (match_operand:QI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_APSV_operand"
              " r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,%2,%0
   xor.b %1,#%2,%0
   xor.b %1,#%2,%0
   xor.b %2,%1,%0
   xor.b %2,%1,%0
   xor.b %2,#%1,%0
   xor.b %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_expand "xorqi3"
  [(set (match_operand:QI 0 "pic30_mode2_operand"
              "=r<>,r<>,R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:QI
           (match_operand:QI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,  r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:QI 2 "pic30_mode1P_APSV_operand"
              " r,  R<>,r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_xorqi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_xorqi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

(define_insn_and_split "*xorqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_register_operand"           "=a,a, r")
        (xor:QI (match_operand:QI 1 "pic30_register_operand"   "%a,d, r")
                (match_operand:QI 2 "pic30_near_operand" " U,U, U")))
   (clobber (match_scratch:HI 3                          "=X,X,&r"))]
  ""
  "@
   xor.b %2,WREG
   mov.b %1,w0\;xor.b %2,WREG
   mov #%2,%3\;xor.b %1,[%3],%0"
  "reload_completed"
  [
   (const_int 0)
  ]
"
{
  if (!pic30_wreg_operand(operands[0], QImode) &&
      !pic30_wreg_operand(operands[1], QImode)) {
    rtx pop = gen_rtx_MEM(QImode, operands[3]);
    emit_insn(gen_movhi_address(operands[3], XEXP(operands[2],0)));
    emit_insn(gen_xorqi3(operands[0], operands[1], pop));
    DONE;
  } else {
    FAIL;
  }
}
"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*xorqi3_sfr1"
  [(set (match_operand:QI 0 "pic30_near_operand"       "=U,!???U")
        (xor:QI (match_dup 0)
                (match_operand:QI 1 "pic30_register_operand"  "a,!???d")))]
  ""
  "*
   switch (which_alternative) {
     case 0: return \"xor.b %0\";
     case 1: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %1,w0\;xor.b %0\;pop w0\";
             else
               return \"exch w0,%1\;xor.b %0\;exch w0,%1\";
   }
  "
[(set_attr "cc" "math")])

;;;;;;;;;;;;;;;
;; half integer
;;;;;;;;;;;;;;;

(define_insn "xorhi3_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>, R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:HI 
           (match_operand:HI 1 "pic30_mode1P_operand"
              "%r,  r,   r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_operand"
              "r,  R<>, r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,#%2,%0
   xor %1,#%2,%0
   xor %2,%1,%0
   xor %2,%1,%0
   xor %2,#%1,%0
   xor %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_insn "xorhi3_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>, R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:HI 
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,   r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>, r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "@
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,%2,%0
   xor %1,#%2,%0
   xor %1,#%2,%0
   xor %2,%1,%0
   xor %2,%1,%0
   xor %2,#%1,%0
   xor %2,#%1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use,def,etc,defuse,use,def,etc")
  ]
)

(define_expand "xorhi3"
  [(set (match_operand:HI 0 "pic30_mode2_operand"
              "=r<>,r<>, R,R,  r<>,R,r<>,R,  r<>,R")
        (xor:HI
           (match_operand:HI 1 "pic30_mode1P_APSV_operand"
              "%r,  r,   r,r,  r,  r,R<>,R<>,P,  P")
           (match_operand:HI 2 "pic30_mode1P_APSV_operand"
              "r,  R<>, r,R<>,P,  P,r,  r,  r,  r")))]
  ""
  "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_xorhi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_xorhi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "xorhi3_sfr2"
  [(set (match_operand:HI 0 "pic30_near_operand" "=U")
        (xor:HI (match_dup 0)
                (match_operand:HI 1 "pic30_immediate_1bit_operand" "i")))]
  ""
  "*
{  unsigned int bit_num;
   static char insn[48];

   if (GET_CODE(operands[1]) == CONST_INT) {
     int i;
     i = INTVAL(operands[1]);
 
     i = i & 0xFFFF;
     /* apparantly I can't use log here */
     /* bit_num = log(bit_num) / log(2.0);
     /* if (bit_num > 15) abort(); */
     for (bit_num = 0 ; bit_num <= 16; bit_num++)
       if (((1 << bit_num) & i) == i) break;
     if (bit_num == 16) abort();
     sprintf(insn, \"btg %%0,#%d\",bit_num);
     return insn;
   } else abort();
}
"
 [(set_attr "cc" "clobber")])

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "xorhi3_sfr3"
  [(set (match_operand:HI 0 "pic30_near_operand" "=U")
        (xor:HI (match_operand:HI 1 "pic30_immediate_1bit_operand" "i")
                (match_dup 0)))]
  ""
  "*
{  unsigned int bit_num;
   static char insn[48];

   if (GET_CODE(operands[1]) == CONST_INT) {
     int i;
     i = INTVAL(operands[1]);

     i = i & 0xFFFF;
     /* apparantly I can't use log here */
     /* bit_num = log(bit_num) / log(2.0);
     /* if (bit_num > 15) abort(); */
     for (bit_num = 0 ; bit_num <= 16; bit_num++)
       if (((1 << bit_num) & i) == i) break;
     if (bit_num == 16) abort();
     sprintf(insn, \"btg %%0,#%d\",bit_num);
     return insn;
   } else abort();
}
"
 [(set_attr "cc" "clobber")])

(define_insn "*xorhi3_imm"
  [(set (match_operand:HI 0 "pic30_register_operand"         "=r")
        (xor:HI (match_operand:HI 1 "pic30_register_operand" "%0")
                (match_operand:HI 2 "pic30_J_operand"   "J")))]
  ""
  "xor #%2,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "*xorhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand" "+a,U,!???d,!???U")
        (xor:HI (match_dup 0)
                (match_operand:HI 1 "pic30_reg_or_near_operand"
                                                        "U,a,!???U,!???d")))]
  ""
  "*
   switch (which_alternative) {
     case 0: return \"xor %1,WREG\";
     case 1: return \"xor %0\";
     case 2: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov w0,%0\;xor %1,WREG\;mov w0,%0\;pop w0\";
             else
               return \"exch w0,%0\;xor %1,WREG\;exch w0,%0\";
     case 3: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %1,w0\;xor %0\;pop w0\";
             else
               return \"exch w0,%1\;xor %0\;exch w0,%1\";
   }
  "
  [(set_attr "cc" "clobber")])

; this match_can cause issues iff operand 1 is dies in this instruction and
;   we decide to use it to reload operand 0 (CAW)
(define_insn "*xorhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_reg_or_near_operand" "+a,U,!???d,!???U")
        (xor:HI (match_operand:HI 1 "pic30_reg_or_near_operand"
                                                        "U,a,!???U,!???d")
                (match_dup 0)))]
  ""
  "*
   switch (which_alternative) {
     case 0: return \"xor %1,WREG\";
     case 1: return \"xor %0\";
     case 2: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %0,w0\;xor %1,WREG\;mov w0,%0\;pop w0\";
             else
               return \"exch w0,%0\;xor %1,WREG\;exch w0,%0\";
     case 3: if (pic30_errata_mask & exch_errata)
               return \"push w0\;mov %1,w0\;xor %0\;pop w0\";
             else
               return \"exch w0,%1\;xor %0\;exch w0,%1\";
   }
  "
[(set_attr "cc" "clobber")])

;;;;;;;;;;;;;;;;;
;; single integer
;;;;;;;;;;;;;;;;;

(define_insn "xorsi3_DATA"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r,&r,<>")
       (xor:SI 
          (match_operand:SI 1 "pic30_mode1P_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "@
  xor %1,%2,%0\;xor %d1,%d2,%d0
  xor %1,%I2,%0\;xor %d1,%D2,%d0
  xor %1,%2,%I0\;xor %d1,%2,%D0
  xor %1,%I2,%I0\;xor %d1,%D2,%D0
  xor %1,%2,%I0\;xor %d1,%d2,%D0
  xor %1,#%2,%I0\;xor %d1,#0,%D0
  xor %1,#%2,%0
  xor %1,#%2,%0\;xor %d1,#0,%d0
  xor %1,#%2,%0\;xor %d1,#0,%0"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "defuse,defuse,etc,use,use,etc,def,def,def")
 ]
)

(define_insn "xorsi3_APSV"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r,&r,<>")
       (xor:SI 
          (match_operand:SI 1 "pic30_mode1P_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_APSV_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "@
  xor %1,%2,%0\;xor %d1,%d2,%d0
  xor %1,%I2,%0\;xor %d1,%D2,%d0
  xor %1,%2,%I0\;xor %d1,%2,%D0
  xor %1,%I2,%I0\;xor %d1,%D2,%D0
  xor %1,%2,%I0\;xor %d1,%d2,%D0
  xor %1,#%2,%I0\;xor %d1,#0,%D0
  xor %1,#%2,%0
  xor %1,#%2,%0\;xor %d1,#0,%d0
  xor %1,#%2,%0\;xor %d1,#0,%0"
 [
  (set_attr "cc" "clobber")
  (set_attr "type" "defuse,defuse,etc,use,use,etc,def,def,def")
 ]
)

(define_expand "xorsi3"
 [(set (match_operand:SI 0 "pic30_mode2_operand"
             "=r<>,&r<>,R,R,R,  R,r,&r,<>")
       (xor:SI
          (match_operand:SI 1 "pic30_mode1P_operand"
             "%r,   r,  r,r,r,  r,0, r,r")
          (match_operand:SI 2 "pic30_mode1P_APSV_operand"
             "r<>, R,  0,R,r<>,P,P, P,P")))]
 ""
 "
{
  if (pic30_mode1P_operand(operands[1],GET_MODE(operands[1])) &&
      pic30_mode1P_operand(operands[2],GET_MODE(operands[2])))
    emit(gen_xorsi3_DATA(operands[0],operands[1],operands[2]));
  else
    emit(gen_xorsi3_APSV(operands[0],operands[1],operands[2]));
  DONE;
}")



;;;;;;;;;;;;;;;;;
;; double integer
;;;;;;;;;;;;;;;;;

(define_insn "xordi3"
  [(set (match_operand:DI 0 "pic30_register_operand"         "=r")
        (xor:DI (match_operand:DI 1 "pic30_register_operand" "%r")
                (match_operand:DI 2 "pic30_register_operand"  "r")))]
  ""
  "xor %2,%1,%0\;xor %d2,%d1,%d0\;xor %t2,%t1,%t0\;xor %q2,%q1,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; NOT
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; QImode ;;
;;;;;;;;;;;;

(define_insn "one_cmplqi2_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (not:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")))]
  ""
  "com.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "one_cmplqi2_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (not:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "com.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*one_cmplqi2_sfr"
  [(set (match_operand:QI 0 "pic30_near_operand"        "=U")
        (not:QI (match_dup 0)))]
  ""
  "com.b %0"
  [(set_attr "cc" "math")])

(define_expand "one_cmplqi2"
  [(set (match_operand:QI 0 "general_operand")
        (not:QI (match_operand:QI 1 "general_operand")))]
  ""
  "{
      if (pic30_mode2_operand(operands[0],QImode)) {
        if (pic30_mode2_operand(operands[1],QImode)) {
          emit(gen_one_cmplqi2_DATA(operands[0],operands[1]));
          DONE;
        } else if (pic30_mode2_APSV_operand(operands[1],QImode)) {
          emit(gen_one_cmplqi2_APSV(operands[0],operands[1]));
          DONE;
        } else {
          rtx op1 = gen_reg_rtx(GET_MODE(operands[1]));

          emit_move_insn(op1, operands[1]);
          emit(gen_one_cmplqi2_DATA(operands[0],op1));
          DONE;
        }
      }
      FAIL;
    }")

;;;;;;;;;;;;
;; HImode ;;
;;;;;;;;;;;;

(define_insn "one_cmplhi2_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (not:HI 
           (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")))]
  ""
  "com %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "one_cmplhi2_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"        "=r<>,r<>,R,R")
        (not:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")))]
  ""
  "com %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_expand "one_cmplhi2"
  [(set (match_operand: HI 0 "general_operand")
        (not: HI (match_operand:HI 1 "general_operand")))]
  ""
  "{
      if (pic30_mode2_operand(operands[0],HImode)) {
        if (pic30_mode2_operand(operands[1],HImode)) {
          emit(gen_one_cmplhi2_DATA(operands[0],operands[1]));
          DONE;
        } else if (pic30_mode2_APSV_operand(operands[1],HImode)) {
          emit(gen_one_cmplhi2_APSV(operands[0],operands[1]));
          DONE;
        } else {
          rtx op1 = gen_reg_rtx(GET_MODE(operands[1]));

          emit_move_insn(op1, operands[1]);
          emit(gen_one_cmplhi2_DATA(operands[0],op1));
          DONE;
        }
      }
      FAIL;
    }")

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*one_cmplhi2_sfr"
  [(set (match_operand:HI 0 "pic30_near_operand"        "=U")
        (not:HI (match_dup 0)))]
  ""
  "com %0"
  [(set_attr "cc" "math")])

;;;;;;;;;;;;
;; SImode ;;
;;;;;;;;;;;;

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "pic30_register_operand"        "=r")
        (not:SI (match_operand:SI 1 "pic30_register_operand" "r")))]
  ""
  "com %1,%0\;com %d1,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*one_cmplsi2_sfr"
  [(set (match_operand:SI 0 "pic30_near_operand"        "=U")
        (not:SI (match_dup 0)))]
  ""
  "com %0\;com %0+2"
  [(set_attr "cc" "clobber")])

;; DImode

(define_insn "one_cmpldi2"
  [(set (match_operand:DI 0 "pic30_register_operand"        "=r")
        (not:DI (match_operand:DI 1 "pic30_register_operand" "r")))]
  ""
  "com %1,%0\;com %d1,%d0\;com %t1,%t0\;com %q1,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*one_cmpldi2_sfr"
  [(set (match_operand:DI 0 "pic30_near_operand"        "=U")
        (not:DI (match_dup 0)))]
  ""
  "com %0\;com %0+2\;com %0+4\;com %0+6"
  [(set_attr "cc" "clobber")])


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Find first one
;;
;; Represents one plus the index of the least significant 1-bit in X,
;; represented as an integer of mode M.  (The value is zero if X is
;; zero.)  The mode of X need not be M; depending on the target
;; machine, various mode combinations may be valid.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "ffshi2_DATA"
  [(set (match_operand:HI 0 "pic30_register_operand"           "=r,r")
        (ffs:HI 
           (match_operand:HI 1 "pic30_mode2_operand" "r,R<>")))]
  ""
  "*
{
  return \"ff1r %1,%0\";
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse")
  ]
)

(define_insn "ffshi2_APSV"
  [(set (match_operand:HI 0 "pic30_register_operand"           "=r,r")
        (ffs:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,R<>")))]
  ""
  "*
{
  return \"ff1r %1,%0\";
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def,defuse")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Shift instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Arithmetic Shift Left instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; QImode ;;
;;;;;;;;;;;;

(define_insn "*ashlqi3_gen_1_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (ashift:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                   (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "sl.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashlqi3_gen_1_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (ashift:QI (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
                   (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "sl.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashlqi3_sfr_1"
  [(set (match_operand:QI 0 "pic30_wreg_or_near_operand"   "=a,U")
        (ashift:QI (match_operand:QI 1 "pic30_near_operand" "U,0")
                   (match_operand:QI 2 "pic30_I_operand"    "I,I")))]
  ""
  "@
    sl.b %1,WREG
    sl.b %0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;; For shift by n, we operate in word mode.
;; This is ok, since we are shifting left
;; (zeroes are inserted from the right).
;; However, the condition codes are not useful.

;; for register sl, must first mask off upper bits because
;;   if it was loaded with a mov.b (likely) into a dirty register (possible)
;;   then the word-sized shift value will saturate

(define_insn "ashlqi3_unused"
  [(set (match_operand:QI 0 "pic30_register_operand"                 "=r,r")
        (ashift:QI (match_operand:QI 1 "pic30_register_operand"       "r,r")
                   (match_operand:QI 2 "pic30_reg_or_P_operand" "r,P")))]
  "0"
  "@
    and.w #0xFF,%2\;sl %1,%2,%0
    sl %1,#%2,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlqi3"
  [(set (match_operand:QI 0 "pic30_register_operand"                 "=r,r")
        (ashift:QI (match_operand:QI 1 "pic30_register_operand"       "r,r")
                   (subreg:QI 
                     (match_operand:HI 2 "pic30_reg_or_P_operand" "r,P") 0)))]
  ""
  "@
    sl %1,%2,%0
    sl %1,#%2,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;
;; HImode ;;
;;;;;;;;;;;;

(define_insn "*ashlhi3_gen_1_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (ashift:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                   (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")
        )
   )
  ]
  ""
  "@
    sl %1,%0
    sl %1,%0
    sl %1,%0
    sl %1,%0"
 [
  (set_attr "cc" "math")
  (set_attr "type" "def,defuse,etc,use")
 ]
)

(define_insn "*ashlhi3_gen_1_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (ashift:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")
        )
   )
  ]
  ""
  "@
    sl %1,%0
    sl %1,%0
    sl %1,%0
    sl %1,%0"
 [
  (set_attr "cc" "math")
  (set_attr "type" "def,defuse,etc,use")
 ]
)

(define_insn_and_split "*ashlhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_register_operand"             "=a,?d")
        (ashift:HI (match_operand:HI 1 "pic30_near_operand" "U, U")
                   (match_operand:HI 2 "pic30_I_operand"    "I, I")))]
  ""
  "@
   sl %1,WREG
   mov %1,%0\;sl %0,%0"
  "reload_completed"
  [
   (set (match_dup 0) (match_dup 1))
   (set (match_dup 0) (ashift:HI (match_dup 0) (const_int 1)))
  ]
"
{
  if (pic30_wreg_operand(operands[0], HImode)) {
    FAIL;
  }
}
"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*ashlhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_near_operand"           "=U")
        (ashift:HI (match_dup 0)
                   (match_operand:HI 1 "pic30_I_operand"    "I")))]
  ""
  "sl %0"
  [(set_attr "cc" "math")])

(define_insn "ashlhi3"
  [(set (match_operand:HI 0 "pic30_register_operand"                   "=r,r,r")
        (ashift:HI (match_operand:HI 1 "pic30_register_operand"         "r,r,r")
                   (match_operand:HI 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
    		return(\"sl %1,#%2,%0\");
	case 1:
		if (INTVAL(operands[2]) < 0)
    			return(\"asr %1,#%J2%%16,%0\");
		else
    			return(\"sl %1,#%2%%16,%0\");
	case 2:
    		return(\"sl %1,%2,%0\");
	default:
		return(\"#\");
	}
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlp16apsv3"
  [(set (match_operand:P16APSV 0 "pic30_register_operand"      "=r,r,r")
        (ashift:P16APSV 
           (match_operand:P16APSV 1 "pic30_register_operand"   "r,r,r")
           (match_operand:P16APSV 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
    		return(\"sl %1,#%2,%0\");
	case 1:
		if (INTVAL(operands[2]) < 0)
    			return(\"asr %1,#%J2%%16,%0\");
		else
    			return(\"sl %1,#%2%%16,%0\");
	case 2:
    		return(\"sl %1,%2,%0\");
	default:
		return(\"#\");
	}
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;
;; SImode ;;
;;;;;;;;;;;;

(define_expand "ashlsi3"
  [(set (match_operand:SI 0 "pic30_register_operand" "")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand" "")
                   (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movsi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_ashlsi3_imm1(operands[0],
						operands[1], operands[2]));
			break;
		case 8:
			emit_insn(gen_ashlsi3_imm8(operands[0],
						operands[1], operands[2]));
			break;
		case 2 ... 7:
		case 9 ... 15:
			emit_insn(gen_ashlsi3_imm2to15(operands[0],
						operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_ashlsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_ashlsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		default:
			emit_insn(gen_movsi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_ashlsi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "ashlsi3_imm1"
  [(set (match_operand:SI 0            "pic30_register_operand" "=r")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand"  "r")
                   (match_operand:HI 2 "pic30_I_operand"         "I")))]
  ""
  "add %1,%1,%0\;addc %d1,%d1,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlsi3_imm8"
  [(set (match_operand:SI 0            "pic30_register_operand"  "=r")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand"   "r")
                   (match_operand:HI 2 "pic30_imm8_operand" "i")))]
  ""
  "*
{
	int idDst = REGNO(operands[0]);
	int idSrc = REGNO(operands[1]);
	if (idDst == idSrc)
	{
		return(	\"sl %d1,#%2,%d0\;\"
			\"swap %1\;\"
			\"mov.b %0,%d0\;\"
			\"clr.b %0\");
	}
	else
	{
		return(	\"sl %d1,#%2,%0\;\"
			\"lsr %1,#%k2,%d0\;\"
			\"ior %0,%d0,%d0\;\"
			\"sl %1,#%2,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlsi3_imm16plus"
  [(set (match_operand:SI 0            "pic30_register_operand"       "=r")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand"        "r")
                   (match_operand:HI 2 "pic30_imm16plus_operand" "i")))]
  ""
  "sl %1,#%K2,%d0\;mov #0,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlsi3_imm2to15"
  [(set (match_operand:SI 0            "pic30_register_operand"      "=r,&r")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand"       "r, r")
                   (match_operand:HI 2 "pic30_imm2to15_operand" "i, i")))
		   (clobber (match_scratch:HI 3               "=&r, X"))]
  ""
  "*
{
	int idDst, idSrc;

	switch (which_alternative)
	{
	case 0:
		/*
		** Take care that the source and dest don't overlap
		*/
		idDst = REGNO(operands[0]);
		idSrc = REGNO(operands[1]);
		if (idDst >= idSrc)
		{
			return(	\"sl %d1,#%2,%3\;\"
				\"lsr %1,#%k2,%d0\;\"
				\"ior %3,%d0,%d0\;\"
				\"sl %1,#%2,%0\");
		}
		else
		{
			return(	\"sl %1,#%2,%0\;\"
				\"sl %d1,#%2,%3\;\"
				\"lsr %1,#%k2,%d0\;\"
				\"ior %3,%d0,%d0\");
		}
	default:
		/*
		** The dest and source don't overlap
		** so use dest lsw as a temporary
		*/
		return(	\"sl %d1,#%2,%0\;\"
			\"lsr %1,#%k2,%d0\;\"
			\"ior %0,%d0,%d0\;\"
			\"sl %1,#%2,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlsi3_reg"
  [(set (match_operand:SI            0 "pic30_register_operand" "=r")
        (ashift:SI (match_operand:SI 1 "pic30_register_operand" " 0")
                   (match_operand:HI 2 "pic30_register_operand" " r")))
   (clobber (match_scratch:HI        3                          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"add %1,%1,%0\;\"
              \"addc %d1,%d1,%d0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)
;
; P32EDS
;

(define_expand "ashlp32eds3"
  [(set (match_operand:P32EDS 0 "pic30_register_operand" "")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand" "")
		   (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movp32eds(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_ashlp32eds3_imm1(operands[0],
						operands[1], operands[2]));
			break;
		case 8:
			emit_insn(gen_ashlp32eds3_imm8(operands[0],
						operands[1], operands[2]));
			break;
		case 2 ... 7:
		case 9 ... 15:
			emit_insn(gen_ashlp32eds3_imm2to15(operands[0],
						operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_ashlp32eds3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_ashlp32eds3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		default:
			emit_insn(gen_movp32eds(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_ashlp32eds3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "ashlp32eds3_imm1"
  [(set (match_operand:P32EDS 0            "pic30_register_operand" "=r")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand"  "r")
		   (match_operand:HI 2 "pic30_I_operand"         "I")))]
  ""
  "add %1,%1,%0\;rlc %0,[w15]\;addc %d1,%d1,%d0\;bclr %0,#15"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlp32eds3_imm8"
  [(set (match_operand:P32EDS 0            "pic30_register_operand"  "=r")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand"   "r")
		   (match_operand:HI 2 "pic30_imm8_operand" "i")))]
  ""
  "*
{
	int idDst = REGNO(operands[0]);
	int idSrc = REGNO(operands[1]);
	if (idDst == idSrc)
	{
		return(	\"sl %d1,#%2,%d0\;\"
			\"swap %1\;\"
			\"mov.b %0,%d0\;\"
			\"clr.b %0\");
	}
	else
	{
		return(	\"sl %d1,#%2,%0\;\"
			\"lsr %1,#%k2,%d0\;\"
			\"ior %0,%d0,%d0\;\"
			\"sl %1,#%2,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlp32eds3_imm16plus"
  [(set (match_operand:P32EDS 0            "pic30_register_operand"       "=r")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand"        "r")
		   (match_operand:HI 2 "pic30_imm16plus_operand" "i")))]
  ""
  "sl %1,#%K2,%d0\;mov #0,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlp32eds3_imm2to15"
  [(set (match_operand:P32EDS 0            "pic30_register_operand"      "=r,&r")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand"       "r, r")
		   (match_operand:HI 2 "pic30_imm2to15_operand" "i, i")))
		   (clobber (match_scratch:HI 3               "=&r, X"))]
  ""
  "*
{
	int idDst, idSrc;

	switch (which_alternative)
	{
	case 0:
		/*
		** Take care that the source and dest don't overlap
		*/
		idDst = REGNO(operands[0]);
		idSrc = REGNO(operands[1]);
		if (idDst >= idSrc)
		{
			return(	\"sl %d1,#%2,%3\;\"
				\"lsr %1,#%k2,%d0\;\"
				\"ior %3,%d0,%d0\;\"
				\"sl %1,#%2,%0\");
		}
		else
		{
			return(	\"sl %1,#%2,%0\;\"
				\"sl %d1,#%2,%3\;\"
				\"lsr %1,#%k2,%d0\;\"
				\"ior %3,%d0,%d0\");
		}
	default:
		/*
		** The dest and source don't overlap
		** so use dest lsw as a temporary
		*/
		return(	\"sl %d1,#%2,%0\;\"
			\"lsr %1,#%k2,%d0\;\"
			\"ior %0,%d0,%d0\;\"
			\"sl %1,#%2,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashlp32eds3_reg"
  [(set (match_operand:P32EDS            0 "pic30_register_operand" "=r")
	(ashift:P32EDS (match_operand:P32EDS 1 "pic30_register_operand" " 0")
		   (match_operand:HI 2 "pic30_register_operand" " r")))
   (clobber (match_scratch:HI        3                          "=2"))]
  ""
  "*
{
      return( 
#if (1)
	      \".set ___BP___,0\n\"
#endif
	      \".LB%=:\;\"
	      \"dec %2,%2\;\"
	      \"bra n,.LE%=\;\"
	      \"add %1,%1,%0\;\"
	      \"addc %d1,%d1,%d0\;\"
	      \"bra .LB%=\n\"
	      \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DImode arithmetic shift left.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "ashldi3"
  [(set (match_operand:DI 0 "pic30_register_operand"            "")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand"  "")
                   (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movdi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_ashldi3_imm1(operands[0],
						    operands[1], operands[2]));
			break;
#if 1
                case 2 ... 63:
			emit_insn(gen_ashldi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#else
		case 2 ... 15:
			emit_insn(gen_ashldi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_ashldi3_imm16(operands[0],
						    operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_ashldi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 32:
			emit_insn(gen_ashldi3_imm32(operands[0],
						    operands[1], operands[2]));
			break;
		case 33 ... 47:
			emit_insn(gen_ashldi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 48:
			emit_insn(gen_ashldi3_imm48(operands[0],
						    operands[1], operands[2]));
			break;
		case 49 ... 63:
			emit_insn(gen_ashldi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#endif
		default:
			emit_insn(gen_movdi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_ashldi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "ashldi3_imm1"
  [(set (match_operand:DI            0 "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand" " r")
                   (match_operand:HI 2 "immediate_operand"      " i")))]
  "INTVAL(operands[2])==1"
  "sl %1,%0\;rlc %d1,%d0\;rlc %t1,%t0\;rlc %q1,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashldi3_imm16"
  [(set (match_operand:DI 0            "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                   (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==16"
  "mov %t1,%q0\;mov %d1,%t0\;mov %1,%d0\;mov #0,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashldi3_imm32"
  [(set (match_operand:DI 0            "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                   (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==32"
  "mov.d %1,%t0\;mul.su %0,#0,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashldi3_imm48"
  [(set (match_operand:DI 0            "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                   (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==48"
  "mov %1,%q0\;mov #0,%t0\;mul.su %0,#0,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashldi3_immn"
  [(set (match_operand:DI            0 "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand" " 0")
                   (match_operand:HI 2 "immediate_operand"      " i")))
   (clobber (match_scratch:HI        3                          "=&r"))]
  ""
  "*
{ static char buffer[256];
  char *f;
  int results = 0;
  rtx result_reg;
      
  /* how many results do we need?
     if all but the low reg is dead we can shift fewer values */
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0]));
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg))) 
    results = 1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+1);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+2);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+2;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+3);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+3;

  f = buffer;
  f += sprintf(buffer,\".set ___BP___,0 ; (%d,%d)\;\", 
               INTVAL(operands[2]), results);
  if (INTVAL(operands[2]) < 16) {
    switch (results) {
      case 4:
        if (!optimize_size) {
          f += sprintf(f,
             \"sl %%q1,#%%2,%%q0\;\"
             \"lsr %%t1,#%d,%%3\;\"
             \"ior %%3,%%q0,%%q0\;\"
             \"sl %%t1,#%%2,%%t0\;\"
             \"lsr %%d1,#%d,%%3\;\"
             \"ior %%3,%%t0,%%t0\;\"
             \"sl %%d1,#%%2,%%d0\;\"
             \"lsr %%1,#%d,%%3\;\"
             \"ior %%3,%%d0,%%d0\;\"
             \"sl %%1,#%%2,%%0\",
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]));
        } else {
          f += sprintf(f,
            \"mov #%%2,%%3\;\"
            \".LB%%=:\;\"
            \"sl %%1,%%0\;\"
            \"rlc %%d1,%%d0\;\"
            \"rlc %%t1,%%t0\;\"
            \"rlc %%q1,%%q0\;\"
            \"dec %%3,%%3\;\"
            \"bra nz,.LB%%=\");
        }
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"sl %%t1,#%%2,%%t0\;\"
           \"lsr %%d1,#%d,%%3\;\"
           \"ior %%3,%%t0,%%t0\;\"
           \"sl %%d1,#%%2,%%d0\;\"
           \"lsr %%1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"sl %%1,#%%2,%%0\", 
           16 - INTVAL(operands[2]), 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"sl %%d1,#%%2,%%d0\;\"
           \"lsr %%1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"sl %%1,#%%2,%%0\", 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
          \"sl %%1,#%%2,%%0\");
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 16) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"mov %%t1,%%q0\;\"
           \"mov %%d1,%%t0\;\"
           \"mov %%1,%%d0\;\"
           \"clr %%0\");
        break;
      case 3:
        f += sprintf(f,
           \"mov %%d1,%%t0\;\"
           \"mov %%1,%%d0\;\"
           \"clr %%0\");
        break;
      case 2:
        f += sprintf(f,
           \"mov %%1,%%d0\;\"
           \"clr %%0\");
        break;
      case 1:
        f += sprintf(f,
           \"clr %%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256) 
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"sl %%t1,#%d,%%q0\;\"
           \"lsr %%d1,#%d,%%3\;\"
           \"ior %%3,%%q0,%%q0\;\"
           \"sl %%d1,#%d,%%t0\;\"
           \"lsr %%1,#%d,%%3\;\"
           \"ior %%3,%%t0,%%t0\;\"
           \"sl %%1,#%d,%%d0\;\"
           \"clr %%0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"sl %%d1,#%d,%%t0\;\"
           \"lsr %%1,#%d,%%3\;\"
           \"ior %%3,%%t0,%%t0\;\"
           \"sl %%1,#%d,%%d0\;\"
           \"clr %%0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"sl %%1,#%d,%%d0\;\"
           \"clr %%0\",
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,\"clr %%0\");
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"mov.d %%1,%%t0\;\"
           \"mul.uu %%0,#0,%%0\");
        break;
      case 3:
        f += sprintf(f,
           \"mov %%1,%%t0\;\"
           \"mul.uu %%0,#0,%%0\");
        break;
      case 2:
        f += sprintf(f,
           \"mul.uu %%0,#0,%%0\");
        break;
      case 1:
        f += sprintf(f,
           \"clr %%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256)
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 48) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"sl %%d1,#%d,%%q0\;\"
           \"lsr %%1,#%d,%%3\;\"
           \"ior %%3,%%q0,%%q0\;\"
           \"sl %%1,#%d,%%t0\;\"
           \"mul.uu %%0,#0,%%0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"sl %%1,#%d,%%t0\;\"
           \"mul.uu %%0,#0,%%0\",
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f, \"mul.uu %%0,#0,%%0\");
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f, \"clr %%0\");
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 48) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"mov %%1,%%q0\;\"
           \"clr %%t0\;\"
           \"mul.uu %%0,#0,%%0\");
        break;
      case 3:
        f += sprintf(f,
           \"clr %%t0\;\"
           \"mul.uu %%0,#0,%%0\");
        break;
      case 2:
        f += sprintf(f,
           \"mul.uu %%0,#0,%%0\");
        break;
      case 1:
        f += sprintf(f,
           \"clr %%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256)
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 64) {
    switch (results) {
      case 4: 
        f += sprintf(f, \"sl %%1,#%d,%%q0\;\", INTVAL(operands[2])-48);
        /* FALLSTHROUGH */
      case 3:
        f += sprintf(f, \"clr %%t0\;\");
        /* FALLSTHROUGH */
      case 2:
        f += sprintf(f, \"mul.uu %%0,#0,%%0\"); 
        break;
      case 1:
        f += sprintf(f, \"clr %%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256) 
      error(\"Unexpected error during assembly generation\");
    return buffer;
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashldi3_reg"
  [(set (match_operand:DI            0 "pic30_register_operand" "=r")
        (ashift:DI (match_operand:DI 1 "pic30_register_operand" " 0")
                   (match_operand:HI 2 "pic30_register_operand" " r")))
   (clobber (match_scratch:HI        3                          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"sl %1,%0\;\"
              \"rlc %d1,%d0\;\"
              \"rlc %t1,%t0\;\"
              \"rlc %q1,%q0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Logical Shift Right instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; QImode ;;
;;;;;;;;;;;;

(define_insn "*lshrqi3_one_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (lshiftrt:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                     (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "lsr.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*lshrqi3_one_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (lshiftrt:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "lsr.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*lshrqi3_sfr0"
  [(set (match_operand:QI 0 "pic30_register_operand"               "=a,r")
        (lshiftrt:QI (match_operand:QI 1 "pic30_near_operand" "U,U")
                     (match_operand:QI 2 "pic30_I_operand"    "I,I")))]
  ""
  "@
    lsr.b %1,WREG
    mov #%1,%0\;lsr.b [%0],%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*lshrqi3_sfr1"
  [(set (match_operand:QI 0 "pic30_near_operand"          "=U")
        (lshiftrt:QI (match_dup 0)
                     (match_operand:QI 1 "pic30_I_operand" "I")))]
  ""
  "lsr %0"
  [(set_attr "cc" "math")])

;;;;;;;;;;;;
;; HImode ;;
;;;;;;;;;;;;

(define_insn "*lshrhi3_one_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (lshiftrt:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                     (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "lsr %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*lshrhi3_one_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (lshiftrt:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "lsr %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*lshrhi3_sfr0"
  [(set (match_operand:HI 0 "pic30_register_operand"               "=a,?r")
        (lshiftrt:HI (match_operand:HI 1 "pic30_data_operand" "U, T")
                     (match_operand:HI 2 "pic30_I_operand"    "I, I")))]
  ""
  "@
   lsr %1,WREG
   mov %1,%0\;lsr %0,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

; leave this match_dup, operand 0 will not require a reload (CAW)
(define_insn "*lshrhi3_sfr1"
  [(set (match_operand:HI 0 "pic30_near_operand"          "=U")
        (lshiftrt:HI (match_dup 0)
                     (match_operand:HI 1 "pic30_I_operand" "I")))]
  ""
  "lsr %0"
  [(set_attr "cc" "math")])

(define_insn "lshrhi3"
  [(set (match_operand:HI 0 "pic30_register_operand"                "=r,r,r")
        (lshiftrt:HI (match_operand:HI 1 "pic30_register_operand"   "r,r,r")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "@
    lsr %1,#%2,%0
    clr %0
    lsr %1,%2,%0"
  [
   (set_attr "cc" "math,change0,math")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrp16apsv3"
  [(set (match_operand:P16APSV   0 "pic30_register_operand"   "=r,r,r")
        (lshiftrt:P16APSV
          (match_operand:P16APSV 1 "pic30_register_operand"   "r,r,r")
          (match_operand:P16APSV 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "@
    lsr %1,#%2,%0
    clr %0
    lsr %1,%2,%0"
  [
   (set_attr "cc" "math,change0,math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;
;; SImode ;;
;;;;;;;;;;;;

(define_expand "lshrsi3"
  [(set (match_operand:SI 0 "pic30_register_operand"              "")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movsi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_lshrsi3_imm1(operands[0],
						operands[1], operands[2]));
			break;
		case 2 ... 15:
			emit_insn(gen_lshrsi3_imm2to15(operands[0],
						operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_lshrsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_lshrsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		default:
			emit_insn(gen_movsi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_lshrsi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "lshrsi3_imm1"
  [(set (match_operand:SI 0 "pic30_register_operand"             "=r")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand" "r")
                     (match_operand:HI 2 "pic30_I_operand"  "I")))]
  ""
  "
    lsr %d1,%d0\;rrc %1,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrsi3_imm16plus"
  [(set (match_operand:SI 0            "pic30_register_operand"         "=r")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand"        "r")
                     (match_operand:HI 2 "pic30_imm16plus_operand" "i")))]
  ""
  "*
{
  if (find_regno_note(insn, REG_UNUSED,  REGNO(operands[1])+1))
    return \"lsr %d1,#%K2,%0\";
  else
    return \"lsr %d1,#%K2,%0\;mov #0,%d0\";
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrsi3_imm2to15"
  [(set (match_operand:SI 0            "pic30_register_operand"        "=r")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand"       "r")
                     (match_operand:HI 2 "pic30_imm2to15_operand" "i")))
		     (clobber (match_scratch:HI 3               "=&r"))]
  ""
  "*
{
	/*
	** Take care that the source and dest don't overlap
	*/
	if (REGNO(operands[0]) <= REGNO(operands[1]))
	{
		return(	\"sl %d1,#%k2,%3\;\"
			\"lsr %1,#%2,%0\;\"
			\"ior %3,%0,%0\;\"
			\"lsr %d1,#%2,%d0\");
	}
	else
	{
		return(	\"lsr %d1,#%2,%d0\;\"
			\"sl %d1,#%k2,%3\;\"
			\"lsr %1,#%2,%0\;\"
			\"ior %3,%0,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrsi3_reg"
  [(set (match_operand:SI 0 "pic30_register_operand"             "=r")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand" "0")
                     (match_operand:HI 2 "pic30_register_operand" "r")))
		     (clobber (match_scratch:HI 3          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"lsr %d1,%d0\;\"
              \"rrc %1,%0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DImode arithmetic shift left.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "lshrdi3"
  [(set (match_operand:DI 0 "pic30_register_operand"              "")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movdi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_lshrdi3_imm1(operands[0],
						    operands[1], operands[2]));
			break;
#if 1
                case 2 ... 63:
			emit_insn(gen_lshrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#else
		case 2 ... 15:
			emit_insn(gen_lshrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_lshrdi3_imm16(operands[0],
						    operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_lshrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 32:
			emit_insn(gen_lshrdi3_imm32(operands[0],
						    operands[1], operands[2]));
			break;
		case 33 ... 47:
			emit_insn(gen_lshrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 48:
			emit_insn(gen_lshrdi3_imm48(operands[0],
						    operands[1], operands[2]));
			break;
		case 49 ... 63:
			emit_insn(gen_lshrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#endif
		default:
			emit_insn(gen_movdi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_lshrdi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "lshrdi3_imm1"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==1"
  "lsr %q1,%q0\;rrc %t1,%t0\;rrc %d1,%d0\;rrc %1,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrdi3_imm16"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==16"
  "mov %d1,%0\;mov %t1,%d0\;mul.uu %q1,#1,%t0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrdi3_imm32"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==32"
  "mov.d %t1,%0\;mul.uu %t0,#0,%t0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrdi3_imm48"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==48"
  "mul.uu %q1,#1,%0\;mul.uu %t0,#0,%t0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrdi3_immn"
  [(set (match_operand:DI 0 "pic30_register_operand"              "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "0")
                     (match_operand:HI 2 "immediate_operand" "i")))
		     (clobber (match_scratch:HI 3           "=&r"))]
  ""
  "*
{ static char buffer[256];
  char *f;
  int results = 0;
  rtx result_reg;
      
  /* how many results do we need?
     if all but the low reg is dead we can shift fewer values */
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0]));
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg))) 
    results = 1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+1);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+2);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+2;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+3);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+3;

  f = buffer;
  f += sprintf(buffer,\".set ___BP___,0 ; (%d,%d)\;\", 
               INTVAL(operands[2]), results);
  if (INTVAL(operands[2]) < 16) {
    switch (results) {
      case 4:
        if (!optimize_size) {
          f += sprintf(f,
             \"lsr %%1,#%%2,%%0\;\"
             \"sl %%d1,#%d,%%3\;\"
             \"ior %%3,%%0,%%0\;\"
             \"lsr %%d1,#%%2,%%d0\;\"
             \"sl %%t1,#%d,%%3\;\"
             \"ior %%3,%%d0,%%d0\;\"
             \"lsr %%t1,#%%2,%%t0\;\"
             \"sl %%q1,#%d,%%3\;\"
             \"ior %%3,%%t0,%%t0\;\"
             \"lsr %%q1,#%%2,%%q0\",
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]));
        } else {;;;;
          f += sprintf(f,
            \"mov #%%2,%%3\;\"
            \".LB%%=:\;\"
            \"lsr %%q1,%%q0\;\"
            \"rrc %%t1,%%t0\;\"
            \"rrc %%d1,%%d0\;\"
            \"rrc %%1,%%0\;\"
            \"dec %%3,%%3\;\"
            \"bra nz,.LB%%=\");
        }
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%1,#%%2,%%0\;\"
           \"sl %%d1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%d1,#%%2,%%d0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"lsr %%t1,#%%2,%%t0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%t0,%%t0\",
           16 - INTVAL(operands[2]), 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%1,#%%2,%%0\;\"
           \"sl %%d1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%d1,#%%2,%%d0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\",
           16 - INTVAL(operands[2]), 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
          \"lsr %%1,#%%2,%%0\;\"
          \"sl %%d1,#%d,%%3\;\"
          \"ior %%3,%%0,%%0\",
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 16) {
    switch (results) {
      case 4: 
      case 3:
        f += sprintf(f,
          \"mov %%d1,%%0\;\"
          \"mov %%t1,%%d0\;\"
          \"mul.uu %%q1,#1,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mov %%d1,%%0\;\"
          \"mov %%t1,%%d0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov %%d1,%%0\");
        break;
     }
     if ((unsigned)f - (unsigned)buffer > 256) 
        error(\"Unexpected error during assembly generation\");
     return buffer;
  } else if (INTVAL(operands[2]) < 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"lsr %%q1,#%d,%%t0\;\"
           \"clr %%q0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"lsr %%q1,#%d,%%t0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
          \"mov.d %%t1,%%0\;\"
          \"mul.uu %%t0,#0,%%t0\");
        break;
      case 3:
        f += sprintf(f,
          \"mov %%t1,%%0\;\"
          \"mul.uu %%t0,#0,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mov.d %%t1,%%0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov.w %%t1,%%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256)
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 48) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%q1,#%d,%%d0\;\"
           \"mul.uu %%t0,#0,%%t0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%q1,#%d,%%d0\;\"
           \"clr %%t0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%q1,#%d,%%d0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 48) {
    switch (results) {
      case 4:
      case 3:
        f += sprintf(f,
          \"mul.uu %%q1,#1,%%0\;\"
          \"mul.uu %%t0,#0,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mul.uu %%q1,#1,%%0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov %%q1,%%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256) 
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 64) {
    f += sprintf(f, \"lsr %%q1,#%d,%%0\", INTVAL(operands[2])-48);
    switch (results) {
      case 4: 
        f += sprintf(f,
          \"\;mul.uu %%t0,#0,%%t0\;\"
          \"clr %%d0\");
        break;
      case 3:
        f += sprintf(f,
          \"\;clr %%t0\;\"
          \"clr %%d0\"); 
        break;
      case 2:
        f += sprintf(f,
          \"\;clr %%d0\"); 
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256) 
      error(\"Unexpected error during assembly generation\");
    return buffer;
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "lshrdi3_reg"
  [(set (match_operand:DI 0 "pic30_register_operand"             "=r")
        (lshiftrt:DI (match_operand:DI 1 "pic30_register_operand" "0")
                     (match_operand:HI 2 "pic30_register_operand" "r")))
		     (clobber (match_scratch:HI 3          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"lsr %q1,%q0\;\"
              \"rrc %t1,%t0\;\"
              \"rrc %d1,%d0\;\"
              \"rrc %1,%0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Arithmetic shift right.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; QImode arithmetic shift right.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*ashrqi3_one_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (ashiftrt:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                   (match_operand:QI 2 "pic30_I_operand"       "I,  I,  I,I")))]
  ""
  "asr.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashrqi3_one_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (ashiftrt:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:QI 2 "pic30_I_operand"       "I,  I,  I,I")))]
  ""
  "asr.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashrqi3_sfr"
  [(set (match_operand:QI 0 "pic30_wreg_or_near_operand"     "=a,U")
        (ashiftrt:QI (match_operand:QI 1 "pic30_near_operand" "U,0")
                   (match_operand:QI 2 "pic30_I_operand"      "I,I")))]
  ""
  "@
    asr.b %1,WREG
    asr.b %0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; HImode arithmetic shift right.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*ashrhi3_one_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (ashiftrt:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                     (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "asr %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashrhi3_one_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (ashiftrt:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "asr %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*ashrhi3_sfr"
  [(set (match_operand:HI 0 "pic30_wreg_or_near_operand"     "=a,U")
        (ashiftrt:HI (match_operand:HI 1 "pic30_near_operand" "U,0")
                     (match_operand:HI 2 "pic30_I_operand"    "I,I")))]
  ""
  "@
    asr %1,WREG
    asr %0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

(define_insn "ashrhi3"
  [(set (match_operand:HI 0 "pic30_register_operand"                "=r,r,r")
        (ashiftrt:HI (match_operand:HI 1 "pic30_register_operand"   "r,r,r")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
    		return(\"asr %1,#%2,%0\");
	case 1:
		if (INTVAL(operands[2]) < 0)
    			return(\"sl %1,#%J2%%16,%0\");
		else
    			return(\"asr %1,#%2%%16,%0\");
	default:
    		return(\"asr %1,%2,%0\");
	}
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrp16apsv3"
  [(set (match_operand:P16APSV   0 "pic30_register_operand"   "=r,r,r")
        (ashiftrt:P16APSV 
          (match_operand:P16APSV 1 "pic30_register_operand"   "r,r,r")
          (match_operand:P16APSV 2 "pic30_reg_or_imm_operand" "K,i,r")))]
  ""
  "*
{
	switch (which_alternative)
	{
	case 0:
    		return(\"asr %1,#%2,%0\");
	case 1:
		if (INTVAL(operands[2]) < 0)
    			return(\"sl %1,#%J2%%16,%0\");
		else
    			return(\"asr %1,#%2%%16,%0\");
	default:
    		return(\"asr %1,%2,%0\");
	}
}"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; SImode arithmetic shift right.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "ashrsi3"
  [(set (match_operand:SI 0 "pic30_register_operand"              "")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movsi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_ashrsi3_imm1(operands[0],
						operands[1], operands[2]));
			break;
		case 2 ... 15:
			emit_insn(gen_ashrsi3_imm2to15(operands[0],
						operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_ashrsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_ashrsi3_imm16plus(operands[0],
						operands[1], operands[2]));
			break;
		default:
			emit_insn(gen_movsi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_ashrsi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")
(define_insn "ashrsi3_imm1"
  [(set (match_operand:SI 0 "pic30_register_operand"             "=r")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand" "r")
                     (match_operand:HI 2 "pic30_I_operand"  "I")))]
  ""
  "
    asr %d1,%d0\;rrc %1,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrsi3_imm2to15"
  [(set (match_operand:SI 0            "pic30_register_operand"        "=r")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand"       "r")
                     (match_operand:HI 2 "pic30_imm2to15_operand" "i")))
		     (clobber (match_scratch:HI 3                "=&r"))]
  ""
  "*
{
	/*
	** Take care that the source and dest don't overlap
	*/
	if (REGNO(operands[0]) <= REGNO(operands[1]))
	{
		return(	\"sl %d1,#%k2,%3\;\"
			\"lsr %1,#%2,%0\;\"
			\"ior %3,%0,%0\;\"
			\"asr %d1,#%2,%d0\");
	}
	else
	{
		return(	\"asr %d1,#%2,%d0\;\"
			\"sl %d1,#%k2,%3\;\"
			\"lsr %1,#%2,%0\;\"
			\"ior %3,%0,%0\");
	}
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrsi3_imm16plus"
  [(set (match_operand:SI 0            "pic30_register_operand"         "=r")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand"        "r")
                     (match_operand:HI 2 "pic30_imm16plus_operand" "i")))]
  ""
  "asr %d1,#%K2,%0\;asr %0,#15,%d0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrsi3_reg"
  [(set (match_operand:SI 0 "pic30_register_operand"             "=r")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand" "0")
                     (match_operand:HI 2 "pic30_register_operand" "r")))
		     (clobber (match_scratch:HI 3          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"asr %d1,%d0\;\"
              \"rrc %1,%0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; DImode arithmetic shift right.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "ashrdi3"
  [(set (match_operand:DI 0 "pic30_register_operand"              "")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "")
                     (match_operand:HI 2 "pic30_reg_or_imm_operand" "")))]
  ""
  "
{
	if (GET_CODE(operands[2]) == CONST_INT)
	{
		switch (INTVAL(operands[2]))
		{
		case 0:
			emit_insn(gen_movdi(operands[0], operands[1]));
			break;
		case 1:
			emit_insn(gen_ashrdi3_imm1(operands[0],
						    operands[1], operands[2]));
			break;
#if 1
		case 2 ... 63:
			emit_insn(gen_ashrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#else
		case 2 ... 15:
			emit_insn(gen_ashrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 16:
			emit_insn(gen_ashrdi3_imm16(operands[0],
						    operands[1], operands[2]));
			break;
		case 17 ... 31:
			emit_insn(gen_ashrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 32:
			emit_insn(gen_ashrdi3_imm32(operands[0],
						    operands[1], operands[2]));
			break;
		case 33 ... 47:
			emit_insn(gen_ashrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
		case 48:
			emit_insn(gen_ashrdi3_imm48(operands[0],
						    operands[1], operands[2]));
			break;
		case 49 ... 63:
			emit_insn(gen_ashrdi3_immn(operands[0],
						   operands[1], operands[2]));
			break;
#endif
		default:
			emit_insn(gen_movdi(operands[0], const0_rtx));
			break;
		}
	}
	else
	{
		emit_insn(gen_ashrdi3_reg(operands[0],operands[1],operands[2]));
	}
	DONE;
}")

(define_insn "ashrdi3_imm1"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==1"
  "asr %q1,%q0\;rrc %t1,%t0\;rrc %d1,%d0\;rrc %1,%0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrdi3_imm16"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==16"
  "mov %d1,%0\;mov %t1,%d0\;mul.su %q1,#1,%t0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrdi3_imm32"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==32"
  "mov.d %t1,%0\;asr %d0,#15,%t0\;mov %t0,%q0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrdi3_imm48"
  [(set (match_operand:DI 0            "pic30_register_operand"   "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "r")
                     (match_operand:HI 2 "immediate_operand" "i")))]
  "INTVAL(operands[2])==48"
  "mul.su %q1,#1,%0\;mul.su %d0,#1,%t0"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrdi3_immn"
  [(set (match_operand:DI 0 "pic30_register_operand"              "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand"  "0")
                     (match_operand:HI 2 "immediate_operand" "i")))
		     (clobber (match_scratch:HI 3           "=&r"))]
  ""
  "*
{ static char buffer[256];
  char *f;
  int results = 0;
  rtx result_reg;
      
  /* how many results do we need?
     if all but the low reg is dead we can shift fewer values */
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0]));
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg))) 
    results = 1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+1);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+1;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+2);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+2;
  result_reg = gen_rtx_REG(HImode, REGNO(operands[0])+3);
  if (!pic30_dead_or_set_p(insn, result_reg) &&
      !find_regno_note(insn, REG_UNUSED, REGNO(result_reg)))
    results = 1+3;

  f = buffer;
  f += sprintf(buffer,\".set ___BP___,0 ; (%d,%d)\;\", 
               INTVAL(operands[2]), results);
  if (INTVAL(operands[2]) < 16) {
    switch (results) {
      case 4:
        if (!optimize_size) {
          f += sprintf(f,
             \"lsr %%1,#%%2,%%0\;\"
             \"sl %%d1,#%d,%%3\;\"
             \"ior %%3,%%0,%%0\;\"
             \"lsr %%d1,#%%2,%%d0\;\"
             \"sl %%t1,#%d,%%3\;\"
             \"ior %%3,%%d0,%%d0\;\"
             \"lsr %%t1,#%%2,%%t0\;\"
             \"sl %%q1,#%d,%%3\;\"
             \"ior %%3,%%t0,%%t0\;\"
             \"asr %%q1,#%%2,%%q0\",
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]), 
             16 - INTVAL(operands[2]));
        } else {
          f += sprintf(f,
            \"mov #%%2,%%3\;\"
            \".LB%%=:\;\"
            \"asr %%q1,%%q0\;\"
            \"rrc %%t1,%%t0\;\"
            \"rrc %%d1,%%d0\;\"
            \"rrc %%1,%%0\;\"
            \"dec %%3,%%3\;\"
            \"bra nz,.LB%%=\");
        }
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%1,#%%2,%%0\;\"
           \"sl %%d1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%d1,#%%2,%%d0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"lsr %%t1,#%%2,%%t0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%t0,%%t0\",
           16 - INTVAL(operands[2]), 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%1,#%%2,%%0\;\"
           \"sl %%d1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%d1,#%%2,%%d0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\",
           16 - INTVAL(operands[2]), 
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
          \"lsr %%1,#%%2,%%0\;\"
          \"sl %%d1,#%d,%%3\;\"
          \"ior %%3,%%0, %%0\;\",
           16 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 16) {
    switch (results) {
      case 4:
      case 3:
        f += sprintf(f,
          \"mov %%d1,%%0\;\"
          \"mov %%t1,%%d0\;\"
          \"mul.su %%q1,#1,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mov %%d1,%%0\;\"
          \"mov %%t1,%%d0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov %%d1,%%0\");
        break;
     }
     if ((unsigned)f - (unsigned)buffer > 256)
        error(\"Unexpected error during assembly generation\");
     return buffer;
  } else if (INTVAL(operands[2]) < 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"asr %%q1,#%d,%%t0\;\"
           \"asr %%q1,#15,%%q0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\"
           \"asr %%q1,#%d,%%t0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"lsr %%t1,#%d,%%d0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%d0,%%d0\;\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
           \"lsr %%d1,#%d,%%0\;\"
           \"sl %%t1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\",
           INTVAL(operands[2]) - 16,
           32 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 16);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 32) {
    switch (results) {
      case 4:
        f += sprintf(f,
          \"mov.d %%t1,%%0\;\"
          \"asr %%t1,#15,%%t0\;\"
          \"asr %%t1,#15,%%q0\");
        break;
      case 3:
        f += sprintf(f,
          \"mov %%t1,%%0\;\"
          \"asr %%t1,#15,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mov.d %%t1,%%0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov.w %%t1,%%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256)
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 48) {
    switch (results) {
      case 4:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"asr %%q1,#%d,%%d0\;\"
           \"asr %%q1,#15,%%t0\;\"
           \"asr %%q1,#15,%%q0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 3:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"asr %%q1,#%d,%%d0\;\"
           \"asr %%q1,#15,%%t0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 2:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\;\"
           \"asr %%q1,#%d,%%d0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]),
           INTVAL(operands[2]) - 32);
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
      case 1:
        f += sprintf(f,
           \"lsr %%t1,#%d,%%0\;\"
           \"sl %%q1,#%d,%%3\;\"
           \"ior %%3,%%0,%%0\",
           INTVAL(operands[2]) - 32,
           48 - INTVAL(operands[2]));
        if ((unsigned)f - (unsigned)buffer > 256) 
          error(\"Unexpected error during assembly generation\");
        return buffer;
    }
  } else if (INTVAL(operands[2]) == 48) {
    switch (results) {
      case 4:
        f += sprintf(f,
          \"mul.su %%q1,#1,%%0\;\"
          \"asr %%q1,#15,%%t0\;\"
          \"asr %%q1,#15,%%q0\");
        break;
      case 3:
        f += sprintf(f,
          \"mul.su %%q1,#1,%%0\;\"
          \"asr %%q1,#15,%%t0\");
        break;
      case 2:
        f += sprintf(f,
          \"mul.su %%q1,#1,%%0\");
        break;
      case 1:
        f += sprintf(f,
          \"mov %%q1,%%0\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256)
      error(\"Unexpected error during assembly generation\");
    return buffer;
  } else if (INTVAL(operands[2]) < 64) {
    f += sprintf(f, \"asr %%q1,#%d,%%0\", INTVAL(operands[2])-48);
    switch (results) {
      case 4: 
        f += sprintf(f,
          \"\;asr %%q1,#15,%%d0\;\"
          \"asr %%q1,#15,%%t0\;\"
          \"asr %%q1,#15,%%q0\;\");
        break;
      case 3:
        f += sprintf(f,
          \"\;asr %%q1,#15,%%d0\;\"
          \"asr %%q1,#15,%%t0\;\");
        break;
      case 2:
        f += sprintf(f,
          \"\;asr %%q1,#15,%%d0\;\");
        break;
    }
    if ((unsigned)f - (unsigned)buffer > 256) 
      error(\"Unexpected error during assembly generation\");
    return buffer;
  }
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

(define_insn "ashrdi3_reg"
  [(set (match_operand:DI 0 "pic30_register_operand"             "=r")
        (ashiftrt:DI (match_operand:DI 1 "pic30_register_operand" "0")
                     (match_operand:HI 2 "pic30_register_operand" "r")))
		     (clobber (match_scratch:HI 3          "=2"))]
  ""
  "*
{
      return( 
#if (1)
      	      \".set ___BP___,0\n\"
#endif
              \".LB%=:\;\"
              \"dec %2,%2\;\"
              \"bra n,.LE%=\;\"
              \"asr %q1,%q0\;\"
              \"rrc %t1,%t0\;\"
              \"rrc %d1,%d0\;\"
              \"rrc %1,%0\;\"
              \"bra .LB%=\n\"
              \".LE%=:\");
}"
  [
   (set_attr "cc" "clobber")
   (set_attr "type" "def")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; rotate instructions
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; rotate left
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; QImode ;;
;;;;;;;;;;;;

(define_insn "*rotlqi3_one_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (rotate:QI 
           (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
           (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rlnc.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotlqi3_one_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (rotate:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:QI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rlnc.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotlqi3_sfr"
  [(set (match_operand:QI 0 "pic30_wreg_or_near_operand"   "=a,U")
        (rotate:QI (match_operand:QI 1 "pic30_near_operand" "U,0")
                   (match_operand:QI 2 "pic30_I_operand"    "I,I")))]
  ""
  "@
    rlnc.b %1,WREG
    rlnc.b %1"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;;;;;;;;;;;;
;; HImode ;;
;;;;;;;;;;;;

(define_insn "*rotlhi3_one_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (rotate:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                   (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rlnc %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotlhi3_one_APSV"
  [(set (match_operand:HI 0 "pic30_mode2_operand"           "=r<>,r<>,R,R")
        (rotate:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rlnc %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotlhi3_sfr"
  [(set (match_operand:HI 0 "pic30_wreg_or_near_operand"   "=a,U")
        (rotate:HI (match_operand:HI 1 "pic30_near_operand" "U,0")
                   (match_operand:HI 2 "pic30_I_operand"    "I,I")))]
  ""
  "@
    rlnc %1,WREG
    rlnc %1"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; rotate right
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;
;; QImode ;;
;;;;;;;;;;;;

(define_insn "*rotrqi3_one_DATA"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (rotatert:QI (match_operand:QI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                   (match_operand:QI 2 "pic30_I_operand"       "I,  I,  I,I")))]
  ""
  "rrnc.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotrqi3_one_APSV"
  [(set (match_operand:QI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (rotatert:QI 
           (match_operand:QI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:QI 2 "pic30_I_operand"       "I,  I,  I,I")))]
  ""
  "rrnc.b %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotrqi3_sfr"
  [(set (match_operand:QI 0 "pic30_wreg_or_near_operand"     "=a,U")
        (rotatert:QI (match_operand:QI 1 "pic30_near_operand" "U,0")
                   (match_operand:QI 2 "pic30_I_operand"      "I,I")))]
  ""
  "@
    rrnc.b %1,WREG
    rrnc.b %1"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;;;;;;;;;;;;
;; HImode ;;
;;;;;;;;;;;;

(define_insn "*rotrhi3_one_DATA"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (rotatert:HI (match_operand:HI 1 "pic30_mode2_operand" "r,  R<>,r,R<>")
                     (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rrnc %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotrhi3_one"
  [(set (match_operand:HI 0 "pic30_mode2_operand"             "=r<>,r<>,R,R")
        (rotatert:HI 
           (match_operand:HI 1 "pic30_mode2_APSV_operand" "r,  R<>,r,R<>")
           (match_operand:HI 2 "pic30_I_operand"     "I,  I,  I,I")))]
  ""
  "rrnc %1,%0"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,defuse,etc,use")
  ]
)

(define_insn "*rotrhi3_sfr"
  [(set (match_operand:HI 0 "pic30_wreg_or_near_operand"     "=a,U")
        (rotatert:HI (match_operand:HI 1 "pic30_near_operand" "U,0")
                   (match_operand:HI 2 "pic30_I_operand"      "I,I")))]
  ""
  "@
    rrnc %1,WREG
    rrnc %1"
  [
   (set_attr "cc" "math")
   (set_attr "type" "def,etc")
  ]
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Special cases of bit-field insns which we should
;; recognize in preference to the general case.
;; These handle aligned 8-bit and 16-bit fields,
;; which can usually be done with move instructions.
;    dsPIC30: t.b.d.
;********************

;; Bit field instructions, general cases.
;; "o,d" constraint causes a nonoffsettable memref to match the "o"
;; so that its address is reloaded.

;; (define_insn "extv" ...

;; (define_insn "extzv" ...

;; (define_insn "insv" ...

(define_expand "insv"
  [(set (zero_extract:HI
          (match_operand    0 "pic30_mode2_or_near_operand" "+RrU")
          (match_operand:HI 1 "immediate_operand"           "i")
          (match_operand:HI 2 "immediate_operand"           "i"))
        (match_operand:HI 3 "immediate_operand"             "i"))]
  ""
  "
{ int n;
  int mode;

  n = 4;
  mode = GET_MODE(operands[0]);
  if (pic30_volatile_operand(operands[0], mode)) n = 1;
  if ((INTVAL(operands[1]) <= n) && (GET_CODE(operands[3]) == CONST_INT) &&
      /* pic30_reg_or_near_operand is already called, but if optimization is
         on, constant addresses are forced into a register so that 'cse can
         get a chance to see them' (in fn memory_address)... thats okay, cse
         will turn them back to constants we hope */
      (pic30_reg_or_near_operand(operands[0],VOIDmode)) &&
      ((mode == HImode) || (mode == QImode)))
  {  int mask;
     rtx mask_rtx;
     int i;
     rtx clobber = 0;
     rtx modifybit = operands[0];

     /* actually operand [3] does not necessarily fit into our bitfield...
        *SIGH* */

     for (i = 0; i < INTVAL(operands[1]); i++) {
       mask =  (1 << (INTVAL(operands[2]) +i));
       if ((INTVAL(operands[3]) & (1 << i)) == 0) {
          if (pic30_mode2_operand(operands[0],VOIDmode)) {
            mask_rtx = gen_rtx_CONST_INT(HImode, ~mask);
            emit(gen_bitclrhi(operands[0], operands[0], mask_rtx));
          } else {
            if (mode == HImode) {
              mask_rtx = gen_rtx_CONST_INT(HImode, ~mask);
              emit(gen_bitclrhi_sfr(operands[0], mask_rtx));
            } else if (mode == QImode) {
              if (mask > 0x80) {
                FAIL;
                break;
              }
              mask_rtx = gen_rtx_CONST_INT(HImode, ~mask);
              emit(gen_bitclrqi_sfr(modifybit, mask_rtx));
            }
          }
       } else {
          if (pic30_mode2_operand(operands[0],VOIDmode)) {
            mask_rtx = gen_rtx_CONST_INT(HImode, mask);
            emit(gen_bitsethi(operands[0], operands[0], mask_rtx));
          } else {
            if (mode == HImode) {
              mask_rtx = gen_rtx_CONST_INT(HImode, mask);
              emit(gen_bitsethi_sfr(operands[0], mask_rtx));
            } else {
              if (mask > 0x80) {
                FAIL;
                break;
              }
              mask_rtx = gen_rtx_CONST_INT(HImode, mask);
              emit(gen_bitsetqi_sfr(modifybit, mask_rtx));
            }
          }
       }
     }
     DONE;
  }
  else FAIL;

}
")

;; Now recognize bit field insns that operate on registers
;; (or at least were intended to do so).
;[unnamed only]

;; Special patterns for optimizing bit-field instructions.
;**************************************

; cc status test ops n.a. on dsPIC30 ......... e.g. "sleu" on 68k:
;  [(set (match_operand:QI 0 "pic30_general_operand" "=d")
;        (leu (cc0) (const_int 0)))]
;  ""
;  "* cc_status = cc_prev_status;
;     return \"sls %0\"; ")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Basic conditional jump instructions.
;; Every machine description must have a named pattern for each of the
;; conditional branch names "bcond"
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "beq"
  [(set (pc)
        (if_then_else (eq (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(EQ, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bne"
  [(set (pc)
        (if_then_else (ne (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(NE, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bgt"
  [(set (pc)
        (if_then_else (gt (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(GT, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "blt"
  [(set (pc)
        (if_then_else (lt (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(LT, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bge"
  [(set (pc)
        (if_then_else (ge (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(GE, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "ble"
  [(set (pc)
        (if_then_else (le (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(LE, operands[0]));"
  [(set_attr "cc" "unchanged")])


; unsigned branches:

(define_insn "bgtu"
  [(set (pc)
        (if_then_else (gtu (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(GTU, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bltu"
  [(set (pc)
        (if_then_else (ltu (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(LTU, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bgeu"
  [(set (pc)
        (if_then_else (geu (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(GEU, operands[0]));"
  [(set_attr "cc" "unchanged")])

(define_insn "bleu"
  [(set (pc)
        (if_then_else (leu (cc0)
                          (const_int 0))
                      (label_ref (match_operand 0 "" ""))
                      (pc)))]
  ""
  "* return(pic30_conditional_branch(LEU, operands[0]));"
  [(set_attr "cc" "unchanged")])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Every machine description must have an anonymous pattern for each of
;; the possible reverse-conditional branches.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "*bccreverse"
  [(set (pc)
        (if_then_else (match_operator 1 "comparison_operator" 
			[(cc0) (const_int 0)])
                      (pc)
                      (label_ref (match_operand 0 "" ""))))]
  ""
  "*
{
	return(pic30_conditional_branch(
		reverse_condition(GET_CODE(operands[1])), operands[0]));
}"
  [(set_attr "cc" "unchanged")])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; An instruction to jump to an address which is operand zero.
;; This pattern name is mandatory on all machines.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "indirect_jump"
 [(set (pc) (match_operand:HI 0 "pic30_register_operand" "r"))]
 ""
 "goto %0")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Instruction to jump to a variable address. This is a low-level capability
;; which can be used to implement a dispatch table when there is no 'casesi'
;; pattern. This pattern requires two operands: the address or offset, and a
;; label which should immediately precede the jump table. If the macro
;; CASE_VECTOR_PC_RELATIVE evaluates to a nonzero value then the first operand
;; is an offset which counts from the address of the table; otherwise, it is
;; an absolute address to jump to. In either case, the first operand has mode
;; Pmode. The 'tablejump' insn is always the last insn before the jump table
;; it uses. Its assembler code normally has no need to use the second operand,
;; but you should incorporate it in the RTL pattern so that the jump optimizer
;; will not delete the table as unreachable code.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_insn "tablejump"
  [(set (pc) (match_operand:HI 0 "pic30_register_operand" "r"))
   (use (label_ref (match_operand 1 "" "")))]
  ""
  "bra %0")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Subroutines of "casesi".
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; Operand 0 is index
;; operand 1 is the minimum bound
;; operand 2 is the maximum bound - minimum bound + 1
;; operand 3 is CODE_LABEL for the table;
;; operand 4 is the CODE_LABEL to go to if index out of range.

(define_expand "casesi"
  [(match_operand:SI 0 "pic30_general_operand" "")
   (match_operand:SI 1 "immediate_operand" "")
   (match_operand:SI 2 "immediate_operand" "")
   (match_operand 3 "" "")
   (match_operand 4 "" "")]
  ""
  "
{
  if (INTVAL(operands[2]) >= 32767) {
    error(\"Too many case statements in switch table\");
    FAIL;
  }
  if (operands[1] != const0_rtx) {
    rtx reg = gen_reg_rtx(SImode);

    operands[1] = GEN_INT(-INTVAL(operands[1]));
    operands[1] = force_reg(SImode, operands[1]);
    emit_insn(gen_addsi3(reg, operands[0], operands[1]));
    operands[0] = reg;
  }
  operands[0] = force_reg(SImode, operands[0]);
  operands[2] = force_reg(SImode, operands[2]);
  emit_insn(gen_cmpsi(operands[0], operands[2]));
  emit_jump_insn(gen_bgtu(operands[4]));
  emit_jump_insn(gen_casesi0(operands[0], operands[3]));
  DONE;
}")

(define_insn "casesi0"
  [(set (pc) (plus:SI
     (mem:SI (plus:SI (pc)
               (match_operand:SI 0 "pic30_mode2_operand" "r,R")))
     (label_ref (match_operand 1 "" ""))))
     (clobber (match_scratch:HI 2  "=X,r"))
     (clobber (match_dup 0))]
  ""
  "*
{
  /*
  ** See if there is anything between us and the jump table
  ** If we could be sure there never was, then the 'clobber'
  ** of operand[0] could be removed.
  */
  register rtx p;
  int fDisjoint = FALSE;
  for (p = NEXT_INSN (insn); p != operands[1]; p = NEXT_INSN (p))
  {
    if (INSN_P(p)) {
      fDisjoint = TRUE;
      break;
    }
  }
  if (fDisjoint)
  {

    if (which_alternative == 0)
      return(\"add #(%1-$)/4,%0\;\"
             \"bra %0\");
    else
      return(\"mov [%0], %2\;\"
             \"add #(%1-$)/4,%2\;\"
             \"bra %2\");
  }
  else
  {
    if (which_alternative == 0)
      return(\"bra %0\");
    else
      return(\"mov %0, %2\;\"
             \"bra %2\");
  }
}")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Unconditional jump
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_insn "jump"
  [(set (pc)
        (label_ref (match_operand 0 "" "")))]
  ""
  "bra %0"
  [(set_attr "cc" "unchanged")])

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; Call subroutine, returning value in operand 0
;; (which must be a hard register).
;; Operand 1 is the function to call
;; Operand 2 is the number of bytes of arguments pushed
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
(define_expand "call_value"
  [(set (match_operand 0 "pic30_register_operand"        "=r,r ,r")
        (call (match_operand    1 "memory_operand"  "R,QS,m")
              (match_operand:HI 2 "pic30_general_operand" "")))]
  ;; Operand 2 not really used for dsPIC30.
  ""
  "
{
     char *security;
     int slot = 0;
     int set_psv;
     rtx sfr;
     rtx psv_page;
 
     security = pic30_boot_secure_access(operands[1],&slot,&set_psv);
     if (set_psv == pic30_set_on_call) {
       sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
       psv_page = gen_reg_rtx(HImode);
       emit_insn(gen_save_const_psv(psv_page, sfr));
       emit(gen_set_psv(psv_page));
     }
     emit(gen_call_value_helper(operands[0], operands[1], operands[2]));
     if (set_psv) {
       if (lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                            DECL_ATTRIBUTES(current_function_decl))) {
         sfr = gen_rtx_SYMBOL_REF(HImode,\"_bootconst_psvpage\");
       } else if (lookup_attribute(IDENTIFIER_POINTER(pic30_identSecure[0]),
                                   DECL_ATTRIBUTES(current_function_decl))) {
         sfr = gen_rtx_SYMBOL_REF(HImode,\"_secureconst_psvpage\");
       } else DONE;
       psv_page = gen_reg_rtx(HImode);
#if 1
       emit_insn(gen_save_const_psv(psv_page, sfr));
       emit(gen_set_psv(psv_page));
#else
       emit_insn(gen_set_const_psv(sfr));
#endif
     }    
     DONE;
}
  "
)

(define_insn "save_const_psv"
  [(set (match_operand:HI 0 "pic30_register_operand"        "=r")
        (match_operand:HI 1 "pic30_symbolic_address_operand" "g"))]
  ""
  "mov #%1,%0"
)

(define_insn "call_value_helper"
  [(set (match_operand 0 "pic30_register_operand"        "=r,r ,r")
        (call (match_operand    1 "memory_operand"  "R,QS,m")
              (match_operand:HI 2 "pic30_general_operand" "")))]
  ;; Operand 2 not really used for dsPIC30.
  "(pic30_check_for_conversion(insn))"
  "*
   static char szInsn[88];
   char *security;
   int slot = 0;
   int set_psv;
   char *this_insn = szInsn;

   pic30_clear_fn_list = 1;
   pic30_valid_call_address_operand(operands[0], Pmode);
#ifdef __C30_BETA__
   if (TARGET_ABI_CHECK) {
    this_insn += sprintf(this_insn,\"call __c30_abi_push\n\t\");
   }
#endif
   switch (which_alternative)
   {
     case 0:
       this_insn += sprintf(this_insn,\"call %s\", reg_names[REGNO(XEXP(operands[1],0))]);
       break;
     case 1:
       /*
        ** Casts of &(int x) to function ptrs, etc.
        */
       error(\"invalid function call\");
     default:
       security = pic30_boot_secure_access(operands[1],&slot,&set_psv);
       if (security) {
         this_insn += sprintf(this_insn, \"%s %s(%d)\",
                         pic30_near_function_p(operands[1]) ? 
                           \"rcall\" : \"call\", security,slot);
       } else 
         this_insn += sprintf(this_insn, \"%s %%1\",
                         pic30_near_function_p(operands[1]) ? 
                           \"rcall\" : \"call\");
         break;
   }
#ifdef __C30_BETA__
   if (TARGET_ABI_CHECK) {
     this_insn += sprintf(this_insn,\"\n\tcall __c30_abi_pop\");
   }
#endif
   return szInsn ;
")

;; Call subroutine with no return value.
(define_expand "call_void"
  [ (call (match_operand:QI 0 "memory_operand"  "R,QS,m")
          (match_operand:HI 1 "pic30_general_operand" ""))]
  ;; Operand 1 not really used for dsPIC30.
  ""
  "
{
     char *security;
     int slot = 0;
     int set_psv;
     rtx sfr;
     rtx psv_page;
 
     security = pic30_boot_secure_access(operands[0],&slot,&set_psv);
     if (set_psv == pic30_set_on_call) {
       sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
       psv_page = gen_reg_rtx(HImode);
       emit_insn(gen_save_const_psv(psv_page, sfr));
       emit(gen_set_psv(psv_page));
     }
     emit(gen_call_void_helper(operands[0], operands[1]));
     if (set_psv) {
       if (lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                            DECL_ATTRIBUTES(current_function_decl))) {
         sfr = gen_rtx_SYMBOL_REF(HImode,\"_bootconst_psvpage\");
       } else if (lookup_attribute(IDENTIFIER_POINTER(pic30_identSecure[0]),
                                   DECL_ATTRIBUTES(current_function_decl))) {
         sfr = gen_rtx_SYMBOL_REF(HImode,\"_secureconst_psvpage\");
       } else DONE;
       psv_page = gen_reg_rtx(HImode);
       emit_insn(gen_save_const_psv(psv_page, sfr));
       emit(gen_set_psv(psv_page));
     }    
     DONE;
}
  "
)

(define_insn "call_void_helper"
  [(call (match_operand:QI 0 "memory_operand" "R,QS,m")
         (match_operand:HI 1 "pic30_general_operand" ""))]
  "(pic30_check_for_conversion(insn))"
  "*
{
   static char szInsn[88];
   char *security;
   int slot = 0;
   char *this_insn = szInsn;

   pic30_clear_fn_list = 1;
   pic30_valid_call_address_operand(operands[0], Pmode);
#ifdef __C30_BETA__
   if (TARGET_ABI_CHECK) {
    this_insn += sprintf(this_insn,\"call __c30_abi_push\n\t\");
   }
#endif
   switch (which_alternative)
   {
     case 0:
       this_insn += sprintf(this_insn,\"call %s\", reg_names[REGNO(XEXP(operands[0],0))]);
       break;
     case 1:
       /*
        ** Casts of &(int x) to function ptrs, etc.
        */
       error(\"invalid function call\");
     default:
       security = pic30_boot_secure_access(operands[0],&slot,0);
       if (security) {
         this_insn += sprintf(this_insn, \"%s %s(%d)\",
                         pic30_near_function_p(operands[0]) ? 
                           \"rcall\" : \"call\",
                         security, slot);
       } else
         this_insn += sprintf(this_insn, \"%s %%0\",
                         pic30_near_function_p(operands[0]) ? 
                           \"rcall\" : \"call\");
         break;
   }
#ifdef __C30_BETA__
   if (TARGET_ABI_CHECK) {
     this_insn += sprintf(this_insn,\"\n\tcall __c30_abi_pop\");
   }
#endif
   return szInsn ;
}")

;; Call subroutine with no return value.
;; This handles intrinsics, such as bcopy.

(define_expand "call"
 [(call (match_operand:QI 0 "" "")
        (match_operand:HI 1 "" ""))]
 ""
 "
{
  char *security;
  int slot = 0;
  int set_psv;
  rtx sfr;
  rtx psv_page;

  if (GET_CODE (operands[0]) == MEM && 
      !pic30_call_address_operand (XEXP (operands[0], 0), Pmode))
    operands[0] = gen_rtx_MEM (GET_MODE (operands[0]),
			       force_reg (Pmode, XEXP (operands[0], 0)));
  security = pic30_boot_secure_access(operands[0],&slot,&set_psv);
  if (set_psv == pic30_set_on_call) {
    sfr = gen_rtx_SYMBOL_REF(HImode,\"_const_psvpage\");
    psv_page = gen_reg_rtx(HImode);
    emit_insn(gen_save_const_psv(psv_page, sfr));
    emit(gen_set_psv(psv_page));
  }
  emit(gen_call_void_helper(operands[0], operands[1]));
  if (set_psv) {
    if (lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                         DECL_ATTRIBUTES(current_function_decl))) {
      sfr = gen_rtx_SYMBOL_REF(HImode,\"_bootconst_psvpage\");
    } else if (lookup_attribute(IDENTIFIER_POINTER(pic30_identSecure[0]),
                                DECL_ATTRIBUTES(current_function_decl))) {
      sfr = gen_rtx_SYMBOL_REF(HImode,\"_secureconst_psvpage\");
    } else DONE;
    psv_page = gen_reg_rtx(HImode);
    emit_insn(gen_save_const_psv(psv_page, sfr));
    emit(gen_set_psv(psv_page));
  }   
  DONE;
}")

(define_insn "*call"
  [(call (mem:QI (match_operand:QI 0 "pic30_call_address_operand" ""))
         (match_operand:HI 1 "pic30_general_operand" ""))]
  "(pic30_check_for_conversion(insn))"
  "*
  {
    static char szInsn[88];
    char *security;
    int slot = 0;
    char *insn = szInsn;

    pic30_clear_fn_list = 1;
    pic30_valid_call_address_operand(operands[0], Pmode);
    security = pic30_boot_secure_access(operands[0],&slot,0);
#ifdef __C30_BETA__
    if (TARGET_ABI_CHECK) {
     insn += sprintf(insn,\"call __c30_abi_push\n\t\");
    }
#endif
    if (security) {
      insn += sprintf(insn, \"call %s(%d)\",security, slot);
    } else sprintf(szInsn,\"call %0\");
#ifdef __C30_BETA__
    if (TARGET_ABI_CHECK) {
      insn += sprintf(insn,\"\n\tcall __c30_abi_pop\");
    }
#endif
    return szInsn;
  }"
)

;;
;; return
;;
(define_insn "return"
  [(return)]
  "pic30_null_epilogue_p()"
  "*
{
	pic30_set_function_return(TRUE);

	return(\"return\");
}"
  [(set_attr "cc" "clobber")])

(define_insn "return_from_epilogue"
  [(return)]
  "!pic30_null_epilogue_p() && reload_completed"
  "*
  { extern tree current_function_decl;
    if (pic30_interrupt_function_p(current_function_decl))
    {
      return(\"retfie\");
    }
    else if (pic30_noreturn_function(current_function_decl))
    {
      return(\"reset\");
    }
    else
    {
      return(\"return\");
    }
  }"
  [(set_attr "cc" "clobber")]
)

;;
;; link
;;
(define_insn "lnk"
  [(set (reg:HI SPREG)
        (match_operand 0 "immediate_operand" "i"))
   (clobber (reg:HI FPREG))
   (use (reg:HI FPREG))
   (use (reg:HI SPREG))
  ]
  "reload_completed"
  "lnk #%0"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;;
;; unlink
;;
(define_insn "ulnk"
  [(set (reg:HI SPREG)
        (reg:HI FPREG))
   (clobber (reg:HI FPREG))]
  "reload_completed"
  "ulnk"
  [
   (set_attr "cc" "change0")
   (set_attr "type" "def")
  ]
)

;;
;; disi
;;
(define_insn "disi"
  [(unspec_volatile [(match_operand 0 "immediate_operand" "i")] UNSPECV_DISI)]
  ""
  "disi #%0")



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; prologue/epilogue
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_expand "prologue"
  [(const_int 1)]
  ""                           
  "
  {
  	pic30_expand_prologue();
	DONE;
  }"
)

(define_expand "epilogue"
  [(const_int 1)]
  ""
  "
  {
  	pic30_expand_epilogue();
	DONE;
  }"
)

;; UNSPEC_VOLATILE is considered to use and clobber all hard registers and
;; all of memory.  This blocks insns from being moved across this point.

(define_insn "blockage"
  [(unspec_volatile [(const_int 0)] UNSPECV_BLOCKAGE)]
  ""
  "")

(define_insn "pa"
  [(unspec_volatile [(match_operand 0 "immediate_operand" "i")] UNSPECV_PA)]
  ""
  ".set ___PA___,%0")

(define_insn "iv"
  [(unspec_volatile [(match_operand 0 "immediate_operand" "i")] UNSPECV_IV)]
  ""
  {
   static char szInsn[96];
	char szVector[32];
	int nVectorID;
			
	nVectorID = INTVAL(operands[0]);
	if (nVectorID < 0)
	{
		nVectorID = -nVectorID-1;
		sprintf(szVector,"__AltInterrupt%d",nVectorID);
	}
	else
	{
		sprintf(szVector, "__Interrupt%d", nVectorID);
	}
	sprintf(szInsn, ".global\t%s\n%s:", szVector, szVector);

	return(szInsn);
  }
)
(define_insn "pp"
  [(unspec_volatile [(const_int 0)] UNSPECV_PP)]
  ""
  {
  	return(pic30_interrupt_preprologue());
  }
)

(define_insn "write_oscconl"
  [(set (match_operand:HI 0 "pic30_register_operand" "=&r")
        (unspec_volatile [
                     (match_operand 1 "pic30_register_operand" "r")
                     (match_operand 2 "pic30_register_operand" "r")
                     (match_operand 3 "pic30_register_operand" "r")]
           UNSPECV_WRITEOSCCONL)
   )]
  ""
  "mov #_OSCCON,%0\;mov.b %1,[%0]\;mov.b %2,[%0]\;mov.b %3,[%0]"
)

(define_insn "write_oscconh"
  [(set (match_operand:HI 0 "pic30_register_operand" "=&r")
        (unspec_volatile [
                     (match_operand 1 "pic30_register_operand" "r")
                     (match_operand 2 "pic30_register_operand" "r")
                     (match_operand 3 "pic30_register_operand" "r")]
    UNSPECV_WRITEOSCCONH)
   )]
  ""
  "mov #_OSCCON+1,%0\;mov.b %1,[%0]\;mov.b %2,[%0]\;mov.b %3,[%0]"
)

(define_insn "write_disicnt"
  [ (unspec_volatile [
      (match_operand 0 "pic30_register_operand" "r")] UNSPECV_WRITEDISICNT)
  ]
  ""
  "mov %0,_DISICNT\;mov w0,_WREG0"
)

(define_insn "write_pwmsfr"
  [ (unspec_volatile [
      (match_operand:HI 0 "pic30_near_operand"     "U,U")
      (match_operand:HI 1 "pic30_register_operand" "a,r")
      (match_operand:HI 2 "pic30_register_operand" "r,r")
      (match_operand:HI 3 "pic30_register_operand" "r,r")
      (match_operand:HI 4 "pic30_R_operand"        "R,R")
    ] UNSPECV_WRITEPWMCON)
    (clobber (match_scratch: HI  5                "=X,r"))
  ]
  ""
  "@
   mov %3,%4\;mov %2,%4\;mov %1,%0
   mov #%0,%5\;mov %3,%4\;mov %2,%4\;mov %1,[%5]"
 )

(define_insn "write_nvm_secure"
  [
    (set (match_operand:HI 0 "pic30_register_operand" "+r")
         (unspec_volatile [ (const_int 0) ] UNSPECV_WRITENVM))
    (set (match_operand:HI 1 "pic30_register_operand" "+r")
         (unspec_volatile [ (const_int 1) ] UNSPECV_WRITENVM)) ]
  ""
  "mov %0,_NVMKEY\;mov %1,_NVMKEY\;bset _NVMCON,#15\;nop\;nop\;clr %0\;clr %1"
)

(define_insn "write_nvm"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (unspec_volatile [ (const_int 0) ] UNSPECV_WRITENVM))]
  ""
  "mov #0x55,%0\;mov %0,_NVMKEY\;mov #0xAA,%0\;mov %0,_NVMKEY\;bset _NVMCON,#15\;nop\;nop"
)

(define_insn "write_rtcwen"
  [(set (match_operand:HI 0 "pic30_register_operand" "=&r")
        (unspec_volatile [
           (match_operand:HI 1 "pic30_register_operand" "0")
         ] UNSPECV_WRITERTCWEN))]
  ""
  "mov #0x55,%0\;mov %0,_NVMKEY\;mov #0xAA,%0\;mov %0,_NVMKEY\;bset _RCFGCAL,#13"
)

(define_insn "write_cryotp"
  [(set (match_operand:HI 0 "pic30_register_operand" "=&r")
        (unspec_volatile [ (const_int 0) ] UNSPECV_WRITECRTOTP))]
  ""
  "mov #0x55,%0\;mov %0,_NVMKEY\;mov #0xAA,%0\;mov %0,_NVMKEY\;nop\;bset _CRYOTP,#0"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; nop
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define_insn "bifnop"
  [(unspec_volatile [(const_int 0)] UNSPECV_NOP)]
  ""
  "nop"
  [(set_attr "cc" "unchanged")])

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop"
  [(set_attr "cc" "unchanged")])

;
;; misc
;

(define_insn "section_begin"
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (unspec:SI [ 
          (match_operand 1 "immediate_operand" "i")
        ] UNSPEC_SECTION_BEGIN))]
  ""
  "*
  { static char *buffer = 0;
    static buffer_size;
    int len;

    tree t = (tree)(INTVAL(operands[1]));
    char *section_name = TREE_STRING_POINTER(t);
    len = sizeof (\"mov #.startof.(),%0\;mov #.startof_hi.(),%d0\");
    len += strlen(section_name)*2;
    len++;
    if (buffer_size < len) {
      if (buffer) free(buffer);
      buffer = xmalloc(len+50);
      buffer_size = len+50;
    }
    sprintf(buffer, \"mov #.startof.(%s),%%0\;mov #.startof_hi.(%s),%%d0\",
            section_name, section_name);
    return buffer;
  }"
)

(define_insn "section_end"
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (unspec:SI [ 
          (match_operand 1 "immediate_operand" "i")
        ] UNSPEC_SECTION_END))]
  ""
  "*
  { static char *buffer = 0;
    static buffer_size;
    int len;

    tree t = (tree)(INTVAL(operands[1]));
    char *section_name = TREE_STRING_POINTER(t);
    len = sizeof (\"mov #.endof.(),%0\;mov #.endof_hi.(),%d0\");
    len += strlen(section_name)*2;
    len++;
    if (buffer_size < len) {
      if (buffer) free(buffer);
      buffer = xmalloc(len+50);
      buffer_size = len+50;
    }
    sprintf(buffer, \"mov #.endof.(%s),%%0\;mov #.endof_hi.(%s),%%d0\",
            section_name, section_name);
    return buffer;
  }"

)

(define_insn "section_size"
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (unspec:SI [ 
          (match_operand 1 "immediate_operand" "i")
        ] UNSPEC_SECTION_SIZE))]
  ""
  "*
  { static char *buffer = 0;
    static buffer_size;
    int len;

    tree t = (tree)(INTVAL(operands[1]));
    char *section_name = TREE_STRING_POINTER(t);
    len = sizeof (\"mov #.sizeof.(),%0\;mov #.sizeof_hi.(),%d0\");
    len += strlen(section_name)*2;
    len++;
    if (buffer_size < len) {
      if (buffer) free(buffer);
      buffer = xmalloc(len+50);
      buffer_size = len+50;
    }
    sprintf(buffer, \"mov #.sizeof.(%s),%%0\;mov #.sizeof_hi.(%s),%%d0\",
            section_name, section_name);
    return buffer;
  }"
)

; define [sg]et_isr_state as an expand, in so that we can generate less
;   code if we don't actually 'use' the value (apart from to re-set it)
;   if unless we say if (__builtin_get_isr_state() == n) there is no need
;   to shift the value

(define_insn "get_isr_state_helper"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (unspec:HI [ (const_int 0) ] UNSPEC_GET_ISR_STATE))]
  ""
  "mov _SR,%0"
)

(define_insn "extract_gie"
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (unspec:HI [ (match_dup 0) ] UNSPEC_EXTRACT_GIE))]
  ""
  "btsc _INTCON2,#15\;bset %0,#3"
)

(define_expand "get_isr_state"
 [(set (match_operand:HI 0 "pic30_register_operand" "=r")
       (unspec:HI [(const_int 0)] UNSPEC_GET_ISR_STATE))]
 ""
 "{ rtx reg = operands[0];

    if (!pic30_register_operand(operands[0], HImode)) {
      reg = force_reg (GET_MODE(operands[0]), operands[0]);
    }
    emit_insn(
      gen_get_isr_state_helper(reg)
    );
    emit_insn(
      gen_ashrhi3(reg, reg, GEN_INT(5))
    );
    emit_insn(
      gen_andhi3(reg, reg, GEN_INT(7))
    );
    if (pic30_device_has_gie()) {
      emit_insn(
        gen_extract_gie(reg)
      );
    }
    if (reg != operands[0]) {
      emit_move_insn(operands[0], reg);
    }
    DONE;
  }")

(define_insn "set_isr_state_helper"
  [(unspec_volatile [
      (match_operand:HI 0 "pic30_register_operand" "r")
    ] UNSPECV_SET_ISR_STATE)
   (clobber (match_scratch:HI 1                    "=r"))
   (clobber (match_scratch:HI 2                    "=r"))]
  ""
  "mov _SR,%1\;and.b #0x1F,%1\;ior %1,%0,%1\;mov %1,_SR"
  [(set_attr "cc" "clobber")]
)

(define_insn "insert_gie"
  [(unspec_volatile [ 
      (match_operand:HI 0 "pic30_register_operand" "r")] UNSPEC_INSERT_GIE)
   (clobber (match_scratch:HI 1                    "=r"))]
  ""
  "mov _INTCON2,%1\;bclr %1,#15\;btsc %0,#3\;bset %1,#15\;mov %1,_INTCON2"
)


(define_expand "set_isr_state"
 [(unspec_volatile [ 
     (match_operand:HI 0 "pic30_register_operand" "=r")] UNSPECV_SET_ISR_STATE)]
 ""
 "{ rtx reg = operands[0];
    rtx reg2 = gen_reg_rtx(HImode);

    if (!pic30_register_operand(operands[0], HImode)) {
      reg = force_reg (GET_MODE(operands[0]), operands[0]);
    }
    emit_insn(
      gen_andhi3(reg2, reg, GEN_INT(7))
    );
    emit_insn(
      gen_ashlhi3(reg2, reg2, GEN_INT(5))
    );
    emit_insn(
      gen_set_isr_state_helper(reg2)
    );
    if (pic30_device_has_gie()) {
      emit_insn(
        gen_insert_gie(reg)
      );
    }
    DONE;
  }")

(define_insn "disable_isr_gie"
  [(unspec_volatile [(const_int 0)] UNSPECV_DISABLE_ISR_GIE)]
  ""
  "*
{
  if (pic30_device_has_gie()) {
    // clear the GIE
    return \"bclr _INTCON2,#15\";
  } else {
    error(\"This device has no GIE\");
    return \"; This device has no GIE\";
  }
}")

(define_insn "enable_isr_gie"
  [(unspec_volatile [(const_int 0)] UNSPECV_ENABLE_ISR_GIE)]
  ""
  "*
{
  if (pic30_device_has_gie()) {
    // set the GIE
    return \"bset _INTCON2,#15\";
  } else {
    error(\"This device has no GIE\");
    return \"; This device has no GIE\";
  }
}")

(define_insn "disable_isr_ipl"
  [(unspec_volatile [(const_int 0)] UNSPECV_DISABLE_ISR)
   (clobber
      (match_scratch:HI 0  "=a,r"))
   (clobber
      (match_scratch:HI 1  "=r,r"))]
  ""
  "*
{
  if (which_alternative == 0) {
    return \"mov _DISICNT,%1\;disi #0x3FFF\;mov #0xE0,%0\;ior _SR\;mov %1,_DISICNT\;mov w0,_WREG0\";
  } else {
    return \"mov _DISICNT,%1\;disi #0x3FFF\;mov _SR,%1\;ior #0xE0,%1\;mov %1,_SR\;mov %1,_DISICNT\;mov w0,_WREG0\";
  }
}")

(define_insn "enable_isr_ipl"
  [(unspec_volatile [(const_int 0)] UNSPECV_ENABLE_ISR)
   (clobber
      (match_scratch:HI 0  "=r"))
   (clobber
      (match_scratch:HI 1  "=r"))]
  ""
  "*
{
  // if (which_alternative == 0) {
  //  return \"mov #0x1F,%1\;and.b _SR,WREG\;bset.b w0,#7\;mov.b w0,_SR\";
  // } else {
    return \"mov _DISICNT,%1\;disi #0x3FFF\;mov _SR,%0\;and.b #0x1F,%0\;bset.b %0,#7\;mov %0,_SR\;mov %1,_DISICNT\;mov w0,_WREG0\";
  // }
}")

(define_expand "disable_isr"
  [(const_int 0)]
  ""
  "{
     if (pic30_device_has_gie()) {
       emit_insn(
         gen_disable_isr_gie()
       );
     } else {
       emit_insn(
         gen_disable_isr_ipl()
       );
     }
     DONE;
   }")

(define_expand "enable_isr"
  [(const_int 0)]
  ""
  "{
     if (pic30_device_has_gie()) {
       emit_insn(
         gen_enable_isr_gie()
       );
     } else {
       emit_insn(
         gen_enable_isr_ipl()
       );
     }
     DONE;
   }")


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;	Peephole
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; generate btst f, #bit

; first from load, and, compare
(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (match_operand:HI 1 "pic30_near_operand" ""))
   (set (match_operand:HI 2 "pic30_register_operand" "")
        (and:HI 
           (match_dup 0)
           (match_operand 3 "immediate_operand" "")))
   (set (cc0)
        (compare
          (match_dup 2)
          (const_int 0)))
  ]
  "(pic30_one_bit_set_p(INTVAL(operands[3])) &&
    (find_regno_note(prev_active_insn(insn), REG_DEAD, REGNO(operands[0])) ||
     (REGNO(operands[2]) == REGNO(operands[0])) ||
     pic30_dead_or_set_p(insn,operands[0])) &&
    (find_regno_note(insn, REG_DEAD, REGNO(operands[2])) ||
     pic30_dead_or_set_p(next_active_insn(insn),operands[2])))"
  "btst %1,#%b3"
  [(set_attr "cc" "math")]
)
 

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (rotate:HI
           (match_operand:HI 1 "pic30_register_operand" "")
           (const_int 1)))
   (set (match_dup 0)
        (rotatert:HI
           (match_dup 0)
           (const_int 1)))
  ]
  ""
  ""
  [(set_attr "cc" "unchanged")]
)

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (rotate:HI
           (match_operand:HI 1 "pic30_register_operand" "")
           (const_int 1)))
   (set (match_operand:HI 2 "pic30_register_operand" "")
        (rotatert:HI
           (match_dup 0)
           (const_int 1)))
  ]
  "(pic30_dead_or_set_p(NEXT_INSN(insn),operands[0]) && 
    (REGNO(operands[1]) == REGNO(operands[2])))"
  ""
  [(set_attr "cc" "unchanged")]
)

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (rotate:HI
           (match_operand:HI 1 "pic30_register_operand" "")
           (const_int 1)))
   (set (match_operand:HI 2 "pic30_register_operand" "")
        (rotatert:HI
           (match_dup 0)
           (const_int 1)))
  ]
  "pic30_dead_or_set_p(NEXT_INSN(insn),operands[0])"
  "mov %1,%2"
  [(set_attr "cc" "unchanged")]
)

;; sequential assignments of 0 to a register
(define_peephole
  [
   (set (match_operand:HI 0 "pic30_register_operand" "")
        (const_int 0))
   (set (match_operand:HI 1 "pic30_register_operand" "")
        (const_int 0))
  ]
  "((REGNO(operands[0]) + 1 == REGNO(operands[1])) && 
    ((REGNO(operands[0]) & 1) == 0))"
  "mul.uu %0, #0, %0"
  [(set_attr "cc" "unchanged")]
)

(define_peephole
  [
   (set (match_operand:HI 0 "pic30_register_operand" "")
        (const_int 0))
   (set (match_operand:HI 1 "pic30_register_operand" "")
        (const_int 0))
  ]
  "((REGNO(operands[1]) + 1 == REGNO(operands[0])) && 
    ((REGNO(operands[1]) & 1) == 0))"
  "mul.uu %1, #0, %1"
  [(set_attr "cc" "unchanged")]
)


;;
;; 16-bit shift right SI followed by truncate to HI.
;; Simplify to most-significant subreg.
;; Unsigned shift.
;;
(define_peephole2
  [(set (match_operand:SI 0            "pic30_register_operand"    "")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand 2 "immediate_operand" "")))
   (set (match_operand:HI 3 "pic30_move_operand" "")
        (match_operand:HI 4 "pic30_register_operand" ""))
  ]
  "(INTVAL(operands[2]) == 16) &&
   (REGNO(operands[0]) == REGNO(operands[4])) &&
   (((REGNO(operands[0]) <= REGNO(operands[3])) && 
     (REGNO(operands[3]) < REGNO(operands[0]) + 
                    HARD_REGNO_NREGS(REGNO(operands[0]), GET_MODE (operands[0]))
     )) || peep2_reg_dead_p(2, operands[0]))"
  [(set (match_dup 3)
        (subreg:HI (match_dup 1) 2))]
  ""
  )

(define_peephole2
  [(set (match_operand:SI 0            "pic30_register_operand"    "")
        (lshiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand 2 "immediate_operand" "")))
   (set (match_operand:HI 3 "pic30_register_operand" "")
        (match_operator:HI 6 "pic30_valid_operator"
           [(match_operand:HI 4 "pic30_register_operand" "")
            (match_operand:HI 5 "" "")]))
  ]
  "(INTVAL(operands[2]) == 16) &&
   (REGNO(operands[0]) == REGNO(operands[4])) &&
   (((REGNO(operands[0]) <= REGNO(operands[3])) &&
     (REGNO(operands[3]) < REGNO(operands[0]) + 
                    HARD_REGNO_NREGS(REGNO(operands[0]), GET_MODE (operands[0]))     )) || peep2_reg_dead_p(2, operands[0]))"
  [ 
   (set (match_dup 4) (subreg:HI (match_dup 1) 2))
   (set (match_dup 3)
        (match_op_dup 6 [(match_dup 4) (match_dup 5)]))
  ]
  ""
  )

;; Ditto for signed shift.
(define_peephole2
  [(set (match_operand:SI 0            "pic30_register_operand"    "")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand 2 "immediate_operand" "")))
   (set (match_operand:HI 3 "pic30_move_operand" "")
        (match_operand:HI 4 "pic30_register_operand" ""))
  ]
  "(INTVAL(operands[2]) == 16) &&
   (REGNO(operands[0]) == REGNO(operands[4])) &&
   (((REGNO(operands[0]) <= REGNO(operands[3])) &&
     (REGNO(operands[3]) < REGNO(operands[0]) + 
                    HARD_REGNO_NREGS(REGNO(operands[0]), GET_MODE (operands[0]))     )) || peep2_reg_dead_p(2, operands[0]))"

  [(set (match_dup 3)
        (subreg:HI (match_dup 1) 2))]
  ""
  )

(define_peephole2
  [(set (match_operand:SI 0            "pic30_register_operand"    "")
        (ashiftrt:SI (match_operand:SI 1 "pic30_register_operand"  "")
                     (match_operand 2 "immediate_operand" "")))
   (set (match_operand:HI 3 "pic30_register_operand" "")
        (match_operator:HI 6 "pic30_valid_operator"
           [(match_operand:HI 4 "pic30_register_operand" "")
            (match_operand:HI 5 "" "")]))
  ]
  "(INTVAL(operands[2]) == 16) &&
   (REGNO(operands[0]) == REGNO(operands[4])) &&
   (((REGNO(operands[0]) <= REGNO(operands[3])) &&
     (REGNO(operands[3]) < REGNO(operands[0]) +
                    HARD_REGNO_NREGS(REGNO(operands[0]), GET_MODE (operands[0]))     )) || peep2_reg_dead_p(2, operands[0]))"
  [
   (set (match_dup 4) (subreg:HI (match_dup 1) 2))
   (set (match_dup 3)
        (match_op_dup 6 [(match_dup 4) (match_dup 5)]))
  ]
  ""
  )


;; Move [Base+Index] to [Base+Index] where Base or Index is common
(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
	(match_operand:HI 1 "pic30_mode3_operand" "RS<>r"))
   (set (match_operand:HI 2 "pic30_mode3_operand" "=RS<>r")
	(match_dup 0))
  ]
 "pic30_IndexEqual(operands[1], operands[2]) &&
	dead_or_set_p(insn, operands[0])"
 "mov %1,%2"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
	(match_operand:QI 1 "pic30_mode3_operand" "RS<>r"))
   (set (match_operand:QI 2 "pic30_mode3_operand" "=RS<>r")
	(match_dup 0))
  ]
 "pic30_IndexEqual(operands[1], operands[2]) &&
	dead_or_set_p(insn, operands[0])"
 "mov.b %1,%2"
  [(set_attr "cc" "unchanged")])

;; add a,b,c;  mov [c], d => mov [a+b], d
(define_peephole2
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (plus:HI (match_operand:HI 1 "pic30_register_operand" "")
                 (match_operand:HI 2 "pic30_register_operand" "")))
   (set (match_operand:HI 3 "pic30_register_operand" "")
        (mem:HI (match_dup 0)))]
  "((!pic30_ecore_target()) && (peep2_reg_dead_p(2, operands[0]) || (REGNO(operands[0]) == REGNO(operands[3]))))"
  [(set (match_dup 3)
        (mem:HI (plus:HI (match_dup 1) (match_dup 2))))]
)

(define_peephole
 [(set (match_operand:HI 0 "pic30_register_operand" "=r")
       (match_dup 0))]
 ""
 "; mov %0, %0"
)

(define_peephole
 [(set (match_operand:HI 0 "pic30_register_operand" "=r")
       (match_operand:HI 1 "pic30_move_operand" "RS<>rTQ"))
  (set (match_operand:HI 2 "pic30_register_operand" "=r")
       (match_dup 0))]
 "dead_or_set_p(insn, operands[0])"
 "mov %1, %2"
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;; GCC often loads a function parameter into an arbitrary register,
;; then moves that register to one appropriate for the function call.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; mov #addr,r0; mov r0,r1 becomes mov #addr,r1

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand"              "=r")
        (match_operand:HI 1 "pic30_symbolic_address_operand" "g"))
   (set (match_operand:HI 2 "pic30_register_operand"              "=r")
        (match_dup 0))]
 "dead_or_set_p(insn, operands[0])"
  "*
{ rtx sym;

  sym = pic30_program_space_operand_p(operands[1]);
  if (sym) {
    tree fndecl = SYMBOL_REF_DECL(sym);
    tree fndecl_attrib;
    char *access=0;
    int slot = 0;

    if (fndecl_attrib = lookup_attribute(IDENTIFIER_POINTER(pic30_identBoot[0]),
                                         DECL_ATTRIBUTES(fndecl))) {
      access=\"boot\";
    } else if (fndecl_attrib = lookup_attribute(
                                IDENTIFIER_POINTER(pic30_identSecure[0]),
                                DECL_ATTRIBUTES(fndecl))) {
      access=\"secure\";
    }
    if (access) {
      if (TREE_VALUE(fndecl_attrib)) {
        if (TREE_CODE(TREE_VALUE(TREE_VALUE(fndecl_attrib))) == INTEGER_CST) {
          slot = TREE_INT_CST_LOW(TREE_VALUE(TREE_VALUE(fndecl_attrib)));
        } else access = 0;
      } else access = 0;
    }
    if (access) {
      static char buffer[25];
      sprintf(buffer,\"mov #%s(%d),%%2\", access, slot);
      return buffer;
    } else {
      return(\"mov #handle(%1),%2\");
    }
  } else {
    return(\"mov #%1,%2\");
  }
}"
  [(set_attr "cc" "clobber")])

;; mov.d [wn+k],r0; mov.d r0,r1 becomes mov.d [wn+k],r1

(define_peephole
  [(set (match_operand:SF 0 "pic30_register_operand" "=r")
        (match_operand:SF 1 "pic30_Q_operand"   "Q"))
   (set (match_operand:SF 2 "pic30_register_operand" "=r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
	int idSrc, idDst;

	idDst = REGNO(operands[2]);
	idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
	if (idDst == idSrc)
	{
		return \"mov %Q1,%d2\;mov %1,%2\";
	}
	else
	{
		return \"mov %1,%2\;mov %Q1,%d2\";
	}
}"
  [(set_attr "cc" "clobber")])

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (match_operand:SI 1 "pic30_Q_operand"   "Q"))
   (set (match_operand:SI 2 "pic30_register_operand" "=r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
	int idSrc, idDst;

	idDst = REGNO(operands[2]);
	idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
	if (idDst == idSrc)
	{
		return \"mov %Q1,%d2\;mov %1,%2\";
	}
	else
	{
		return \"mov %1,%2\;mov %Q1,%d2\";
	}
}"
  [(set_attr "cc" "clobber")])

;; mov.d a,r0; mov.d r0,r1 becomes mov.d a,r1

(define_peephole
  [(set (match_operand:SF 0 "pic30_register_operand" "=r")
        (match_operand:SF 1 "pic30_T_operand"   "T"))
   (set (match_operand:SF 2 "pic30_register_operand" "=r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
	return \"mov %1,%2\;mov %Q1,%d2\";
}"
  [(set_attr "cc" "clobber")])

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (match_operand:SI 1 "pic30_T_operand"   "T"))
   (set (match_operand:SI 2 "pic30_register_operand" "=r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
	return \"mov %1,%2\;mov %Q1,%d2\";
}"
  [(set_attr "cc" "clobber")])

;; mov.q #k,r0; mov.q r0,r1 becomes mov.q #k,r1

(define_peephole
  [(set (match_operand:DF 0 "pic30_register_operand" "=r,r")
        (match_operand:DF 1 "immediate_operand" "G,i"))
   (set (match_operand:DF 2 "pic30_register_operand" "=r,r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
	REAL_VALUE_TYPE r;
	long l[4] = { 0 };

	switch (which_alternative)
	{
	case 0:
		return(	\"mul.uu %2,#0,%2\;\"
			\"mul.uu %t2,#0,%t2\");
	default:
		REAL_VALUE_FROM_CONST_DOUBLE(r, operands[1]);
		REAL_VALUE_TO_TARGET_DOUBLE(r, l);
		if (l[0] == 0)
		{
			return( \"mul.uu %0,#0,%2\;\"
				\"mov #%x1,%t2\;\"
				\"mov #%w1,%q2\");
		}
		else
		{
			return( \"mov #%z1,%2\;\"
				\"mov #%y1,%d2\;\"
				\"mov #%x1,%t2\;\"
				\"mov #%w1,%q2\");
		}
	}
}"
  [(set_attr "cc" "clobber")])

;; mov.q [wn+k],r0; mov.q r0,r1 becomes mov.q [wn+k],r1

(define_peephole
  [(set (match_operand:DF 0 "pic30_register_operand" "=r")
        (match_operand:DF 1 "pic30_Q_operand"   "Q"))
   (set (match_operand:DF 2 "pic30_register_operand" "=r")
        (match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0])"
 "*
{
  int idSrc, idDst;
  char temp[48];
  char save[48];
  static char szInsn[48];

  szInsn[0] = 0;
  temp[0] = 0;
  save[0] = 0;

  idDst = REGNO(operands[2]);
  idSrc = REGNO(XEXP(XEXP(operands[1],0),0));
  strcpy(temp, \"mov %1,%2\;\");
  if (idDst != idSrc)
    strcat(szInsn, temp);
  else
    strcat(save, temp);
  idDst++;
  strcpy(temp, \"mov %Q1,%d2\;\");
  if (idDst != idSrc)
    strcat(szInsn, temp);
  else
    strcat(save, temp);
  idDst++;
  strcpy(temp, \"mov %R1,%t2\;\");
  if (idDst != idSrc)
    strcat(szInsn, temp);
  else
    strcat(save, temp);
  idDst++;
  strcpy(temp, \"mov %S1,%q2\;\");
  if (idDst != idSrc)
    strcat(szInsn, temp);
  else
    strcat(save, temp);
  idDst++;
  if (save[0]) {
    save[strlen(save)-2] = 0;
    strcat(szInsn, save);
  }
  return szInsn;
}"
  [(set_attr "cc" "clobber")])

;;
;; Substitute RETLW #k,Wn for MOV #k,Wn; RETURN
;;
(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
	(match_operand:HI 1 "pic30_J_operand"   "J"))
   (return)
  ]
 "pic30_null_epilogue_p()"
 "*
{
	pic30_set_function_return(TRUE);
	if (REGNO(operands[0]) == WR0_REGNO)
	{
		return(\"retlw #%1,%0\");
	}
	else
	{
		return(\"mov #%1,%0\;return\");
	}
}"
  [(set_attr "cc" "clobber")])

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
	(match_operand:SI 1 "pic30_J_operand"   "J"))
   (return)
  ]
 "pic30_null_epilogue_p()"
 "*
{
	pic30_set_function_return(TRUE);
	if (REGNO(operands[0]) == WR0_REGNO)
	{
		return(\"mov #0,%d0\;retlw #%1,%0\");
	}
	else
	{
		return(\"mov #0,%d0\;mov #%1,%0\;return\");
	}
}"
  [(set_attr "cc" "clobber")])

;; Substitute bra/goto f for rcall/call f; ret

; (define_peephole
;   [(call (match_operand:QI 0 "memory_operand" "R,mp")
;          (match_operand:HI 1 "pic30_general_operand" ""))
;    (return)
;   ]
;   "pic30_null_epilogue_p() /* a */"
;   "*
; {
; 	pic30_set_function_return(TRUE);
; 	switch (which_alternative)
; 	{
; 	static char szInsn[48];
; 	case 0:
; 		sprintf(szInsn, \"goto %s\",
; 				reg_names[REGNO(XEXP(operands[0],0))]);
; 		return(szInsn);
; 	case 1:
; 		if (pic30_near_function_p(operands[0]))
; 			return(\"bra %0\");
; 		else
; 			return(\"goto %0\");
; 	default:
; 		return(\";\");
; 	}
; }")

; (define_peephole
;   [(set (match_operand 0 "pic30_register_operand"         "r,r")
;         (call (match_operand:QI 1 "memory_operand"  "R,mp")
;               (match_operand:HI 2 "pic30_general_operand" "")))
;    (return)
;   ]
;   "pic30_null_epilogue_p()"
;   "*
; {
; 	pic30_set_function_return(TRUE);
; 	switch (which_alternative)
; 	{
; 	static char szInsn[48];
; 	case 0:
; 		sprintf(szInsn, \"goto %s\",
; 				reg_names[REGNO(XEXP(operands[1],0))]);
; 		return(szInsn);
; 	case 1:
; 		if (pic30_near_function_p(operands[1]))
; 			return(\"bra %1\");
; 		else
; 			return(\"goto %1\");
; 	default:
; 		return(\";\");
; 	}
; }")

;; Combine mov.w pairs to mov.d

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "pic30_register_operand"  "r"))
   (set (match_operand:HI 2 "pic30_register_operand" "=r")
        (match_operand:HI 3 "pic30_register_operand"  "r"))
  ]
  "pic30_registerpairs_p(operands[0],operands[2],operands[1],operands[3])"
  "*
{
	if (REGNO(operands[0]) < REGNO(operands[2]))
	{
		return(\"mov.d %1,%0\");
	}
	else
	{
		return(\"mov.d %3,%2\");
	}
}"
  [(set_attr "cc" "clobber")])
  
;; Combine mov.w [Wn],Wm; mov.w [Wn+2],Wm+1 to mov.d [Wn],Wm

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "pic30_R_operand"   "R"))
   (set (match_operand:HI 2 "pic30_register_operand" "=r")
        (match_operand:HI 3 "pic30_Q_operand"   "Q"))
  ]
  "(REGNO(operands[0]) == (REGNO(operands[2]) - 1)) &&
   (IS_EVEN_REG(REGNO(operands[0]))) &&
   (pic30_Q_base(operands[3]) == REGNO(operands[1])) &&
   (pic30_Q_displacement(operands[3]) == 2)"
  "mov.d %1,%0"
  [(set_attr "cc" "clobber")])

;; Zero-extend followed by shift

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand"                  "=r")
        (zero_extend:SI (match_operand:QI 1 "pic30_register_operand"   "r")) )
   (set (match_dup 0)
        (ashift:SI (match_dup 0)
                   (match_operand:HI 2 "pic30_imm16plus_operand" "i")))
  ]
  ""
  "*
{
	int n = INTVAL(operands[2]);
	if (n == 16)
	{
		return(\"ze %1,%d0\;mov #0,%0\");
	}
	else if (n >= 24)
	{
		return(\"sl %1,#%K2,%d0\;mov #0,%0\");
	}
	else
	{
		return(\"ze %1,%0\;sl %0,#%K2,%d0\;mov #0,%0\");
	}
}"
  [(set_attr "cc" "clobber")])


(define_peephole2
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (match_operand:HI 1 "pic30_near_operand"  "") )
   (set (match_operand:SI 2 "pic30_wreg_operand" "")
        (mult:SI (zero_extend:SI (match_dup 0))
                 (zero_extend:SI (match_operand 3 "immediate_operand"   ""))))]
  "((INTVAL(operands[3]) == 2) && (peep2_reg_dead_p(2, operands[0]) || (REGNO(operands[0]) == REGNO(operands[2]))))"
  [(set (subreg:HI (match_dup 2) 0)
        (ashift:HI (match_dup 1)
                   (const_int 1)))]
  ""
  )

(define_peephole2
  [(set (match_operand:HI 0 "pic30_register_operand" "")
        (match_operand:HI 1 "pic30_near_operand"  "") )
   (set (match_operand:SI 2 "pic30_register_operand" "")
        (mult:SI (zero_extend:SI (match_dup 0))
                 (match_operand:SI 3 "immediate_operand"   "")))
   (set (match_operand:HI 4 "pic30_wreg_operand" "" )
        (match_operand:HI 5 "pic30_register_operand" ""))
]
  "((INTVAL(operands[3]) == 2) && (REGNO(operands[2]) == REGNO(operands[5])) &&     (peep2_reg_dead_p(3, operands[0]) || (REGNO(operands[0]) == REGNO(operands[4]))))"
  [(set (subreg:HI (match_dup 2) 0)
        (ashift:HI (match_dup 1)
                   (const_int 1)))]
  ""
  )

(define_peephole2
  [(match_scratch:HI 5 "a") 
   (set (match_operand:HI 0 "pic30_register_operand" "")
        (match_operand:HI 1 "pic30_near_operand"  ""))
   (set (match_operand:SI 2 "pic30_register_operand" "")
        (mult:SI (zero_extend:SI (match_dup 0))
                 (match_operand:SI 3 "immediate_operand"   "")))
   (set (match_operand:HI 6 "pic30_register_operand" "")
        (plus:HI (match_operand:HI 7 "pic30_register_operand" "")
                 (match_operand:HI 4 "pic30_math_operand" "")))]
  "((INTVAL(operands[3]) == 2) && (REGNO(operands[7]) == REGNO(operands[2])) &&     (peep2_reg_dead_p(3, operands[0]) ||                                              (REGNO(operands[0]) == REGNO(operands[6])) ||                                   (REGNO(operands[0]) == REGNO(operands[2]))) &&                                (peep2_reg_dead_p(3, operands[2]) ||                                              (REGNO(operands[2]) == REGNO(operands[6]))))"
  [(set (match_dup 5)
        (ashift:HI (match_dup 1)
                   (const_int 1)))
   (set (match_dup 6) 
        (plus:HI (match_dup 4) (match_dup 5)))]
  ""
  )

(define_peephole2
  [(match_scratch:HI 5 "a") 
   (set (match_operand:HI 0 "pic30_register_operand" "")
        (match_operand:HI 1 "pic30_near_operand"  ""))
   (set (match_operand:SI 2 "pic30_register_operand" "")
        (mult:SI (zero_extend:SI (match_dup 0))
                 (match_operand:SI 3 "immediate_operand"   "")))
   (set (match_operand:HI 6 "pic30_register_operand" "")
        (plus:HI (match_operand:HI 4 "pic30_math_operand" "")
                 (match_operand:HI 7 "pic30_register_operand" "")))]
  "((INTVAL(operands[3]) == 2) && (REGNO(operands[7]) == REGNO(operands[2])) &&     (peep2_reg_dead_p(3, operands[0]) ||                                              (REGNO(operands[0]) == REGNO(operands[6])) ||                                   (REGNO(operands[0]) == REGNO(operands[2]))) &&                                (peep2_reg_dead_p(3, operands[2]) ||                                              (REGNO(operands[2]) == REGNO(operands[6]))))"
  [(set (match_dup 5)
        (ashift:HI (match_dup 1)
                   (const_int 1)))
   (set (match_dup 6) 
        (plus:HI (match_dup 4) (match_dup 5)))]
  ""
  )

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
        (zero_extend:SI (match_operand:QI 1 "pic30_register_operand" "r")) )
   (set (match_dup 0)
        (ashift:SI (match_dup 0)
                   (match_operand:HI 2 "pic30_imm8_operand"    "i")))
  ]
  ""
  "*
{
	return(	\"sl %1,#%2,%0\;\"
		\"mov #0,%d0\");
}"
  [(set_attr "cc" "clobber")])

;; bit set optimizations

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
	(match_operand:SI 1 "pic30_R_operand"   "R"))
   (set (match_dup 0)
        (ior:SI  (match_dup 0)
                 (match_operand:SI 2 "const_int_operand"   "i")))
   (set (match_dup 0)
        (ior:SI  (match_dup 0)
                 (match_operand:SI 3 "const_int_operand"   "i")))
   (set (match_dup 1)
	(match_dup 0))
  ]
 "dead_or_set_p(insn, operands[0]) &&
  pic30_one_bit_set_p(INTVAL(operands[2])) &&
  (pic30_which_bit(INTVAL(operands[2]))<16) &&
  pic30_one_bit_set_p(INTVAL(operands[3])) &&
  (pic30_which_bit(INTVAL(operands[2]))<16)"
 "bset %1,#%b2\;bset %1,#%b3"
  [(set_attr "cc" "clobber")])

;; Improve mov.b Wd,sfr

(define_peephole
  [(set (match_operand:QI 0 "pic30_near_operand" "=U,U")
	(match_operand:QI 1 "pic30_register_operand"    "d,a"))
   (set (match_operand:QI 2 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 3 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 4 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 5 "pic30_near_operand" "=U,U")
	(match_dup 1))
  ]
 "!(pic30_errata_mask & exch_errata)"
 "@
  exch w0,%1\;mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3\;mov.b WREG,%4\;mov.b WREG,%5\;exch w0,%1
  mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3\;mov.b WREG,%4\;mov.b WREG,%5"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_near_operand" "=U,U")
	(match_operand:QI 1 "pic30_register_operand"    "d,a"))
   (set (match_operand:QI 2 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 3 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 4 "pic30_near_operand" "=U,U")
	(match_dup 1))
  ]
 "!(pic30_errata_mask & exch_errata)"
 "@
  exch w0,%1\;mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3\;mov.b WREG,%4\;exch w0,%1
  mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3\;mov.b WREG,%4"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_near_operand" "=U,U")
	(match_operand:QI 1 "pic30_register_operand"    "d,a"))
   (set (match_operand:QI 2 "pic30_near_operand" "=U,U")
	(match_dup 1))
   (set (match_operand:QI 3 "pic30_near_operand" "=U,U")
	(match_dup 1))
  ]
 "!(pic30_errata_mask & exch_errata)"
 "@
  exch w0,%1\;mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3\;exch w0,%1
  mov.b WREG,%0\;mov.b WREG,%2\;mov.b WREG,%3"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_near_operand" "=U,U")
	(match_operand:QI 1 "pic30_register_operand"    "d,a"))
   (set (match_operand:QI 2 "pic30_near_operand" "=U,U")
	(match_dup 1))
  ]
 "!(pic30_errata_mask & exch_errata)"
 "@
  exch w0,%1\;mov.b WREG,%0\;mov.b WREG,%2\;exch w0,%1
  mov.b WREG,%0\;mov.b WREG,%2"
  [(set_attr "cc" "unchanged")])

;
;  mov val, reg
;  mov reg, reg2
;
;  -> mov val, reg2
;
;  don't know why we need them, seems the register allocator should do better
;

(define_peephole
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "immediate_operand" "i"))
   (set (match_operand:QI 2 "pic30_register_operand" "=r")
        (match_dup 0))]
  "dead_or_set_p(insn,operands[0])"
  "mov.b #%1,%2"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "pic30_near_operand" "U"))
   (set (match_operand:QI 2 "pic30_register_operand" "=r")
        (match_dup 0))]
  "dead_or_set_p(insn,operands[0])"
  "mov #%1,%2\;mov.b [%2],%2"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "pic30_general_operand" "g"))
   (set (match_operand:QI 2 "pic30_register_operand" "=r")
        (match_dup 0))]
  "dead_or_set_p(insn,operands[0])"
  "mov.b %1,%2"
  [(set_attr "cc" "unchanged")])
 
(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "immediate_operand" "i"))
   (set (match_operand:HI 2 "pic30_register_operand" "=r")
        (match_dup 0))]
  "dead_or_set_p(insn,operands[0])"
  "mov.w #%1,%2"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "pic30_general_operand" "g"))
   (set (match_operand:HI 2 "pic30_register_operand" "=r")
        (match_dup 0))]
  "dead_or_set_p(insn,operands[0])"
  "mov.w %1,%2"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:HI 0 "pic30_register_operand" "=r")
        (match_operand:HI 1 "pic30_general_operand" "g"))
   (set (match_dup 1)
        (match_dup 0))]
  "pic30_dead_or_set_p(NEXT_INSN(insn),operands[0])"
  "\\; move deleted"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:QI 0 "pic30_register_operand" "=r")
        (match_operand:QI 1 "pic30_general_operand" "g"))
   (set (match_dup 1)
        (match_dup 0))]
  "pic30_dead_or_set_p(NEXT_INSN(insn),operands[0])"
  "\\; move deleted"
  [(set_attr "cc" "unchanged")])

(define_peephole
  [(set (match_operand:SI 0 "pic30_register_operand" "=r")
        (match_operand:SI 1 "pic30_general_operand" "g"))
   (set (match_dup 1)
        (match_dup 0))]
  "pic30_dead_or_set_p(NEXT_INSN(insn),operands[0])"
  "\\; move deleted"
  [(set_attr "cc" "unchanged")])

;
; some general const int optimizations
;

(define_peephole2
 [(set (match_operand:HI 0 "pic30_register_operand" "")
       (match_operand:HI 1 "immediate_operand" ""))
  (set (match_operand:HI 2 "pic30_near_operand" "")
       (match_dup 0))]
 "((INTVAL(operands[1]) == 255) && 
   (!pic30_volatile_operand(operands[2],HImode)) &&
   (peep2_regno_dead_p(2, REGNO(operands[0]))))"
 [(set (match_dup 2) (const_int 0))
  (set (subreg:QI (match_dup 2) 0) (const_int -1))]
 ""
)

(define_peephole2
 [(set (match_operand:HI 0 "pic30_register_operand" "")
       (match_operand:HI 1 "immediate_operand" ""))
  (set (match_operand:HI 2 "pic30_near_operand" "")
       (match_dup 0))]
 "((INTVAL(operands[1]) == -256) && 
   (!pic30_volatile_operand(operands[2],HImode)) &&
   (peep2_regno_dead_p(2, REGNO(operands[0]))))"
 [(set (match_dup 2) (const_int -1))
  (set (subreg:QI (match_dup 2) 0) (const_int 0))]
 ""
)

;
; add become subtract
;
(define_peephole2
 [(set (match_operand:HI 0 "pic30_register_operand" "")
       (match_operand:HI 1 "immediate_operand" ""))
  (set (match_operand:HI 2 "pic30_register_operand" "")
       (plus:HI (match_dup 2)
                (match_dup 0)))]
 "((INTVAL(operands[1]) <= 0) && (INTVAL(operands[1]) >= -1023) && peep2_regno_dead_p(2,REGNO(operands[0])))"
 [ (set (match_dup 2)
        (minus:HI (match_dup 2) (match_dup 1)))]
 "{ operands[1] = gen_rtx_CONST_INT(HImode,-INTVAL(operands[1])); }"
)

(define_peephole2
 [(set (match_operand:HI 0 "pic30_register_operand" "")
       (match_operand:HI 1 "immediate_operand" ""))
  (set (match_operand:HI 2 "pic30_register_operand" "")
       (plus:HI (match_operand:HI 3 "pic30_register_operand" "")
                (match_dup 0)))]
 "((INTVAL(operands[1]) <= 0) && (INTVAL(operands[1]) >= -31) && peep2_regno_dead_p(2,REGNO(operands[0])))"
 [ (set (match_dup 2)
        (minus:HI (match_dup 3) (match_dup 1)))]
 "{ operands[1] = gen_rtx_CONST_INT(HImode,-INTVAL(operands[1])); }"
)

;
; combining two HI's into an SI
;

(define_peephole2
 [(set (match_operand:SI 0 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "")))
  (set (match_operand:SI 2 "pic30_register_operand" "")
       (ashift:SI (match_dup 0) (const_int 16)))
  (set (match_operand:SI 3 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 4 "pic30_register_operand" "")))
  (set (match_operand:SI 5 "pic30_register_operand" "")
       (ior:SI (match_dup 2)
               (match_dup 3)))]
 "((REGNO(operands[5]) != REGNO(operands[1])) && pic30_dead_or_set_p(peep2_next_insn(4), operands[2]) && pic30_dead_or_set_p(peep2_next_insn(4),operands[3]))"
 [(set (match_dup 5) (match_dup 4))
  (set (match_dup 0) (match_dup 1))]
 "{ int reg = REGNO(operands[5]);
    operands[5] = gen_rtx_REG(HImode,reg);
    operands[0] = gen_rtx_REG(HImode,reg+1); }"
)

; [(set (subreg:HI (match_dup 5) 0) (match_dup 4))
;  (set (subreg:HI (match_dup 5) 2) (match_dup 1))]

(define_peephole2
 [(set (match_operand:SI 0 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "")))
  (set (match_operand:SI 2 "pic30_register_operand" "")
       (ashift:SI (match_dup 0) (const_int 16)))
  (set (match_operand:SI 3 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 4 "pic30_register_operand" "")))
  (set (match_operand:SI 5 "pic30_register_operand" "")
       (ior:SI (match_dup 2)
               (match_dup 3)))]
 "(((REGNO(operands[5])+1) != REGNO(operands[4])) && pic30_dead_or_set_p(peep2_next_insn(4), operands[2]) && pic30_dead_or_set_p(peep2_next_insn(4),operands[3]))"
 [(set (match_dup 5) (match_dup 1))
  (set (match_dup 0) (match_dup 4))]
 "{ int reg = REGNO(operands[5]);
    operands[5] = gen_rtx_REG(HImode,reg+1);
    operands[0] = gen_rtx_REG(HImode,reg); }"
)

(define_peephole2
 [(set (match_operand:SI 0 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 1 "pic30_register_operand" "")))
  (set (match_operand:SI 2 "pic30_register_operand" "")
       (ashift:SI (match_dup 0) (const_int 16)))
  (set (match_operand:SI 3 "pic30_register_operand" "")
       (zero_extend:SI (match_operand:HI 4 "pic30_register_operand" "")))
  (set (match_operand:SI 5 "pic30_register_operand" "")
       (ior:SI (match_dup 2)
               (match_dup 3)))]
 "(REGNO(operands[0])+1 != REGNO(operands[4]))"
 [
  (set (match_dup 2) (match_dup 1))
  (set (match_dup 3) (match_dup 4))
  (set (match_dup 5) (match_dup 0))
 ] 
 "{ int reg = REGNO(operands[0]);
    operands[2] = gen_rtx_REG(HImode, reg+1);
    operands[3] = gen_rtx_REG(HImode, reg);  }"
)

(define_peephole2
 [(set (match_operand:SI 0 "pic30_register_operand" "")
       (match_operand:SI 1 "pic30_move_operand" ""))
  (set (match_operand:SI 2 "pic30_register_operand" "")
       (match_dup 0))]
 "peep2_reg_dead_p(2, operands[0])"
 [(set (match_dup 2) (match_dup 1))]
)

(define_peephole2
 [(set (match_operand:SI 0 "pic30_register_operand" "")
       (match_operand:SI 1 "pic30_register_operand" ""))
  (set (match_operand:SI 2 "pic30_move_operand" "")
       (match_dup 0))]
 "peep2_reg_dead_p(2, operands[0])"
 [(set (match_dup 2) (match_dup 1))]
)

(define_peephole2
 [(set (match_operand:SI 0 "pic30_move_operand" "")
       (match_operand:SI 1 "pic30_register_operand" ""))
  (set (match_operand:SI 2 "pic30_register_operand" "")
       (match_dup 0))]
 "peep2_reg_dead_p(2, operands[1])"
 [(set (match_dup 0) (match_dup 1))
  (set (match_dup 2) (match_dup 1))]
)

; [(set (subreg:HI (match_dup 5) 2) (match_dup 1))
;  (set (subreg:HI (match_dup 5) 0) (match_dup 4))]

;; (define_peephole
;;   [(set (match_operand:SI 0 "pic30_register_operand"                "=r")
;;         (sign_extend:SI (match_operand:HI 1 "pic30_register_operand" "r")) )
;;    (set (match_operand:SF 2 "pic30_register_operand" "=r")
;;         (call (match_operand:QI 3 "memory_operand"  "m")
;;               (match_operand:HI 4 "pic30_general_operand" "g")))
;;   ]
;;   "isfloatsisf(operands[3])"
;;   "*
;; {
;; 	if (TARGET_SMALL_CODE)
;; 		return(\"rcall __floathisf\");
;; 	else
;; 		return(\"call __floathisf\");
;; }")

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;End.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
