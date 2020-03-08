#ifndef XC_COVERAGE_H_
#define XC_COVERAGE_H_

/* Section names for code coverage information */
#define  CODECOV_SECTION   ".codecov"
#define  CODECOV_INFO      ".codecov_info"
#define  CODECOV_INFO_HDR  ".codecov_info.hdr"

/*
 * The target code could/should define the following macros (xc-coverage.c uses them):
 *
 * TARGET_XCCOV_POINTSIZE    - the number of bits to hold the covered/not covered status for a
 *                             coverage point;
 *                             optional; the default value is 1
 *
 * TARGET_XCCOV_UNITSIZE     - the number of bits of the integers forming the 'cc_bits' array;
 *                             optional; the default value is 8 (i.e. uint8_t)
 *
 * TARGET_XCCOV_COVERED_VAL  - the value used to represent a 'covered' coverage point (0 or 1);
 *                             optional; the default value is 1 i.e. 1-'covered' and 0-'not covered'
 *
 * TARGET_XCCOV_LIBEXEC_PATH - the target specific path from which the compiler is executed
 *                             e.g. "/lib/gcc/pic32c/" for PIC32C to help locating the coverage tool;
 *                             mandatory; no default
 *
 * TARGET_XCCOV_BEGIN_INSTMT - optional; if defined, it is called at the beginning of the instrumentation
 *                             process (for each function)
 *
 * TARGET_XCCOV_ADJ_INS_POS  - optional; if defined, returns the position where (before which) the
 *                             instrumented code is inserted in a BB (or NULL, in which case the code
 *                             will be added just past BB_END(bb))
 *                             this is necessary for position-independent code on PIC32C, for example,
 *                             where a part of the PIC-setup code is added after NOTE_INSN_PROLOGUE_END;
 *                             example prototype:
 *                               RTX_INSN pic32c_adjust_insert_pos (basic_block bb,
 *                                                                  int         prologue_bb,
 *                                                                  RTX_INSN    where);
 *                             where: BB is the basic block where the code is to be inserted,
 *                                    PROLOGUE_BB is non-zero if BB contains NOTE_INSN_PROLOGUE_END and
 *                                    WHERE is the default position (see get_insertion_point ()
 *                                                                   in xc-coverage.c)
 *
 * TARGET_XCCOV_LD_CC_BITS   - this can be defined as a macro/function taking an unsigned int arg e.g.
 *                               void pic32c_ld_cc_bits (unsigned offset);
 *                             that would emit instructions to set-up a register (at the beginning of
 *                             each instrumented function) to the address of the 'cc_bits' array + an offset;
 *                             the offset is used only if TARGET_XCCOV_CC_BITS_OFS is defined, otherwise it
 *                             is ignored (always 0)
 *                             optional; default is undefined
 *
 * TARGET_XCCOV_CC_BITS_OFS  - define this as a C expression that will evaluate to a non-zero value in order
 *                             to have the indexes passed to TARGET_XCCOV_SET_CC_BIT relative to the current
 *                             function (and a possibly non-zero offset as argument to TARGET_XCCOV_LD_CC_BITS)
 *                             can be defined only if TARGET_XCCOV_LD_CC_BITS is defined; default is undefined
 *
 * TARGET_XCCOV_SET_CC_BIT   - the target should define this as a function/macro taking an unsigned
 *                             int argument (the index of a coverage point) and emit the instrumentation
 *                             code to set the specified entry in the 'cc_bits' array e.g.
 *                               void pic32c_set_cc_bit (unsigned bit_no);
 *                             If TARGET_XCCOV_CC_BITS_OFS is not defined, 'bit_no' would be relative to
 *                             the entire compilation unit, otherwise it would relative to the part/byte of
 *                             the 'cc_bits' array corresponding to the currently compiled function w/o
 *                             gaps/padding e.g. if the first compiled function has 10 instrumentation
 *                             points/coverage bits, if TARGET_XCCOV_CC_BITS_OFS is not defined, the first
 *                             coverage bit for the second function would be cc_bit #10, otherwise, it would
 *                             be cc_bit #2 (and the offset for TARGET_XCCOV_LD_CC_BITS would be 1)
 *
 * TARGET_XCCOV_END_INSTMT   - optional; if defined, it is called after all the instrumentation code was
 *                             inserted in the current function
 *
 * TARGET_XCCOV_LICENSED     - mandatory; should evaluate to != 0 if a valid CodeCov license is detected
 *
 * TARGET_XCCOV_EMIT_SECTION - mandatory; should emit the .section directive (+ the target specific flags)
 *                             corresponding to the single argument / section name (i.e. one of CODECOV_SECTION,
 *                             CODECOV_INFO_HDR or CODECOV_INFO)
 */

/* The target code ***should call*** this from TARGET_ASM_CODE_END so that the
 * 'cc_bits' definition and the coverage info section(s) are added to the assembly output */
void xccov_code_end (void);

/* Getter for code coverage symbol name (in case the target code needs it,
 * for example from TARGET_XCCOV_LD_CC_BITS) */
const char *xccov_cc_bits_name (void);

/* Returns 'true' if the code coverage pass will be run on the current function */
bool xccov_enabled (void);

#endif /* XC_COVERAGE_H_ */
