(define_insn "*store_word_multiple"
  [(match_parallel 0 ""
       [(set (match_operand:SI 1 "memory_operand")
	     (match_operand:SI 2 "register_operand"))])]
  "TARGET_MICROMIPS
   && micromips_save_restore_pattern_p (true, operands[0])"
  { return micromips_output_save_restore (true, operands[0]); }
  [(set_attr "type"	"multimem")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

(define_insn "*load_word_multiple"
  [(match_parallel 0 ""
       [(set (match_operand:SI 1 "register_operand")
	     (match_operand:SI 2 "memory_operand"))])]
  "TARGET_MICROMIPS
   && micromips_save_restore_pattern_p (false, operands[0])"
  { return micromips_output_save_restore (false, operands[0]); }
  [(set_attr "type"	"multimem")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

; For lwp
(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "memory_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_operand:SI 3 "memory_operand" ""))]
  "TARGET_MICROMIPS
   && REGNO (operands[0]) <= GP_REG_LAST
   && REGNO (operands[2]) <= GP_REG_LAST
   && ((REGNO (operands[0]) + 1 == REGNO (operands[2])
	&& micromips_load_store_pair_p (true, false, operands[0], operands[1], operands[3]))
       || (REGNO (operands[2]) + 1 == REGNO (operands[0])
	   && micromips_load_store_pair_p (true, true, operands[2], operands[3], operands[1])))"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_insn "*lwp"
  [(parallel [(set (match_operand:SI 0 "register_operand")
		   (match_operand:SI 1 "memory_operand"))
	      (set (match_operand:SI 2 "register_operand")
		   (match_operand:SI 3 "memory_operand"))])]

  "TARGET_MICROMIPS
   && REGNO (operands[0]) <= GP_REG_LAST
   && REGNO (operands[2]) <= GP_REG_LAST
   && ((REGNO (operands[0]) + 1 == REGNO (operands[2])
	&& micromips_load_store_pair_p (true, false, operands[0], operands[1], operands[3]))
       || (REGNO (operands[2]) + 1 == REGNO (operands[0])
	   && micromips_load_store_pair_p (true, true, operands[2], operands[3], operands[1])))"
  {
    if (REGNO (operands[0]) + 1 == REGNO (operands[2]))
      return micromips_output_load_store_pair (true, operands[0], operands[1]);
    else
      return micromips_output_load_store_pair (true, operands[2], operands[3]);
  }
  [(set_attr "type"	"load")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

; For swp
(define_peephole2
  [(set (match_operand:SI 0 "memory_operand" "")
        (match_operand:SI 1 "register_operand" ""))
   (set (match_operand:SI 2 "memory_operand" "")
        (match_operand:SI 3 "register_operand" ""))]
  "TARGET_MICROMIPS
   && REGNO (operands[1]) <= GP_REG_LAST
   && REGNO (operands[3]) <= GP_REG_LAST
   && ((REGNO (operands[1]) + 1 == REGNO (operands[3])
	&& micromips_load_store_pair_p (false, false, operands[1], operands[0], operands[2]))
       || (REGNO (operands[3]) + 1 == REGNO (operands[1])
	   && micromips_load_store_pair_p (false, true, operands[3], operands[2], operands[0])))"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_insn "*swp"
  [(parallel [(set (match_operand:SI 0 "memory_operand")
		   (match_operand:SI 1 "register_operand"))
	      (set (match_operand:SI 2 "memory_operand")
		   (match_operand:SI 3 "register_operand"))])]

  "TARGET_MICROMIPS
   && REGNO (operands[1]) <= GP_REG_LAST
   && REGNO (operands[3]) <= GP_REG_LAST
   && ((REGNO (operands[1]) + 1 == REGNO (operands[3])
	&& micromips_load_store_pair_p (false, false, operands[1], operands[0], operands[2]))
       || (REGNO (operands[3]) + 1 == REGNO (operands[1])
	   && micromips_load_store_pair_p (false, true, operands[3], operands[2], operands[0])))"
  {
    if (REGNO (operands[1]) + 1 == REGNO (operands[3]))
      return micromips_output_load_store_pair (false, operands[1], operands[0]);
    else
      return micromips_output_load_store_pair (false, operands[3], operands[2]);
  }
  [(set_attr "type"	"store")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

; For jraddiusp
(define_insn "mips_jraddiusp"
  [(parallel [(return)
              (use (reg:SI 31))
	      (set (reg:SI 29)
		   (plus:SI (reg:SI 29)
			    (match_operand 0 "const_int_operand")))])]
  "TARGET_MICROMIPS"
  "jraddiusp\t%0"
  [(set_attr "type"	"trap")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

; For movep
(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "reg_or_0_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_operand:SI 3 "reg_or_0_operand" ""))]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && (operands[1] == CONST0_RTX (SImode)
       || REGNO (operands[1]) == 0
       || REGNO (operands[1]) == 2
       || REGNO (operands[1]) == 3
       || (REGNO (operands[1]) >= 16 && REGNO (operands[1]) <= 20))
   && (operands[3] == CONST0_RTX (SImode)
       || REGNO (operands[3]) == 0
       || REGNO (operands[3]) == 2
       || REGNO (operands[3]) == 3
       || (REGNO (operands[3]) >= 16 && REGNO (operands[3]) <= 20))"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_peephole2
  [(set (match_operand:SI 0 "register_operand" "")
        (match_operand:SI 1 "reg_or_0_operand" ""))
   (set (match_operand:SF 2 "register_operand" "")
        (match_operand:SF 3 "const_0_operand" ""))]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && (operands[1] == CONST0_RTX (SImode)
       || REGNO (operands[1]) == 0
       || REGNO (operands[1]) == 2
       || REGNO (operands[1]) == 3
       || (REGNO (operands[1]) >= 16 && REGNO (operands[1]) <= 20))
   && operands[3] == CONST0_RTX (SFmode)"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_peephole2
  [(set (match_operand:SF 0 "register_operand" "")
        (match_operand:SF 1 "const_0_operand" ""))
   (set (match_operand:SI 2 "register_operand" "")
        (match_operand:SI 3 "reg_or_0_operand" ""))]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && operands[1] == CONST0_RTX (SFmode)
   && (operands[3] == CONST0_RTX (SImode)
       || REGNO (operands[3]) == 0
       || REGNO (operands[3]) == 2
       || REGNO (operands[3]) == 3
       || (REGNO (operands[3]) >= 16 && REGNO (operands[3]) <= 20))"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_peephole2
  [(set (match_operand:SF 0 "register_operand" "")
        (match_operand:SF 1 "const_0_operand" ""))
   (set (match_operand:SF 2 "register_operand" "")
        (match_operand:SF 3 "const_0_operand" ""))]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && operands[1] == CONST0_RTX (SFmode)
   && operands[3] == CONST0_RTX (SFmode)"
  [(parallel [(set (match_dup 0) (match_dup 1))
              (set (match_dup 2) (match_dup 3))])]
)

(define_insn "*movepsisi"
  [(parallel [(set (match_operand:SI 0 "register_operand")
		   (match_operand:SI 1 "reg_or_0_operand"))
	      (set (match_operand:SI 2 "register_operand")
		   (match_operand:SI 3 "reg_or_0_operand"))])]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && (operands[1] == CONST0_RTX (SImode)
       || REGNO (operands[1]) == 0
       || REGNO (operands[1]) == 2
       || REGNO (operands[1]) == 3
       || (REGNO (operands[1]) >= 16 && REGNO (operands[1]) <= 20))
   && (operands[3] == CONST0_RTX (SImode)
       || REGNO (operands[3]) == 0
       || REGNO (operands[3]) == 2
       || REGNO (operands[3]) == 3
       || (REGNO (operands[3]) >= 16 && REGNO (operands[3]) <= 20))"
{
  if (REGNO (operands[0]) < REGNO (operands[2]))
    return "movep\t%0,%2,%z1,%z3";
  else
    return "movep\t%2,%0,%z3,%z1";
}
  [(set_attr "type"	"move")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SI")])

(define_insn "*movepsisf"
  [(parallel [(set (match_operand:SI 0 "register_operand")
		   (match_operand:SI 1 "reg_or_0_operand"))
	      (set (match_operand:SF 2 "register_operand")
		   (match_operand:SF 3 "const_0_operand"))])]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && (operands[1] == CONST0_RTX (SImode)
       || REGNO (operands[1]) == 0
       || REGNO (operands[1]) == 2
       || REGNO (operands[1]) == 3
       || (REGNO (operands[1]) >= 16 && REGNO (operands[1]) <= 20))
   && operands[3] == CONST0_RTX (SFmode)"
{
  if (REGNO (operands[0]) < REGNO (operands[2]))
    return "movep\t%0,%2,%z1,%z3";
  else
    return "movep\t%2,%0,%z3,%z1";
}
  [(set_attr "type"	"move")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SF")])

(define_insn "*movepsfsi"
  [(parallel [(set (match_operand:SF 0 "register_operand")
		   (match_operand:SF 1 "const_0_operand"))
	      (set (match_operand:SI 2 "register_operand")
		   (match_operand:SI 3 "reg_or_0_operand"))])]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && operands[1] == CONST0_RTX (SFmode)
   && (operands[3] == CONST0_RTX (SImode)
       || REGNO (operands[3]) == 0
       || REGNO (operands[3]) == 2
       || REGNO (operands[3]) == 3
       || (REGNO (operands[3]) >= 16 && REGNO (operands[3]) <= 20))"
{
  if (REGNO (operands[0]) < REGNO (operands[2]))
    return "movep\t%0,%2,%z1,%z3";
  else
    return "movep\t%2,%0,%z3,%z1";
}
  [(set_attr "type"	"move")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SF")])

(define_insn "*movepsfsf"
  [(parallel [(set (match_operand:SF 0 "register_operand")
		   (match_operand:SF 1 "const_0_operand"))
	      (set (match_operand:SF 2 "register_operand")
		   (match_operand:SF 3 "const_0_operand"))])]
  "TARGET_MICROMIPS
   && micromips_movep_target_p (operands[0], operands[2])
   && operands[1] == CONST0_RTX (SFmode)
   && operands[3] == CONST0_RTX (SFmode)"
{
  if (REGNO (operands[0]) < REGNO (operands[2]))
    return "movep\t%0,%2,%z1,%z3";
  else
    return "movep\t%2,%0,%z3,%z1";
}
  [(set_attr "type"	"move")
   (set_attr "can_delay" "no")
   (set_attr "mode"	"SF")])
