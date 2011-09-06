 /* Definitions of target machine PIC32 for GNU compiler.
   Copyright (C) 1989, 1990, 1991, 1993, 1994, 1995, 1996, 1997, 1998,
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009
   Free Software Foundation, Inc.
   Contributed by J. Grosbach, james.grosbach@microchip.com
   Changes by J. Kajita, jason.kajita@microchip.com and
   G. Loegel, george.loegel@microchip.com

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either mips_expand_prologueversion 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

/* Macro for conditional compilation of PIC32 only stuff */

#ifndef MCHP_H
#define MCHP_H

#include <safe-ctype.h>
#include "config/mips/mips-machine-function.h"

#undef TARGET_MCHP_PIC32MX
#define TARGET_MCHP_PIC32MX 1

#undef MCHP_DEBUG
#define MCHP_DEBUG 1

extern       int mchp_io_size_val;
extern       HOST_WIDE_INT mchp_pic32_license_valid;

#undef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR 1

#undef TARGET_SHORT_DOUBLE
#define TARGET_SHORT_DOUBLE  0

/*
** This is how to output a reference to a user-level label named NAME.
** `assemble_name_raw' uses this.
*/
#if 1
#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE, NAME) \
do {                     \
  char * real_name;               \
  real_name = mchp_strip_name_encoding ((NAME));   \
  asm_fprintf (FILE, "%U%s", real_name);      \
} while (0)
#endif

/* Put at the end of the command given to the linker if -nodefaultlibs or
 * -nostdlib is not specified on the command line. This includes all the
 * standard libraries, the peripheral libraries if the -mno-peripheral-libs
 * option is not specified on the command line, and the processor-specific
 * peripheral library if -mno-peripheral-libs option is not specified, but
 * the -mprocessor option is specified.
 */
#undef  LIB_SPEC
#if 1 /* chipKIT */
#define LIB_SPEC "--start-group -lc -lm -lsupc++ -lpic32 %{!mno-peripheral-libs:-lmchp_peripheral %{mprocessor=*:-lmchp_peripheral_%*}} --end-group"
#else
#define LIB_SPEC "--start-group %{!mno-mpdebug-lib:-ldebug} -lc -lm -le -ldsp -lsupc++ %{!mno-peripheral-libs:-lmchp_peripheral %{mprocessor=*:-lmchp_peripheral_%*}} --end-group"
#endif

/* No libstdc++ for now.  Empty string doesn't work.  */
#if 1
#undef LIBSTDCXX
#define LIBSTDCXX "-lsupc++"
#undef LIBSTDCXX_STATIC
#define LIBSTDCXX_STATIC "-lsupc++"
#else
#undef LIBSTDCXX
#define LIBSTDCXX "-lgcc"
#undef LIBSTDCXX_STATIC
#define LIBSTDCXX_STATIC "-lgcc"
#endif

/* Don't set.  This defaults to crt0.o if not specified. */
#undef  STARTFILE_SPEC
#if 1 /* for chipKIT */
#define STARTFILE_SPEC "cpp-startup%O%s crti%O%s crtbegin%O%s"
#endif

/* Don't set.  This defaults to an empty string if not specified. */
#undef  ENDFILE_SPEC
#if 1 /* for chipKIT */
#define ENDFILE_SPEC "crtend%O%s crtn%O%s"
#endif

/* Added on the linker command line after all user-specified -L options are
 * included.  This will add all the standard -L search paths, the
 * processor-specific library search path, and define _DEBUGGER if the
 * -mdebugger command-line option was specified.
 */
#if 1
#define LINK_LIBGCC_SPEC "%D -L %s%{mprocessor=*:./proc/%*; :./proc/32MX795F512L} %{mdebugger:--defsym _DEBUGGER=1}"
#else
#define LINK_LIBGCC_SPEC "%D -L %s%{mprocessor=*:./proc/%*; :./proc/32MXGENERIC} %{mdebugger:--defsym _DEBUGGER=1}"
#endif

/* These are MIPS-specific specs that we do not utilize.  Undefine them
 * and define them as an empty string.
 */
#undef ENDIAN_SPEC
#define ENDIAN_SPEC ""

#undef ASM_ABI_DEFAULT_SPEC
#define ASM_ABI_DEFAULT_SPEC ""

/* Supports configure-time default options (i.e., '--with' options) in the
 * driver.  We don't have any options that are configurable at this time.
 */
#undef OPTION_DEFAULT_SPECS

/* The default MIPS CPU is pic32mx. */
#undef MIPS_CPU_STRING_DEFAULT
#define MIPS_CPU_STRING_DEFAULT "pic32mx"

/* Override the LINK_SPEC specified in mips.h since we removed a number
 * of the options utilized in that spec.
 */
#undef LINK_SPEC
#define LINK_SPEC "\
%{G*} %{bestGnum} %{shared} %{non_shared} \
%{mno-smart-io:--no-smart-io} %{msmart-io=0:--no-smart-io}"

/* Override the GAS_ASM_SPEC specified in mips.h since we removed the mtune
 * option utilized in that spec.
 */
#undef GAS_ASM_SPEC
#define GAS_ASM_SPEC "%{v}"

/* Override the ASM_SPEC specified in mips.h since we removed a number
 * of the options utilized in that spec.
 */
#undef ASM_SPEC
#define ASM_SPEC "\
%{G*} \
%{mips16:%{!mno-mips16:-mips16}} %{mno-mips16:-no-mips16} \
%{mips16e} \
%(subtarget_asm_optimizing_spec) \
%(subtarget_asm_debugging_spec) \
%{mxgot:-xgot} \
%(target_asm_spec) \
%(subtarget_asm_spec)"

/* SUBTARGET_CC1_SPEC is passed to the compiler proper.  It may be
   overridden by subtargets.  */
#ifndef SUBTARGET_CC1_SPEC
#define SUBTARGET_CC1_SPEC ""
#endif

/* CC1_SPEC is the set of arguments to pass to the compiler proper.  This
 * was copied from the one in mips.h, but that one had some problems and
 * contained the endian-selection options.
 */
#if 1 /* chipKIT */
#undef CC1_SPEC
#define CC1_SPEC "\
%{gline:%{!g:%{!g0:%{!g1:%{!g2: -g1}}}}} \
%{G*} \
-mconfig-data-dir= %J%s%{ mprocessor=* :./proc/%*; :./proc/32MX795F512L} \
%{mno-float:-fno-builtin-fabs -fno-builtin-fabsf} \
%{mlegacy-libc:-msmart-io=0 -fno-short-double} \
%{mlong-calls:-msmart-io=0} \
%{msmart-io:%{msmart-io=*:%emay not use both -msmart-io and -msmart-io=LEVEL}} \
%{mno-smart-io:%{msmart-io=*:%emay not use both -mno-smart-io and -msmart-io=LEVEL}} \
%{mno-smart-io:-msmart-io=0} \
%{msmart-io:-msmart-io=1} \
%{save-temps: -fverbose-asm} \
%{O2:%{!fno-remove-local-statics: -fremove-local-statics}} \
%{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}} \
%{!fexceptions:-fno-exceptions} \
%{!fno-function-sections:-ffunction-sections} \
%{!fno-data-sections:-fdata-sections} \
%(subtarget_cc1_spec) \
"
#else
#undef CC1_SPEC
#define CC1_SPEC "\
%{gline:%{!g:%{!g0:%{!g1:%{!g2: -g1}}}}} \
%{G*} \
-mconfig-data-dir= %J%s%{ mprocessor=* :./proc/%*; :./proc/32MXGENERIC} \
%{mno-float:-fno-builtin-fabs -fno-builtin-fabsf} \
%{mlegacy-libc:-msmart-io=0 -fno-short-double} \
%{mlong-calls:-msmart-io=0} \
%{msmart-io:%{msmart-io=*:%emay not use both -msmart-io and -msmart-io=LEVEL}} \
%{mno-smart-io:%{msmart-io=*:%emay not use both -mno-smart-io and -msmart-io=LEVEL}} \
%{mno-smart-io:-msmart-io=0} \
%{msmart-io:-msmart-io=1} \
%{save-temps: -fverbose-asm} \
%{O2:%{!fno-remove-local-statics: -fremove-local-statics}} \
%{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}} \
%{!fexceptions:-fno-exceptions} \
%(subtarget_cc1_spec) \
"
#endif

#if 1
#undef CC1PLUS_SPEC
#define CC1PLUS_SPEC "%{!frtti:-fno-rtti}"
#endif

/* Preprocessor specs.  */

/* SUBTARGET_CPP_SPEC is passed to the preprocessor.  It may be
   overridden by subtargets.  */
#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC ""
#endif

#undef CPP_SPEC
#define CPP_SPEC "%(subtarget_cpp_spec)\
%{mappio-debug:-D__APPIO_DEBUG} \
"

#undef SUBTARGET_SELF_SPECS
#define SUBTARGET_SELF_SPECS      \
  /* Make sure a -mips option is present.  This helps us to pick \
     the right multilib, and also makes the later specs easier  \
     to write.  */       \
  MIPS_ISA_LEVEL_SPEC,       \
         \
  /* Infer the default float setting from -march.  */   \
  MIPS_ARCH_FLOAT_SPEC,       \
         \
  /* Infer the default dsp setting from -march.  */   \
  MIPS_ARCH_DSP_SPEC,       \
         \
  /* If no ABI option is specified, infer one from the ISA level \
     or -mgp setting.  */      \
  "%{!mabi=*: %{" MIPS_32BIT_OPTION_SPEC ": -mabi=32;: -mabi=n32}}", \
         \
  /* Remove a redundant -mfp64 for -mabi=n32; we want the !mfp64 \
     multilibs.  There's no need to check whether the architecture \
     is 64-bit; cc1 will complain if it isn't.  */   \
  "%{mabi=n32: %<mfp64}",      \
         \
  /* Make sure that an endian option is always present.  This makes \
     things like LINK_SPEC easier to write.  */    \
  /* "%{!EB:%{!EL:%(endian_spec)}}", */     \
         \
  /* -mcode-xonly is a traditional alias for -mcode-readable=pcrel and \
     -mno-data-in-code is a traditional alias for -mcode-readable=no. \
     The latter trumps the former.  */     \
  "%{mno-data-in-code: -mcode-readable=no}",    \
  "%{!mcode-readable=no: %{mcode-xonly: -mcode-readable=pcrel}}", \
  "%<mno-data-in-code %<mcode-xonly"

/* We won't ever support SDB or MIPS debugging info.  */
#undef SDB_DEBUGGING_INFO
#undef MIPS_DEBUGGING_INFO
#undef DWARF2_DEBUGGING_INFO
#define DWARF2_DEBUGGING_INFO 1         /* dwarf2 debugging info */

#ifndef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
#endif

/* None of the OPTIONS specified in MULTILIB_OPTIONS are set by default. */
#undef MULTILIB_DEFAULTS

/* Describe how we implement __builtin_eh_return.  */

/* At the moment, nothing appears to use more than 2 EH data registers.
   The chosen registers must not clash with the return register ($2),
   EH_RETURN_STACKADJ ($3), or MIPS_EPILOGUE_TEMP ($5), and they must
   be general MIPS16 registers.  Pick $6 and $7.  */
#undef EH_RETURN_DATA_REGNO
#define EH_RETURN_DATA_REGNO(N) \
  ((N) < 2 ? 7 - (N) : INVALID_REGNUM)

/* Use $5 as a temporary for both MIPS16 and non-MIPS16.  */
#undef MIPS_EPILOGUE_TEMP_REGNUM
#define MIPS_EPILOGUE_TEMP_REGNUM (GP_REG_FIRST + 5)

/* Using long will always be right for size_t and ptrdiff_t, since
   sizeof(long) must equal sizeof(void *), following from the setting
   of the -mlong64 option.  */
#undef SIZE_TYPE
#define SIZE_TYPE "long unsigned int"
#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "long int"

/* Enable parsing of #pragma pack(push,<n>) and #pragma pack(pop).  */
#define HANDLE_PRAGMA_PACK_PUSH_POP 1

/* Use standard ELF-style local labels (not '$' as on early Irix).  */
#undef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."

/* Use periods rather than dollar signs in special g++ assembler names.  */
#define NO_DOLLAR_IN_LABEL

/* Attach a special .ident directive to the end of the file to identify
   the version of GCC which compiled this code.  */
#undef IDENT_ASM_OP
#define IDENT_ASM_OP "\t.ident\t"

/* Output #ident string into the ELF .comment section, so it doesn't
   form part of the load image, and so that it can be stripped.  */
#undef ASM_OUTPUT_IDENT
#define ASM_OUTPUT_IDENT(STREAM, STRING) \
  fprintf (STREAM, "%s\"%s\"\n", IDENT_ASM_OP, STRING);

/* Currently we don't support 128bit long doubles, so for now we force
   n32 to be 64bit.  */
#undef LONG_DOUBLE_TYPE_SIZE
#define LONG_DOUBLE_TYPE_SIZE 64

#ifdef IN_LIBGCC2
#undef LIBGCC2_LONG_DOUBLE_TYPE_SIZE
#define LIBGCC2_LONG_DOUBLE_TYPE_SIZE 64
#endif

/* Force all .init and .fini entries to be 32-bit, not mips16, so that
   in a mixed environment they are all the same mode. The crti.asm and
   crtn.asm files will also be compiled as 32-bit due to the
   -no-mips16 flag in SUBTARGET_ASM_SPEC above. */
#undef CRT_CALL_STATIC_FUNCTION
#define CRT_CALL_STATIC_FUNCTION(SECTION_OP, FUNC) \
   asm (SECTION_OP "\n\
 .set push\n\
 .set nomips16\n\
 jal " USER_LABEL_PREFIX #FUNC "\n\
 .set pop\n\
 " TEXT_SECTION_ASM_OP);

/* Used for option processing */
extern const char *mchp_processor_string;
extern const char *mchp_config_data_dir;

/* Since these switches are only used by the specs, do not need to assign a
 * unique mask
 */
#define MASK_LINK_PERIPHERAL_LIBS 0
#define MASK_DEBUG_EXEC           0
#define MASK_APPIO_DEBUG          0

/* Put small constants in .rodata, not .sdata. */
#undef TARGET_DEFAULT
/* TODO */
#define TARGET_DEFAULT              \
  (TARGET_CPU_DEFAULT    \
   | TARGET_ENDIAN_DEFAULT   \
   | TARGET_FP_EXCEPTIONS_DEFAULT  \
   | MASK_CHECK_ZERO_DIV   \
   | MASK_FUSED_MADD                            \
   | MASK_SOFT_FLOAT_ABI )

#undef TARGET_ENDIAN_DEFAULT
#define TARGET_ENDIAN_DEFAULT 0

#undef TARGET_FP_EXCEPTIONS_DEFAULT
#define TARGET_FP_EXCEPTIONS_DEFAULT 0

/* We want to change the default pre-defined macros. Many of these
   are the same as presented in sde.h, but not all */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()                            \
  do {                                                      \
    if (TARGET_ABICALLS || flag_pic)                        \
      {                                                     \
        builtin_define ("__PIC__");                         \
        builtin_define ("__pic__");                         \
      }                                                     \
                                                            \
    if (mips_abi != ABI_32)                                 \
      fatal_error ("internal error: mips_abi != ABI_32");   \
    builtin_define ("_ABIO32=1");                           \
    builtin_define ("_MIPS_SIM=_ABIO32");                   \
                                                            \
    builtin_define_with_int_value ("_MIPS_SZINT", INT_TYPE_SIZE); \
    builtin_define_with_int_value ("_MCHP_SZINT", INT_TYPE_SIZE); \
    builtin_define_with_int_value ("_MIPS_SZLONG", LONG_TYPE_SIZE); \
    builtin_define_with_int_value ("_MCHP_SZLONG", LONG_TYPE_SIZE); \
    builtin_define_with_int_value ("_MIPS_SZPTR", POINTER_SIZE); \
    builtin_define_with_int_value ("_MCHP_SZPTR", POINTER_SIZE); \
    builtin_define_with_int_value ("_MIPS_FPSET",           \
                                   32 / MAX_FPRS_PER_FMT);  \
    builtin_define_with_int_value ("_MCHP_FPSET",           \
                                   32 / MAX_FPRS_PER_FMT);  \
                                                            \
 /* These defines reflect the ABI in use, not whether the   \
    FPU is directly accessible.  */                         \
    if (TARGET_NO_FLOAT)                                    \
      {                                                     \
        builtin_define ("__NO_FLOAT");                      \
        builtin_define ("__mips_no_float");                 \
        builtin_define ("__mchp_no_float");                 \
      }                                                     \
    else if (!TARGET_HARD_FLOAT_ABI)                        \
      {                                                     \
        builtin_define ("__SOFT_FLOAT");                    \
        builtin_define ("__mips_soft_float");               \
      }                                                     \
    if (TARGET_SINGLE_FLOAT)                                \
      {                                                     \
        builtin_define ("__SINGLE_FLOAT");                  \
      }                                                     \
                                                            \
    builtin_define_std ("PIC32MX");                         \
    if ((mchp_processor_string != NULL) && *mchp_processor_string) \
      {                                                     \
        char *proc, *p;                                     \
        int setnum, memsize;                                \
        char *pinset;                                       \
        gcc_assert(strlen(mchp_processor_string) < 20);     \
        pinset = alloca(2);                                 \
        pinset[1] = 0;                                      \
        proc = alloca (strlen (mchp_processor_string) + 5); \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        for (p = proc ; *p ; p++)                           \
          *p = TOUPPER (*p);                                \
        builtin_define (proc);                              \
                                                            \
        if (strchr(proc,'F') != NULL) {                     \
          sscanf (proc, "__32MX%6dF%6d%1c__", &setnum,      \
                  &memsize, &pinset[0]);                    \
          builtin_define_with_int_value                     \
             ("__PIC32_FEATURE_SET__",                      \
              setnum);                                      \
          builtin_define_with_int_value                     \
             ("__PIC32_MEMORY_SIZE__",                      \
              memsize);                                     \
          builtin_define_with_value                         \
             ("__PIC32_PIN_SET__",                          \
              &pinset[0], 1);                               \
        }                                                   \
      }                                                     \
    else                                                    \
      {                                                     \
        builtin_define ("__32MXGENERIC__");                 \
      }                                                     \
                                                            \
    if ((version_string != NULL) && *version_string)  \
      {                                                     \
        char *Microchip;                                    \
        int pic32_compiler_version;                         \
        gcc_assert(strlen(version_string) < 80);            \
        Microchip = strrchr (version_string, 'v');          \
        if (Microchip)                                      \
          {                                                 \
            int major =0, minor=0;                          \
            while ((*Microchip) &&                          \
                   ((*Microchip < '0') ||                   \
                    (*Microchip > '9')))                    \
              { Microchip++; }                              \
            if (*Microchip)                                 \
              {                                             \
                major = strtol (Microchip, &Microchip, 0);  \
              }                                             \
            if ((*Microchip) &&                             \
               ((*Microchip=='_') || (*Microchip=='.')))    \
               {                                            \
                 Microchip++;                               \
                 minor = strtol(Microchip, &Microchip, 0);  \
               }                                            \
             pic32_compiler_version = (major*100) + minor;  \
          }                                                 \
        else                                                \
          {                                                 \
            fatal_error ("internal error: version_string == NULL");     \
            builtin_define_with_int_value ("__C32_VERSION__", -1);      \
          }                                                             \
        builtin_define_with_int_value ("__C32_VERSION__", pic32_compiler_version);  \
      }                                                     \
                                                            \
  } while (0);

/*
** Easy access check for function beginning
**/
#define NOTE_INSN_FUNCTION_BEG_P(INSN) \
  ((GET_CODE(INSN) == NOTE) && \
   (NOTE_KIND (INSN) == NOTE_INSN_FUNCTION_BEG))

/* The Microchip port has a few pragmas to define as well */
#undef REGISTER_TARGET_PRAGMAS

#define REGISTER_TARGET_PRAGMAS() { \
  c_register_pragma(0, "interrupt", mchp_handle_interrupt_pragma); \
  c_register_pragma(0, "config", mchp_handle_config_pragma); }
/* TODO
#define REGISTER_TARGET_PRAGMAS() { \

  c_register_pragma(0, "vector", mchp_handle_vector_pragma); \
  c_register_pragma(0, "config", mchp_handle_config_pragma); }
*/
/* There are no additional prefixes to try after STANDARD_EXEC_PREFIX. */
#undef MD_EXEC_PREFIX

/* There are no additional prefixes to try after STANDARD_STARTFILE_PREFIX. */
#undef MD_STARTFILE_PREFIX

/* Disallow big endian even when the command-line option is passed */
#undef TARGET_BIG_ENDIAN
#define TARGET_BIG_ENDIAN 0

#undef TARGET_STRIP_NAME_ENCODING
#define TARGET_STRIP_NAME_ENCODING mchp_strip_name_encoding

/* Disable options not supported by PIC32 */
#undef MASK_DSP
#define MASK_DSP 0
#undef MASK_DSPR2
#define MASK_DSPR2 0
#undef TARGET_DSP
#define TARGET_DSP ((target_flags & MASK_DSP) != 0)
#undef TARGET_DSPR2
#define TARGET_DSPR2 ((target_flags & MASK_DSPR2) != 0)
#undef MASK_PAIRED_SINGLE_FLOAT
#define MASK_PAIRED_SINGLE_FLOAT 0
#undef TARGET_PAIRED_SINGLE_FLOAT
#define TARGET_PAIRED_SINGLE_FLOAT ((target_flags & MASK_PAIRED_SINGLE_FLOAT) != 0)
#undef MASK_SINGLE_FLOAT
#define MASK_SINGLE_FLOAT 0
#undef TARGET_SINGLE_FLOAT
#define TARGET_SINGLE_FLOAT ((target_flags & MASK_SINGLE_FLOAT) != 0)
#undef TARGET_DOUBLE_FLOAT
#define TARGET_DOUBLE_FLOAT ((target_flags & MASK_SINGLE_FLOAT) == 0)
#undef TARGET_HARD_FLOAT_ABI
#define TARGET_HARD_FLOAT_ABI 0
#undef MASK_64BIT
#define MASK_64BIT 0
#undef TARGET_64BIT
#define TARGET_64BIT ((target_flags & MASK_64BIT) != 0)
#undef MASK_MIPS3D
#define MASK_MIPS3D 0
#undef TARGET_MIPS3D
#define TARGET_MIPS3D ((target_flags & MASK_MIPS3D) != 0)
#undef MASK_64BIT
#define MASK_64BIT 0
#undef TARGET_64BIT
#define TARGET_64BIT ((target_flags & MASK_64BIT) != 0)
#undef MASK_FLOAT64
#define MASK_FLOAT64 0
#undef TARGET_FLOAT64
#define TARGET_FLOAT64 ((target_flags & MASK_FLOAT64) != 0)
#undef MASK_OCTEON_UNALIGNED
#define MASK_OCTEON_UNALIGNED 0
#undef TARGET_OCTEON_UNALIGNED
#define TARGET_OCTEON_UNALIGNED ((target_flags & MASK_OCTEON_UNALIGNED) != 0)
#undef MASK_SMARTMIPS
#define MASK_SMARTMIPS 0
#undef TARGET_SMARTMIPS
#define TARGET_SMARTMIPS ((target_flags & MASK_SMARTMIPS) != 0)
#undef MASK_VR4130_ALIGN
#define MASK_VR4130_ALIGN 0
#undef TARGET_VR4130_ALIGN
#define TARGET_VR4130_ALIGN ((target_flags & MASK_VR4130_ALIGN) != 0)

static const int TARGET_MAD = 0;
static const int TARGET_MDMX = 0;

/* */



#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END mchp_file_end

#undef MIPS_SUBTARGET_FUNCTION_END_PROLOGUE

#undef MIPS_SUBTARGET_SET_CURRENT_FUNCTION

#undef MIPS_SUBTARGET_PREPARE_FUNCTION_START
#define MIPS_SUBTARGET_PREPARE_FUNCTION_START(fndecl) \
  mchp_prepare_function_start(fndecl)

#undef MIPS_SUBTARGET_SUPPRESS_PROLOGUE
#define MIPS_SUBTARGET_SUPPRESS_PROLOGUE() mchp_suppress_prologue()

#undef MIPS_SUBTARGET_SUPPRESS_EPILOGUE
#define MIPS_SUBTARGET_SUPPRESS_EPILOGUE() mchp_suppress_epilogue()

#undef MIPS_SUBTARGET_EXPAND_PROLOGUE_AFTER_SAVE

#undef MIPS_SUBTARGET_EXPAND_PROLOGUE_END
#define MIPS_SUBTARGET_EXPAND_PROLOGUE_END(frame) \
  mchp_expand_prologue_end(frame)

#undef MIPS_SUBTARGET_EXPAND_EPILOGUE_RESTOREREGS
#define MIPS_SUBTARGET_EXPAND_EPILOGUE_RESTOREREGS(step1,step2) \
  mchp_expand_epilogue_restoreregs(step1,step2)

#undef MIPS_SUBTARGET_EXPAND_EPILOGUE_RETURN

#undef MIPS_SUBTARGET_OUTPUT_FUNCTION_BEGIN_EPILOGUE

#undef MIPS_SUBTARGET_OUTPUT_FUNCTION_EPILOGUE
#define MIPS_SUBTARGET_OUTPUT_FUNCTION_EPILOGUE(file,size) \
  mchp_output_function_epilogue(file,size)

#undef MIPS_SUBTARGET_COMPUTE_FRAME_INFO
#define MIPS_SUBTARGET_COMPUTE_FRAME_INFO() mchp_compute_frame_info()

#undef MIPS_SUBTARGET_ATTRIBUTE_TABLE
#define MIPS_SUBTARGET_ATTRIBUTE_TABLE                                          \
    /* Microchip: allow functions to be specified as interrupt handlers */      \
    { "interrupt",        0, 1, true, true, true, mchp_interrupt_attribute },   \
    { "vector",           1, 64, true, false, false, mchp_vector_attribute },   \
    { "at_vector",        1, 1, true, false, false, mchp_at_vector_attribute }, \
    /* also allow functions to be created without prologue/epilogue code */     \
    { "naked",            0, 0, true, false, false, NULL },
#undef MIPS_DISABLE_INTERRUPT_ATTRIBUTE
#define MIPS_DISABLE_INTERRUPT_ATTRIBUTE

extern enum mips_function_type_tag current_function_type;

#undef MIPS_SUBTARGET_SAVE_REG_P

#undef MIPS_SUBTARGET_OUTPUT_FUNCTION_PROLOGUE
#define MIPS_SUBTARGET_OUTPUT_FUNCTION_PROLOGUE(file,tsize,size) \
  mchp_output_function_prologue(file,tsize,size)

#undef MIPS_SUBTARGET_EXPAND_PROLOGUE_SAVEREGS
#define MIPS_SUBTARGET_EXPAND_PROLOGUE_SAVEREGS(size,step1)  \
  mchp_expand_prologue_saveregs(size,step1)

#undef MIPS_SUBTARGET_INSERT_ATTRIBUTES
#define MIPS_SUBTARGET_INSERT_ATTRIBUTES(decl,attr_ptr) \
  mchp_target_insert_attributes (decl,attr_ptr)

#undef MIPS_SUBTARGET_FUNCTION_OK_FOR_SIBCALL

#undef MIPS_SUBTARGET_OVERRIDE_OPTIONS1
#define MIPS_SUBTARGET_OVERRIDE_OPTIONS1() \
  mchp_subtarget_override_options1()

#undef MIPS_SUBTARGET_OVERRIDE_OPTIONS2

#undef MIPS_SUBTARGET_MIPS16_ENABLED
#define MIPS_SUBTARGET_MIPS16_ENABLED(decl) \
  mchp_subtarget_mips16_enabled(decl)

#undef SUPPORTS_DISCRIMINATOR
#define SUPPORTS_DISCRIMINATOR 0

/* A few bitfield locations for the coprocessor registers */
#define CAUSE_IPL       10
#define SR_IPL          10
#define SR_IE           0
#define SR_EXL          1
#define SR_ERL          2

#endif /* MCHP_H */

