/* gcc_assert() should assert, at least on debug builds...
 * so why isn't this ON by default?! */
#if !defined(NDEBUG) && !defined(ENABLE_ASSERT_CHECKING)
#define ENABLE_ASSERT_CHECKING 1
#endif

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "target.h"
#include "tm.h"
#include "rtl.h"
#include "tree.h"
#include "expr.h"
#include "function.h"
#include "emit-rtl.h"
#include "insn-attr.h"
#include "df.h"
#include "xc-coverage.h"
#include "bversion.h" /* for BUILDING_GCC_MAJOR, BUILDING_GCC_MINOR etc. */

#include "config/mips/mips-protos.h"

#define _XSTR(s) _STR(s)
#define _STR(s) #s

/* the path of the cc1 executable */
const char *pic32_libexec_path = "/pic32mx/bin/gcc/pic32mx/"
                                  _XSTR(BUILDING_GCC_MAJOR) "."
                                  _XSTR(BUILDING_GCC_MINOR) "."
                                  _XSTR(BUILDING_GCC_PATCHLEVEL);

/* $s1 will be reserved exclusively for the CodeCov instrumentation code */
/* NOTE: any other $s0..$s7 regs would do here but $s0 and $s1 yield */
/* better results on microMIPS (see LBU16/SB16 base register reqs) */
/* Later note: actually on MIPS16, we can use *only* $s0 or $s1 for this */
/* Even later note: $s1 (aka $17) is used as the frame pointer for MIPS16 so this */
/* leaves us with $16 as the one and only option - good thing we have one :-| */
static const unsigned CC_ADDR_REGNO = 16;

/* these will be initialized per each BB by pic32_adjust_insert_pos () */
static unsigned CC_TMP1_REGNO = INVALID_REGNUM;
static unsigned CC_TMP2_REGNO = INVALID_REGNUM;

/* called from TARGET_CONDITIONAL_REGISTER_USAGE */
void
pic32_cond_reg_usage (void)
{
  /* reserve CC_ADDR_REGNO when code coverage is enabled */
  /* NOTE: we can't avoid this for 'nocodecov' functions since this is called only
   *       once per compilation unit - current limitation */
  if (mchp_codecov)
    {
      fixed_regs[CC_ADDR_REGNO] = call_used_regs[CC_ADDR_REGNO] = 1;
    }
}

/* called from mchp_subtarget_save_reg_p () */
bool
pic32_ccov_save_reg_p (unsigned regno)
{
  /* the reserved register needs to be saved only when actually doing
   * the code coverage instrumentation */
  return xccov_enabled () && regno == CC_ADDR_REGNO;
}

/* called at the beginning of the instrumentation of the current function
 * TODO: remove it, if not necessary after all */
void
pic32_begin_cc_instrument (void)
{
}

/* emits the instructions needed to load the address of the 'cc_bits'
 * array into the CC_ADDR_REGNO register */
void
pic32_ld_cc_bits (unsigned offset)
{
  /* just in case somebody messes with TARGET_XCCOV_CC_BITS_OFS... */
  /* (non-zero offsets are allowed only on MIPS16) */
  gcc_assert (offset == 0 || TARGET_MIPS16);

  rtx CC_ADDR_REG = gen_rtx_REG (Pmode, CC_ADDR_REGNO);
  rtx cc_bits_ref = gen_rtx_SYMBOL_REF (Pmode, xccov_cc_bits_name ());

  if (!flag_pic)
    {
      if (!TARGET_MIPS16)
        {
          /* split the address into a high part and a LO_SUM */
          rtx addr = NULL_RTX;
          bool rc = mips_split_symbol (CC_ADDR_REG, cc_bits_ref, Pmode, &addr);
          gcc_assert (rc && addr != NULL_RTX);
          mips_emit_move (CC_ADDR_REG, addr);
        }
      else
        {
          /* on MIPS16, this will generate a PC-relative LW instruction */
          /* (the cc_bits addr + offset will be stored in the constant pool) */
          cc_bits_ref = plus_constant (SImode, cc_bits_ref, offset);
          cc_bits_ref = force_const_mem (SImode, cc_bits_ref);
          gcc_assert (cc_bits_ref);
          emit_insn (gen_rtx_SET (Pmode, CC_ADDR_REG, cc_bits_ref));
        }
    }
  else
    {
      /* MIPS16 is again a little special... */
      if (TARGET_MIPS16)
        cc_bits_ref = mips_got_load (CC_ADDR_REG, cc_bits_ref, SYMBOL_GOTOFF_DISP);
      /* this will map to a LW $r17, ofs($gp) or, in MIPS16's case that can't
       * use $gp directly to a MOVE $r16, $gp and a LW $r17, ofs($r17) */
      emit_insn (gen_rtx_SET (Pmode, CC_ADDR_REG, cc_bits_ref));
      /* if a non-zero offset (MIPS16-only), add it */
      if (offset)
        emit_insn (gen_addsi3 (CC_ADDR_REG, CC_ADDR_REG, GEN_INT (offset)));
    }
}

/* helper for pic32_adjust_insert_pos() below;
 * returns a reg number or INVALID_REGNUM if no tmp reg was found */
static unsigned
find_unused_tmp_reg (basic_block bb, unsigned skip_regno)
{
  /* regs $2..$7 are preferable for microMIPS and MIPS16 (that's ok
   * given the order we use), then $8..$15 and $24..$25 and lastly $1
   * which is always free but requires extra care */
  /* the 'skip_regno' argument indicates a register returned by a
   * previous call to find_unused_tmp_reg() or INVALID_REGNUM, in the
   * case of first (per BB) call */
  if (skip_regno == AT_REGNUM)
    return INVALID_REGNUM;

  for (unsigned regno = (skip_regno != INVALID_REGNUM) ? (skip_regno + 1) : 2;
       regno <= 25; ++regno)
    {
      if ((regno < 16 || regno > 23)
          && (regno != skip_regno)
          && !REGNO_REG_SET_P (DF_LR_IN (bb), regno))
        return regno;
    }

  return AT_REGNUM; /* use $1 ($at is another name) as the last resort */
}

/* for now, we don't really change the insertion position from this hook,
 * we're using it to look at the tmp regs not live when the basic
 * block is entered */
rtx
pic32_adjust_insert_pos (basic_block bb,
                         int prologue_bb ATTRIBUTE_UNUSED,
                         rtx where)
{
  /* find a temp register that we can use;
   * NOTE: there's no need to check the return value as this will
   * always succeed ($at can always be used as a temporary register) */
  CC_TMP1_REGNO = find_unused_tmp_reg (bb, INVALID_REGNUM);

  /* in the MIPS16 case, a second temp register is needed */
  if (TARGET_MIPS16)
    {
      CC_TMP2_REGNO = find_unused_tmp_reg (bb, CC_TMP1_REGNO);

      /* no free register? (didn't hit this case yet in the tests but...) */
      if (CC_TMP2_REGNO == INVALID_REGNUM)
        {
          /* set first reg to $zero and the second to $at
           * (this will enable a save/restore path in pic32_setcovbit_m16() */
          CC_TMP1_REGNO = GP_REG_FIRST;
          CC_TMP2_REGNO = AT_REGNUM;
        }
    }
  else
    {
      /* otherwise, the second temp reg is not needed */
      CC_TMP2_REGNO = INVALID_REGNUM;
    }

  return where;
}

/* emits the instrumentation code for setting bit BIT_NO in the
 * 'cc_bits' array */
void
pic32_set_cc_bit (unsigned bit_no)
{
  rtx CC_ADDR_REG = gen_rtx_REG (Pmode, CC_ADDR_REGNO);
  rtx CC_TMP1_REG = gen_rtx_REG (SImode, CC_TMP1_REGNO);
  rtx CC_TMP2_REG = CC_TMP2_REGNO != INVALID_REGNUM ?
                      gen_rtx_REG (SImode, CC_TMP2_REGNO) : NULL_RTX;

  /* which byte of the cc_bits array we'll update */
  unsigned byte_no = bit_no / 8;
  const unsigned bit_mask = 1U << (bit_no & 7);
  unsigned tmp_ofs = 0;

  /* the MIPS16 LBU/SB instructions have 5-bit immediate values for their offsets
    * => 'byte_no' can be at most 31; we can extend this by offseting the
    * address in CC_ADDR_REG */
  if (TARGET_MIPS16 && byte_no > 31)
    {
      /* if the offset can be kept under 128, that would generate a single ADDIU8...*/
      if (byte_no < 127+32)
        tmp_ofs = (byte_no < 127) ? byte_no : 127;
      else
        tmp_ofs = byte_no;
      /* emit the add instruction(s) */
      emit_insn (gen_addsi3 (CC_ADDR_REG, CC_ADDR_REG, GEN_INT (tmp_ofs)));
      byte_no -= tmp_ofs;
    }

  /* if $at is not used, we can emit regular RTL insns */
  if (CC_TMP1_REGNO != AT_REGNUM && CC_TMP2_REGNO != AT_REGNUM)
    {
      rtx CC_SAVE1_REG = NULL_RTX;
      rtx CC_SAVE2_REG = NULL_RTX;

      /* for MIPS16, we can't use directly the regs > 7,
      * so use reg $2 and possibly $3 instead, but save their values */
      if (TARGET_MIPS16)
        {
          unsigned save_regno = 2;
          if (CC_TMP1_REGNO > 7)
            {
              CC_SAVE1_REG = CC_TMP1_REG;
              CC_TMP1_REG = gen_rtx_REG (SImode, save_regno++);
              emit_insn (gen_rtx_SET (SImode, CC_SAVE1_REG, CC_TMP1_REG));
            }
          if (CC_TMP2_REGNO > 7)
            {
              CC_SAVE2_REG = CC_TMP2_REG;
              CC_TMP2_REG = gen_rtx_REG (SImode, save_regno);
              emit_insn (gen_rtx_SET (SImode, CC_SAVE2_REG, CC_TMP2_REG));
            }
        }

      rtx byte_addr = CC_ADDR_REG;
      if (byte_no) byte_addr = gen_rtx_PLUS (SImode, byte_addr, GEN_INT (byte_no));
      rtx byte_ref = gen_rtx_MEM (QImode, byte_addr);

      /* mark the MEM volatile to prevent a possible move of the load/store insns */
      MEM_VOLATILE_P (byte_ref) = 1;

      /* LBU tmp1, byte_no(cc_addr) */
      emit_insn (gen_rtx_SET (SImode, CC_TMP1_REG,
                              gen_rtx_ZERO_EXTEND (SImode, byte_ref)));

      /* if not MIPS16, we can do the OR directly using the ORI insn */
      if (!TARGET_MIPS16)
        {
          /* ORI tmp1, tmp1, bit_mask */
          emit_insn (gen_iorsi3 (CC_TMP1_REG, CC_TMP1_REG, GEN_INT (bit_mask)));
        }
      else
        {
          /* on MIPS16, we need an additional register to load the immediate mask */
          /* i.e. LI tmp2, bit_mask */
          emit_insn (gen_rtx_SET (SImode, CC_TMP2_REG, GEN_INT (bit_mask)));

          /* OR tmp1, tmp2 */
          emit_insn (gen_iorsi3 (CC_TMP1_REG, CC_TMP1_REG, CC_TMP2_REG));
        }

      /* SB tmp1, byte_no(cc_addr) */
      emit_insn (gen_rtx_SET (QImode, byte_ref,
                              gen_lowpart_SUBREG (QImode, CC_TMP1_REG)));

      /* if we need to restore some reg values for MIPS16, do so now */
      if (CC_SAVE1_REG != NULL_RTX)
        emit_insn (gen_rtx_SET (SImode, CC_TMP1_REG, CC_SAVE1_REG));

      if (CC_SAVE2_REG != NULL_RTX)
        emit_insn (gen_rtx_SET (SImode, CC_TMP2_REG, CC_SAVE2_REG));
    }
  else
    {
      /* one more check to make sure that $at is the first reg for non-MIPS16,
      * resp. the second one for MIPS16 (since it is used implicitly in the patterns */
      gcc_assert (AT_REGNUM == (TARGET_MIPS16 ? CC_TMP2_REGNO : CC_TMP1_REGNO));

      /* ok, generate and emit the specific unspec_volatile pattern */
      emit_insn (!TARGET_MIPS16 ?
        gen_pic32_setcovbit (CC_ADDR_REG, GEN_INT (byte_no), GEN_INT (bit_mask)) :
        gen_pic32_setcovbit_m16 (CC_ADDR_REG, GEN_INT (byte_no), GEN_INT (bit_mask),
                                CC_TMP1_REG));
    }

  /* if we offseted the address, restore its value */
  if (tmp_ofs)
    emit_insn (gen_addsi3 (CC_ADDR_REG, CC_ADDR_REG, GEN_INT (-(int)tmp_ofs)));
}

const char *
pic32_setcovbit_m16 (rtx operands[])
{
#define ADD_STR(str) obstack_grow (&obs, (str), strlen((str)))
#define ADD_INT(x) do { \
  char buf[16]; \
  const int len = snprintf (buf, sizeof(buf), "%d", (int)(x)); \
  obstack_grow (&obs, buf, len); \
} while (0)

  /* use an obstack to provide storage for the variable length output */
  static struct obstack obs;
  static char *res = NULL;

  /* if a previously allocated string, free it; otherwise, init obstack */
  if (res)
    obstack_free (&obs, res);
  else
    obstack_init (&obs);

  /* .set noreorder and .set noat */
  ADD_STR ("%(%[");

  /* we're only interested in the temp register */
  unsigned tmp1 = REGNO (operands[3]), save1 = INVALID_REGNUM;

  /* $at will always back up a lower ($2 or $3) register */
  const unsigned tmp2 = 2 + (tmp1 == 2), save2 = 1;
  ADD_STR ("move\t$"); ADD_INT (save2); ADD_STR (",$"); ADD_INT (tmp2);

  /* if the tmp reg is not a MIPS16 reg, it can't be used directly */
  if (tmp1 > 7)
    {
      save1 = tmp1;
      tmp1 = tmp2 ^ 1; /* if tmp2 is $3, tmp1 will be $2 and viceversa */
      ADD_STR ("\n\tmove\t$"); ADD_INT (save1); ADD_STR (",$"); ADD_INT (tmp1);
    }
  else if (tmp1 == GP_REG_FIRST)
    {
      /* this is the *really rare* case where we didn't found any free regs */
      /* so we'll use the other $s register (one of them is CC_ADDR_REGNO) */
      /* that we can save on the stack using the SAVE insn */
      save1 = tmp1;
      tmp1 = CC_ADDR_REGNO ^ 1;/* if CC_ADDR_REGNO is $s0, tmp1 will be $s1 and viceversa */
      ADD_STR ("\n\tsave\t8,$"); ADD_INT (tmp1);
    }

  ADD_STR ("\n\tlbu\t$"); ADD_INT (tmp1); ADD_STR (",%1(%0)\t# pic32_setcovbit_m16 %1,%2");
  ADD_STR ("\n\tli\t$");  ADD_INT (tmp2); ADD_STR (",%2");
  ADD_STR ("\n\tor\t$");  ADD_INT (tmp1); ADD_STR (",$"); ADD_INT (tmp2);
  ADD_STR ("\n\tsb\t$");  ADD_INT (tmp1); ADD_STR (",%1(%0)");

  /* restore the value of lower reg used as tmp1 (if needed) */
  if (save1 != INVALID_REGNUM)
    {
      /* from another register or from the stack */
      if (save1 != GP_REG_FIRST)
        {
          ADD_STR ("\n\tmove\t$"); ADD_INT (tmp1); ADD_STR (",$"); ADD_INT (save1);
        }
      else
        {
          ADD_STR ("\n\trestore\t8,$"); ADD_INT (tmp1);
        }
    }

  /* this is always needed */
  ADD_STR ("\n\tmove\t$"); ADD_INT (tmp2); ADD_STR (",$"); ADD_INT (save2);

  /* .set reorder and .set at */
  ADD_STR ("%]%)");

  /* add a NUL terminator, then the finish the current object (string) / get its address */
  obstack_1grow (&obs, '\0');
  res = XOBFINISH (&obs, char *);

  /* return the address of the ASCIIZ string
   * (note: 'res' is a static variable so storage will be freed at the next call) */
  return res;

#undef ADD_STR
#undef ADD_INT
}

/* adds a 'nil' instruction at the end of the basic block (at the RTL level) */
bool pic32_add_eob_insn (basic_block bb)
{
  /* generate this end-of-block insn in all cases for now, for debugging purposes */
#if 0
  /* if no 16-bit insns, everything should already be aligned */
  if (!(TARGET_MIPS16 || TARGET_MICROMIPS))
    return false;
#endif

  /* the end label should've already been added at this point */
  rtx end = BB_END (bb);
  gcc_assert (LABEL_P (end));

  /* append the nil instruction and make it the new end of the block */
  end = emit_insn_after (gen_pic32_nil_op (GEN_INT (bb->index)), end);
  BB_END (bb) = end;
  set_block_for_insn (end, bb);

  return true;
}

/* this implements the 'nil instruction' at the assembler level */
const char *pic32_output_nil_op (rtx operands[] ATTRIBUTE_UNUSED)
{
  /* the 'instruction' will be just a comment (no extra code bytes) enclosed
   * in a .set noreorder/.set reorder pair of directives (or vice versa,
   * depending on the current state).
   * we're counting here on a (side) effect the '.set noreorder' directive has
   * on the assembler: it makes it "forget" the previously seen labels for the
   * current instruction, thus effectively disabling the automatic alignment
   * for the label marking the end of the basic block */
  return mips_noreorder.nesting_level ? ".set\treorder\n"
                                        "\t# end of BB %0\n"
                                        "\t.set\tnoreorder"
                                      :
                                        ".set\tnoreorder\n"
                                        "\t# end of BB %0\n"
                                        "\t.set\treorder";
}
