#ifndef _PIC30_H_
#define _PIC30_H_

/* Definitions of target machine for GNU compiler.
   Copyright (C) 1994, 95-98, 1999 Free Software Foundation, Inc.
   Contributed by Microchip.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/************************************************************************/
/* This is the target description macros file for the Microchip dsPIC.   */
/************************************************************************/

#include "config/mchp-cci/cci-backend.h"

#if defined(OBJECT_FORMAT_ELF)
#define   PIC30_DWARF2   1      /* ELF/DWARF default output ?   */
#else
#define   PIC30_DWARF2   0      /* ELF/DWARF default output ?   */
#endif

/*
 *  define this if you want the RTL memory address dumped out before each
 *    instruction whenever the instruction is dumped.  Useful for debugging
 *    with -dapA, but probably not how we want to release a compiler (though
 *    it'll be no uglier to the user than how the RTL is currently dumped).
 */
#define MCHP_TRACE  1

/*
 * define this to 0 or 1 depending upon the targets default value for the
 *    size of doubles  0 => large doubles
 */
#define TARGET_SHORT_DOUBLE  1

/*
 *  definitions of possible return types for pic30 operand types 
 */


/* Define the intrinsic functions for the dsPIC.  */

enum pic30_builtins
{
   PIC30_BUILTIN_READSFR,     /*  __builtin_readsfr(...) */
   PIC30_BUILTIN_WRITESFR,    /*  __builtin_writesfr(...) */
   PIC30_BUILTIN_TBLADDRESS,  /*  __builtin_tbladdress(...) */
   PIC30_BUILTIN_TBLPAGE,     /*  __builtin_tblpage(...) */
   PIC30_BUILTIN_TBLOFFSET,   /*  __builtin_tbloffset(...) */
   PIC30_BUILTIN_PSVPAGE,     /*  __builtin_psvpage(...) */
   PIC30_BUILTIN_PSVOFFSET,   /*  __builtin_psvoffset(...) */
   PIC30_BUILTIN_NOP,         /*  __builtin_nop(void) */
   PIC30_BUILTIN_DIVSD,       /*  __builtin_divsd(...) */
   PIC30_BUILTIN_MODSD,       /*  __builtin_modsd(...) */
   PIC30_BUILTIN_DIVMODSD,    /*  __builtin_divmodsd(...) */
   PIC30_BUILTIN_DIVUD,       /*  __builtin_divud(...) */
   PIC30_BUILTIN_MODUD,       /*  __builtin_modud(...) */
   PIC30_BUILTIN_DIVMODUD,    /*  __builtin_divmodud(...) */
   PIC30_BUILTIN_MULSS,       /*  __builtin_mulss(...) */
   PIC30_BUILTIN_MULSU,       /*  __builtin_mulsu(...) */
   PIC30_BUILTIN_MULUS,       /*  __builtin_mulus(...) */
   PIC30_BUILTIN_MULUU,       /*  __builtin_muluu(...) */
   PIC30_BUILTIN_BTG,         /*  __builtin_btg(...) */
   PIC30_BUILTIN_DMAPAGE,     /*  __builtin_dmapage() */
   PIC30_BUILTIN_DMAOFFSET,   /*  __builtin_dmaoffset() */
   PIC30_BUILTIN_ADDAB,
   PIC30_BUILTIN_ADD,
   PIC30_BUILTIN_CLR,
   PIC30_BUILTIN_CLR_PREFETCH,
   PIC30_BUILTIN_ED,
   PIC30_BUILTIN_EDAC,
   PIC30_BUILTIN_FBCL,
   PIC30_BUILTIN_LAC,
   PIC30_BUILTIN_MAC,
   PIC30_BUILTIN_MOVSAC,
   PIC30_BUILTIN_MPY,
   PIC30_BUILTIN_MPYN,
   PIC30_BUILTIN_MSC,
   PIC30_BUILTIN_SAC,
   PIC30_BUILTIN_SACR,
   PIC30_BUILTIN_SFTAC,
   PIC30_BUILTIN_SUBAB,
   PIC30_BUILTIN_DISI,
   PIC30_BUILTIN_ITTYPE,
   PIC30_BUILTIN_UNIQUEID,
   PIC30_BUILTIN_WRITEOSCCONL,
   PIC30_BUILTIN_WRITEOSCCONH,
   PIC30_BUILTIN_WRITERTCWEN,
   PIC30_BUILTIN_WRITENVM,
   PIC30_BUILTIN_WRITENVM_SECURE,
   PIC30_BUILTIN_TBLRDLB,
   PIC30_BUILTIN_TBLRDHB,
   PIC30_BUILTIN_TBLRDL,
   PIC30_BUILTIN_TBLRDH,
   PIC30_BUILTIN_TBLWTL,
   PIC30_BUILTIN_TBLWTH,
   PIC30_BUILTIN_TBLWTLB,
   PIC30_BUILTIN_TBLWTHB,
   PIC30_BUILTIN_DIVF,
   PIC30_BUILTIN_READ_EXT8,
   PIC30_BUILTIN_WRITE_EXT8,
   PIC30_BUILTIN_EDSPAGE,
   PIC30_BUILTIN_EDSOFFSET,
   PIC30_BUILTIN_WRITEPWMSFR,
   PIC30_BUILTIN_WRITEDISICNT,
   PIC30_BUILTIN_WRITECRYOTP,
   MCHP_BUILTIN_SECTION_BEGIN,
   MCHP_BUILTIN_SECTION_SIZE,
   MCHP_BUILTIN_SECTION_END,
   MCHP_BUILTIN_GET_ISR_STATE,
   MCHP_BUILTIN_SET_ISR_STATE,
   MCHP_BUILTIN_DISABLE_ISR,
   MCHP_BUILTIN_ENABLE_ISR
};

#define       TARGET_USE_PA   1


/*
** A C string constant that tells the GNU CC driver program options to pass to
** the assembler. It can also specify how to translate options you give to GNU
** CC into options for GNU CC to pass to the assembler. See the file `sun3.h'
** for an example of this. 
*/
#define QUOTE2(X) #X
#define QUOTE(X) QUOTE2(X)

#define ASM_SPEC   "%{!.s:%{!.S:--relax}} %{mcpu=*:-p%*} -omf=" OMF

#ifndef MCHP_CCI_CC1_SPEC
#error MCHP_CCI_CC1_SPEC not defined
#endif

#define CC1_SPEC  " \
  %(mchp_cci_cc1_spec) \
  -mresource=%I-../../c30_device.info -omf=" OMF

#define LINK_SPEC   "%{mcpu=*:-p%*} -omf=" OMF

/*
** A C string constant that tells the GNU CC driver program how to run any
** programs before the normal assembler.  This gets tacked on to the end
** of the compile line, in case any additions to the compiler line are needed.
**
** For dsPIC, this macro is used as a pre-assembly stage, for implementing
** procedure aggregation.
**
** The %(program_prefix) is used to distinguish COFF and ELF/DWARF tools.
*/
#if TARGET_USE_PA
#define       PRE_ASM_SPEC ""\
"%{pipe: %e-pipe and -mpa are incompatible} "\
"%{!mpa: %{save-temps: -o %b.s} %{!save-temps: %{!S:-o %g.s} %{S:%{!o*:-o %b.s}%{o*:%{o*}}}}\n}"  \
"%{mpa: %{save-temps: -o %b.p} %{!save-temps: %{S: -o %b.p\n} %{!S:-o %g.p}}\n" \
    "%(program_prefix)pa -omf=%(omf) %{mcpu=*:--mcpu=%*} %{mpa=*:-n%*} %{v:-v} "\
       "%{save-temps: -o%b.s}" \
       "%{!save-temps: %{!S:-o%g.s} %{S:%{!o*:-o%b.s}%{o*:%{o*}}}} "  \
       "%{save-temps: %b.p} %{!save-temps: %{S: %b.p} %{!S: %g.p}}\n}"
#endif

/*
** A C string constant that tells the GNU CC driver program how and when to
** place a reference to `libgcc.a' into the linker command line.
** This constant is placed both before and after the value of LIB_SPEC. 
** If this macro is not defined, the GNU CC driver provides a default that
** passes the string `-lgcc' to the linker unless the `-shared' option is
** specified. 
*/
#undef   LIBGCC_SPEC
#define   LIBGCC_SPEC   ""
/*
** If this macro is not defined, a default is provided that loads the
** standard C library from the usual place.
*/
#undef   LIB_SPEC
#define  ALT_LIB_SPECS
#if (PIC30_DWARF2)
#define   LIB_SPEC   "-start-group -lpic30-elf -lm-elf -lc-elf -end-group"
#define   ALT_FM_LIB_SPEC   "-start-group -lpic30-elf -lfastm-elf -lc-elf -end-group"
#define   ALT_RM_LIB_SPEC   "-start-group -lpic30-elf -lrcfastm-elf -lc-elf -end-group"
#define   ALT_LC_LIB_SPEC   "-start-group -llega-pic30-elf -lm-elf -llega-c-elf -end-group"
#define   ALT_FMLC_LIB_SPEC "-start-group -llega-pic30-elf -lfastm-elf -llega-c-elf -end-group"
#define   ALT_RMLC_LIB_SPEC "-start-group -llega-pic30-elf -lrcfastm-elf -llega-c-elf -end-group"
#else
#define   LIB_SPEC   "-start-group -lpic30-coff -lm-coff -lc-coff -end-group"
#define   ALT_FM_LIB_SPEC   "-start-group -lpic30-coff -lfastm-coff -lc-coff -end-group"
#define   ALT_RM_LIB_SPEC   "-start-group -lpic30-coff -lrcfastm-coff -lc-coff -end-group"
#define   ALT_LC_LIB_SPEC   "-start-group -llega-pic30-coff -lm-coff -llega-c-coff -end-group"
#define   ALT_FMLC_LIB_SPEC "-start-group -llega-pic30-coff -lfastm-coff -llega-c-coff -end-group"
#define   ALT_RMLC_LIB_SPEC "-start-group -llega-pic30-coff -lrcfastm-coff -llega-c-coff -end-group"
#endif

/*
** Another C string constant used much like LINK_SPEC. The difference between
** the two is that STARTFILE_SPEC is used at the very beginning of the command
** given to the linker. 
** If this macro is not defined, a default is provided that loads the standard
** C startup file from the usual place. See `gcc.c'.
*/
#undef   STARTFILE_SPEC
#define   STARTFILE_SPEC   ""

/* This macro defines names of additional specifications to put in the specs
   that can be used in various specifications like CC1_SPEC.  Its definition
   is an initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GCC driver
   program.  */
#undef EXTRA_SPECS
#define EXTRA_SPECS											\
  { "mchp_cci_cc1_spec", MCHP_CCI_CC1_SPEC },				\
  SUBTARGET_EXTRA_SPECS

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS
#endif

/* making STANDARD_EXEC_PREFIX and STANDARD_BINDIR_PREFIX point to the same
   directory will cause make_relative_paths to make no change - ie look in the 
   gcc executable's directory.  */
#undef STANDARD_EXEC_PREFIX
#undef STANDARD_BINDIR_PREFIX
#define STANDARD_EXEC_PREFIX "/bin/"
#define STANDARD_LIBEXEC_PREFIX "/bin/"
#define STANDARD_BINDIR_PREFIX "/bin/bin"

/* By default, the GCC_EXEC_PREFIX_ENV prefix is "GCC_EXEC_PREFIX", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host GCC_EXEC_PREFIX */
#ifndef GCC_EXEC_PREFIX_ENV
#define GCC_EXEC_PREFIX_ENV "PIC30_EXEC_PREFIX"
#endif

/* By default, the COMPILER_PATH_ENV is "COMPILER_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host COMPILER_PATH */
#ifndef COMPILER_PATH_ENV
#define COMPILER_PATH_ENV "PIC30_COMPILER_PATH"
#endif

/* By default, the C_INCLUDE_PATH_ENV is "C_INCLUDE_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host C_INCLUDE_PATH */
#ifndef C_INCLUDE_PATH_ENV
#define C_INCLUDE_PATH_ENV "PIC30_C_INCLUDE_PATH"
#endif

/* By default, the LIBRARY_PATH_ENV is "LIBRARY_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host LIBRARY_PATH */
#ifndef LIBRARY_PATH_ENV
#define LIBRARY_PATH_ENV "PIC30_LIBRARY_PATH"
#endif


/* define PATH to be used if C_INCLUDE_PATH is not declared 
   (and CPLUS_INCLUDE_PATH for C++, &c).  The directories are all relative
   to the current executable's directory */
extern char * pic30_default_include_path(void) __attribute__((weak));
#define DEFAULT_INCLUDE_PATH ( pic30_default_include_path ?     \
                                 pic30_default_include_path() : \
                               MPLABC30_COMMON_INCLUDE_PATH ":" \
                               MPLABC30_PIC24E_INCLUDE_PATH ":" \
                               MPLABC30_PIC24F_INCLUDE_PATH ":" \
                               MPLABC30_PIC24H_INCLUDE_PATH ":" \
                               MPLABC30_PIC30F_INCLUDE_PATH ":" \
                               MPLABC30_PIC33E_INCLUDE_PATH ":" \
                               MPLABC30_PIC33F_INCLUDE_PATH )
#ifndef TARGET_EXTRA_INCLUDES
extern void pic30_system_include_paths(const char *root, const char *system, 
                                       int nostdinc);

#define TARGET_EXTRA_INCLUDES pic30_system_include_paths
#endif

#ifdef PATH_SEPARATOR
#if PATH_SEPARATOR == ';'
#define PATH_SEPARATOR_STR ";"
#else 
#define PATH_SEPARATOR_STR ":"
#endif
#endif

#ifdef DIR_SEPARATOR
#if DIR_SEPARATOR == '\\'
#define DIR_SEPARATOR_STR "\\"
#else 
#define DIR_SEPARATOR_STR "/"
#endif
#endif

#define DEFAULT_LIB_PATH  \
         MPLABC30_COMMON_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24E_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24F_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24H_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC30F_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC33E_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC33F_LIB_PATH
        
#define DEFAULT_ERRATA_LIB_PATH  \
         MPLABC30_ERRATA_COMMON_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24E_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24F_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC24H_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC30F_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC33E_LIB_PATH PATH_SEPARATOR_STR \
         MPLABC30_PIC33F_LIB_PATH
        
#define MASK_ARCH_PIC30F         (MASK_ARCH_PIC30FXXXX | MASK_ARCH_PIC30F202X)

/*
** Small data model means that data objects can be
** addressed using the short 13-bit direct address.
*/
#define TARGET_SMALL_AGG   ((target_flags & MASK_LARGE_AGG) == 0)

/*
** Small scalar data model means that scalar objects can be
** addressed using the short 13-bit direct address.
*/
#define TARGET_SMALL_SCALAR   ((target_flags & MASK_LARGE_SCALAR) == 0)


/*
** Small code model means that calls to functions
** can be done using the short RCALL instruction.
*/
#define TARGET_SMALL_CODE   ((target_flags & MASK_LARGE_CODE) == 0)

#define TARGET_CONST_IN_CODE ((!TARGET_CONST_IN_DATA) && (!TARGET_CONST_IN_PSV))

/*
 *  Generic test to see if ARCHictecture version X is requested:
 *    current values for X are A and B
 */
#define TARGET_ARCH(X)  ((target_flags & MASK_ARCH_ ## X) != 0)

extern int target_flags;

/* 
** This macro is similar to TARGET_SWITCHES but defines names of command
** options that have values. Its definition is an initializer with a
** subgrouping for each command option. 
** Each subgrouping contains a string constant, that defines the fixed part
** of the option name, the address of a variable, and a description string.
** The variable, type char *, is set to the variable part of the given option
** if the fixed part matches. The actual option name is made by appending
** `-m' to the specified name. 
*/

extern int pic30_compiler_version;
extern const char *pic30_target_family;
extern const char *pic30_target_cpu;
extern const char *pic30_text_scn;
extern const char *pic30_errata;
extern const char *pic30_io_size;
extern const char *pic30_io_fmt;
extern const char *pic30_it_option;
extern const char *pic30_fillupper;
extern const char *pic30_resource_file;
extern const char *pic30_sfr_warning;
enum errata_mask {
  retfie_errata = 1,
  retfie_errata_disi = 2,
  psv_errata = 4,
  exch_errata = 8,
  psv_address_errata = 16
};
extern int pic30_errata_mask;

extern const char *pic30_pa_level;

/*
 *  Post-process after setting option values - checks for valid -m options
 */
#define OVERRIDE_OPTIONS pic30_override_options()

extern int         pic30_io_size_val;
extern int         pic30_clear_fn_list;
/************************************************************************/

/*----------------------------------------------------------------------*/
/*   Target machine storage layout               */
/*----------------------------------------------------------------------*/

/*
** Define this if most significant bit is lowest numbered
** in instructions that operate on numbered bit-fields.
*/
#define BITS_BIG_ENDIAN      0 

/*
** Define this if most significant byte of a word is the lowest numbered.
*/
#define BYTES_BIG_ENDIAN   0

/*
** Define this if most significant word of a multiword value is lowest numbered.
*/
#define WORDS_BIG_ENDIAN   0

/*
** Number of bits in an addressable storage unit
*/
#define BITS_PER_UNIT      8

/*
** Width in bits of a "word", which is the contents of a machine register.
** Note that this is not necessarily the width of data type `int';
** if using 16-bit ints on a 68000, this would still be 32.
** But on a machine with 16-bit registers, this would be 16.
*/
#define BITS_PER_WORD      16

/*
** Width of a word, in units (bytes).
*/
#define UNITS_PER_WORD      2

/*
** A C expression for a string describing the name of the data type to use for
** the result of subtracting two pointers. The typedef name ptrdiff_t is
** defined using the contents of the string.
*/
#define PTRDIFF_TYPE      "int"

/*
** Type to use for `size_t'. If undefined, uses `long unsigned int'.
*/
#define SIZE_TYPE      ((TARGET_BIG || TARGET_EDS) ? \
                           "long unsigned int" :            \
                           "unsigned int")

/*
** The name of the data type to use for wide characters.
*/
#define WCHAR_TYPE      "short unsigned int"

/*
** The size in bits of the data type for wide characters
*/
#define WCHAR_TYPE_SIZE      16

/*
** The name of the data type to use for wide characters passed to printf.
*/
#define   WINT_TYPE      "unsigned int"

/*
** dsPIC uses IEEE floating-point format
*/
#define TARGET_FLOAT_FORMAT   IEEE_FLOAT_FORMAT

/*
** Allocation boundary (in *bits*) for storing arguments in argument list.
*/
#define PARM_BOUNDARY      16

/*
** Boundary (in *bits*) on which stack pointer should be aligned.
*/
#define STACK_BOUNDARY      16

/*
** Allocation boundary (in *bits*) for the code of a function.
**   Now there is a distinction between program memory and data memory;
**   program memory is 16 bits wide (but really 24) and data memory is 8 bits
**   wide... reduce the function boundry, so that bits_per_unit remains at 8
*/
#define FUNCTION_BOUNDARY   16

/*
** Alignment of field after `int : 0' in a structure.
*/
#define EMPTY_FIELD_BOUNDARY   16

/*
** No data type wants to be aligned rounder than this.
*/
#define BIGGEST_ALIGNMENT   16

/*
** Biggest alignment supported by the object file format of this machine.
** Use this macro to limit the alignment which can be specified using the
** __attribute__ ((aligned (n))) construct.
** If not defined, the default value is BIGGEST_ALIGNMENT.
*/
#define MAX_OFILE_ALIGNMENT   (32768*BITS_PER_UNIT)

/*
** Define this to 1 if move instructions will actually fail to work
** when given unaligned data.
*/
#define STRICT_ALIGNMENT 1

/*
** Define number of bits in most basic integer type.
** (If undefined, default is BITS_PER_WORD).
*/
#define INT_TYPE_SIZE  16 

/*
** Define number of bits in short integer type.
** (If undefined, default is half of BITS_PER_WORD).
*/
#define SHORT_TYPE_SIZE 16

/*
** Define number of bits in long integer type.
** (If undefined, default is BITS_PER_WORD).
*/
#define LONG_TYPE_SIZE  32

/*
** Define number of bits in long long integer type.
** (If undefined, default is twice BITS_PER_WORD).
*/
#define LONG_LONG_TYPE_SIZE  64

/*
** Define number of bits in char type.
** (If undefined, default is one fourth of BITS_PER_WORD).
*/
#define CHAR_TYPE_SIZE  8

/*
** Define number of bits in float type.
** (If undefined, default is BITS_PER_WORD).
*/
#define FLOAT_TYPE_SIZE  32

/*
** Define number of bits in double type.
** (If undefined, default is twice BITS_PER_WORD).
*/
#define DOUBLE_TYPE_SIZE  64

/*
** Define number of bits in long double type.
** (If undefined, default is twice BITS_PER_WORD).
*/
#define LONG_DOUBLE_TYPE_SIZE  64

/*
** Adjust the alignment of a bitfield.
**  on dspic, usually the alignment of the underlieing type.
*/
#define PCC_BITFIELD_TYPE_MATTERS 1

/*
** On some platforms it is nice to increase alignment under certain conditions.
**  on dspic, the minimum alignment for structures is 2
*/
#define DATA_ALIGNMENT(type, align) pic30_data_alignment(type, align)

/************************************************************************/

/* Standard register usage.  */

/* Define register numbers */

/* Working/address registers */

#define WR0_REGNO    0
#define WR1_REGNO    1
#define WR2_REGNO    2
#define WR3_REGNO    3
#define WR4_REGNO    4
#define WR5_REGNO    5
#define WR6_REGNO    6
#define WR7_REGNO    7
#define WR8_REGNO    8
#define WR9_REGNO    9
#define WR10_REGNO   10
#define WR11_REGNO   11
#define WR12_REGNO   12
#define WR13_REGNO   13
#define WR14_REGNO   14
#define WR15_REGNO   15
#define RCOUNT_REGNO 16
#define A_REGNO      17
#define B_REGNO      18
#define PSVPAG       19
#define DSRPAG       19
#define PMADDR       20
#define PMMODE       21
#define PMDIN1       22
#define PMDIN2       23
#define DSWPAG       24
#define SINK0        25    /* a register that means we don't need the result */
#define SINK1        26    /* a register that means we don't need the result */
#define SINK2        27    /* a register that means we don't need the result */
#define SINK3        28    /* a register that means we don't need the result */
#define SINK4        29    /* a register that means we don't need the result */
#define SINK5        30    /* a register that means we don't need the result */
#define SINK6        31    /* a register that means we don't need the result */
#define SINK7        32    /* a register that means we don't need the result */

/*
** Number of actual hardware registers.
** The hardware registers are assigned numbers for the compiler
** from 0 to just below FIRST_PSEUDO_REGISTER.
** All registers that the compiler knows about must be given numbers,
** even those that are not normally considered general registers.
*/
#define FIRST_PSEUDO_REGISTER 33

/* Mappings for dsPIC registers */

#define   FP_REGNO   WR14_REGNO
#define   SP_REGNO   WR15_REGNO

/*
** This dsPIC-specific constant defines the highest-numbered
** register that is available for parameter passing.
*/
#define   PIC30_LAST_PARAM_REG   7

/*
** 1 for registers that have pervasive standard uses
** and are not available for the register allocator.
** W14 is the frame pointer. This is not fixed, since the frame pointer
** may not be used in certain functions.
** W15 is the stack pointer.
*/

#define FIXED_REGISTERS      \
{ \
   /* WREG0 */  0, 0, 0, 0, 0, 0, 0, 0,   \
   /* WREG8 */  0, 0, 0, 0, 0, 0, 0, 1,   \
   /* RCOUNT */ 1, 0, 0, 1, 0, 0, 0, 0,   \
   /* DSWPAG */ 1, 1, 1, 1, 1, 1, 1, 1,   \
   /* SINK7 */  1                         \
}

/*
** 1 for registers not available across function calls.
** These must include the FIXED_REGISTERS and also any
** registers that can be used without being saved.
** The latter must include the registers where values are returned
** and the register where structure-value addresses are passed.
** Aside from that, you can include as many other registers as you like.
**
** If a register has 0 in CALL_USED_REGISTERS, the compiler automatically
** saves it on function entry and restores it on function exit, if
** the register is used within the function.
**
** In short:
**   1: Caller-saved
**   0: Callee-saved
**
** For dsPIC, it makes sense that the number of callee-saved
** registers should by a multiple of two, since storew/loadw operate
** on register pairs.
** If any registers are callee-saved, then many should be saved, since
** the register allocator gives preference to callee-saved registers.
*/

#define CALL_USED_REGISTERS  \
{ \
 /* WREG0 */  1, 1, 1, 1, 1, 1, 1, 1,   \
 /* WREG8 */  0, 0, 0, 0, 0, 0, 0, 1,   \
 /* RCOUNT */ 1, 0, 0, 1, 0, 0, 0, 0,   \
 /* DSWPAG */ 1, 1, 1, 1, 1, 1, 1, 1,   \
 /* SINK7 */  1                         \
}

/*
** Order in which to allocate registers.  Each register must be
** listed once, even those in FIXED_REGISTERS.  List frame pointer
** late and fixed registers last.  Note that, in general, we prefer
** registers listed in CALL_USED_REGISTERS, keeping the others
** available for storage of persistent values.
*/

/* #define REG_ALLOC_ORDER \
   { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 }
 */

/*
** Return number of consecutive hard regs needed starting at reg REGNO
** to hold something of mode MODE.
** This is ordinarily the length in words of a value of mode MODE
** but can be less for certain modes in special long registers.
** All dsPIC registers are one word long.
*/
#define HARD_REGNO_NREGS(REGNO, MODE)   \
   ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/*
** Value is 1 if hard register REGNO (or several registers starting
** with that one) can hold a value of machine-mode MODE.
** You need not include code to check for the numbers of fixed registers,
** because the allocation mechanism considers them to be always occupied.
*/
#define HARD_REGNO_MODE_OK(REGNO, MODE)   \
   pic30_hard_regno_mode_ok(REGNO, MODE)

/*
** A C expression that is nonzero if a value of mode mode1 is accessible in
** mode mode2 without copying. If HARD_REGNO_MODE_OK (r, mode1) and
** HARD_REGNO_MODE_OK (r, mode2) are always the same for any r, then
** MODES_TIEABLE_P (mode1, mode2) should be nonzero. If they differ for any r,
** you should define this macro to return zero unless some other mechanism
** ensures the accessibility of the value in a narrower mode. You should define
** this macro to return nonzero in as many cases as possible since doing so
** will allow GCC to perform better register allocation.
*/
#define MODES_TIEABLE_P(MODE1, MODE2)   1

/*
** Specify the registers used for certain standard purposes.
** The values of these macros are register numbers.
*/

/*
** dsPIC pc isn't overloaded on a register.
*/
/* #define PC_REGNUM  */

/*
** Register to use for pushing function arguments.
*/
#define STACK_POINTER_REGNUM SP_REGNO

/*
** Base register for access to local variables of the function.
*/
#define FRAME_POINTER_REGNUM FP_REGNO

/*
** Define this macro if debugging can be performed even without a frame
** pointer. If this macro is defined, GNU CC will turn on the
** `-fomit-frame-pointer' option whenever `-O' is specified.
*/
#define   CAN_DEBUG_WITHOUT_FP   1

/*
** Base register for access to arguments of the function.
*/
#define ARG_POINTER_REGNUM FP_REGNO

/*
** Register in which static-chain is passed to a function.
*/
#define STATIC_CHAIN_REGNUM   13

/*
** If the structure value address is not passed in a register, define
** STRUCT_VALUE as an expression returning an RTX for the place where the
** address is passed. If it returns 0, the address is passed as an
** "invisible" first argument.
*/
#define STRUCT_VALUE   0

/*
** On some architectures the place where the structure value address is found
** by the called function is not the same place that the caller put it. This
** can be due to register windows, or it could be because the function prologue
** moves it to a different place. If the incoming location of the structure
** value address is in a register, define this macro as the register number.
#define STRUCT_VALUE_INCOMING
*/

/*
** Register in which address to store a structure value
** is passed to a function.
#define STRUCT_VALUE_REGNUM   WR0_REGNO
*/

/*
** Define this to be 1 if all structure return values must be in memory.
*/
#define DEFAULT_PCC_STRUCT_RETURN 1

/************************************************************************/

/*
** Define the classes of registers for register constraints in the
** machine description.  Also define ranges of constants.
**
** One of the classes must always be named ALL_REGS and include all hard regs.
** If there is more than one class, another class must be named NO_REGS
** and contain no registers.
**
** The name GENERAL_REGS must be the name of a class (or an alias for
** another name such as ALL_REGS).  This is the class of registers
** that is allowed by "g" or "r" in a register constraint.
** Also, registers outside this class are allocated only when
** instructions express preferences for them.
**
** The classes must be numbered in nondecreasing order; that is,
** a larger-numbered class must never be contained completely
** in a smaller-numbered class.
**
** For any two classes, it is very desirable that there be another
** class that represents their union.
*/

enum reg_class
{
   NO_REGS,
   A_REGS,          /* 'a': accumulator regs (w0) */
   B_REGS,          /* 'b': accumulator regs (w1) */
   C_REGS,          /* 'c': accumulator regs (w2) */
   AWB_REGS,        /*  w13 */
   PSV_REGS,        /*  PSVPAG */
   CC_REGS,         /* 'C': accumulator regs 32 bit (w2-w3) */
   ABC_REGS,        /* 'a+b+c': accumulator regs (w0..w2) */
   X_PREFETCH_REGS, /*  w8,w9: X prefetch registers */
   Y_PREFETCH_REGS, /*  w10,w11: Y prefetch registers */
   ACCUM_REGS,      /*  A,B: accumulators */
   VERY_RESTRICTED_PRODUCT_REGS,  /* w7 */
   RESTRICTED_PRODUCT_REGS,       /* W4-W6 */
   PRODUCT_REGS,    /*  w4 - w7: mac product registers  */
   E_REGS,          /* 'e': data regs (w2..w14) */
   AE_REGS,         /* 'a+e': data regs (w0,w2..w14) */
   D_REGS,          /* 'd': data regs (w1..w14) */
   W_REGS,          /* 'w': working regs (w0..w15) */
   ER_REGS,         /* even working regs (w0..w12) */
   SINK_REGS,       /* sink */
   ALL_REGS,        /* 'r': (w0..w14) */
   LIM_REG_CLASSES
};

#define N_REG_CLASSES (int) LIM_REG_CLASSES

/*
** Since GENERAL_REGS is the same class as W_REGS,
** don't give it a different class number; just make it an alias.
*/
#define GENERAL_REGS W_REGS

/*
** The class value for index registers, and the one for base regs.
*/
#define BASE_REG_CLASS  W_REGS
#define INDEX_REG_CLASS W_REGS

/*
** Give names of register classes as strings for dump file.
*/

#define REG_CLASS_NAMES \
{ \
  "NOREG",      \
  "W0REG",      \
  "W1REG",      \
  "W2REG",      \
  "AWB",        \
  "PSV",        \
  "W2REG32",    \
  "W0+W1+W2",   \
  "W8..W9",     \
  "W10..W11",   \
  "A..B",       \
  "W7",         \
  "W4..W6",     \
  "W4..W7",     \
  "W2..W14",    \
  "W0+W2..W14", \
  "W1..W14",    \
  "W0..W15",    \
  "ERREGS",     \
  "SINK",       \
  "ALL_REGS"    \
}

/*
** Define which registers fit in which classes.
** This is an initializer for a vector of HARD_REG_SET
** of length N_REG_CLASSES.
** All dsPIC registers may be used as either BASE or INDEX registers.
** Register W0 may be used as PIC-compatible working registers.
*/

/*
 * sink registers can be ypf/xpf/regs so that reload doesn't try to reload them
 *
 */

#define REG_CLASS_CONTENTS   \
{ \
        { 0x00000000, 0x00000000 }, \
        { 0x00000001, 0x00000000 }, \
        { 0x00000002, 0x00000000 }, \
        { 0x00000004, 0x00000000 }, \
/*AWB*/ { 0x00002000, 0x00000000 }, \
/*PSV*/ { 0x01080000, 0x00000000 }, \
        { 0x0000000c, 0x00000000 }, \
        { 0x00000007, 0x00000000 }, \
/*XPF*/ { 0x00000300, 0x00000000 }, \
/*YPF*/ { 0x00000c00, 0x00000000 }, \
/*ACC*/ { 0x00060000, 0x00000000 }, \
/*VRP*/ { 0x00000080, 0x00000000 }, \
/* RP*/ { 0x00000070, 0x00000000 }, \
/*PRG*/ { 0x000000f0, 0x00000000 }, \
        { 0x0000fffc, 0x00000000 }, \
        { 0x0000fffd, 0x00000000 }, \
        { 0x0000fffe, 0x00000000 }, \
        { 0xff08ffff, 0x00000001 }, \
/*ER*/  { 0x00001555, 0x00000000 }, \
        { 0xfe000000, 0x00000001 }, \
        { 0x0000ffff, 0x00000000 }  \
}

/*
** The same information, inverted:
** Return the class number of the smallest class containing
** reg number REGNO.  This could be a conditional expression
** or could index an array.
*/
#define IS_EVEN_REG(r)              (((r)&1)==0)
#define IS_QUAD_REG(r)              (((r)&3)==0)
#define IS_PSEUDO_REG(r)            ((r) >= FIRST_PSEUDO_REGISTER)
#define IS_AREG_REG(r)              ((r) == WR0_REGNO)
#define IS_BREG_REG(r)              ((r) == WR1_REGNO)
#define IS_CREG_REG(r)              ((r) == WR2_REGNO)
#define IS_AWB_REG(r)               ((r) == WR13_REGNO)
#define IS_DREG_REG(r)              (((r) >= WR1_REGNO) && IS_WREG_REG(r))
#define IS_EREG_REG(r)              (((r) >= WR2_REGNO) && IS_WREG_REG(r))
#define IS_WREG_REG(r)              ((r) <= WR15_REGNO)
#define IS_AREG_OR_PSEUDO_REG(r)    (IS_AREG_REG(r) || IS_PSEUDO_REG(r))
#define IS_BREG_OR_PSEUDO_REG(r)    (IS_BREG_REG(r) || IS_PSEUDO_REG(r))
#define IS_CREG_OR_PSEUDO_REG(r)    (IS_CREG_REG(r) || IS_PSEUDO_REG(r))
#define IS_EREG_OR_PSEUDO_REG(r)    (IS_EREG_REG(r) || IS_PSEUDO_REG(r))
#define IS_AREG_OR_PSEUDO_REGNO(op) (IS_AREG_OR_PSEUDO_REG(REGNO(op)))
#define IS_BREG_OR_PSEUDO_REGNO(op) (IS_BREG_OR_PSEUDO_REG(REGNO(op)))
#define IS_CREG_OR_PSEUDO_REGNO(op) (IS_CREG_OR_PSEUDO_REG(REGNO(op)))
#define IS_EREG_OR_PSEUDO_REGNO(op) (IS_EREG_OR_PSEUDO_REG(REGNO(op)))
#define IS_VRP_REG(r)               ((r) == WR7_REGNO)
#define IS_RP_REG(r)                (((r) >= WR4_REGNO) && ((r) <= WR6_REGNO))
#define IS_PRODUCT_REG(r)           (((r) >= WR4_REGNO) && ((r) <= WR7_REGNO))
#define IS_XPREFETCH_REG(r)         (((r) == WR8_REGNO) || ((r) == WR9_REGNO))
#define IS_YPREFETCH_REG(r)         (((r) == WR10_REGNO) || ((r) == WR11_REGNO))
#define IS_PSV_REG(r)               (((r) == PSVPAG) || ((r) == DSWPAG))
#ifdef IGNORE_ACCUM_CHECK
#define MAYBE_IS_ACCUM_REG(r)       (1)
#else
#define MAYBE_IS_ACCUM_REG(r)       (((r) == A_REGNO) || ((r) == B_REGNO))
#endif
#define IS_ACCUM_REG(r)             (((r) == A_REGNO) || ((r) == B_REGNO))
#define OTHER_ACCUM_REG(r)          ((r == A_REGNO) ? B_REGNO : A_REGNO)
#define IS_SINK_REG(r)              ((r >= SINK0) && (r <= SINK7))


#define REGNO_REG_CLASS(REGNO) (   \
   IS_AREG_REG(REGNO) ? A_REGS :   \
   IS_BREG_REG(REGNO) ? B_REGS :   \
   IS_CREG_REG(REGNO) ? C_REGS :   \
   IS_AWB_REG(REGNO) ? AWB_REGS : \
   IS_VRP_REG(REGNO) ? VERY_RESTRICTED_PRODUCT_REGS : \
   IS_RP_REG(REGNO) ? RESTRICTED_PRODUCT_REGS : \
   IS_PRODUCT_REG(REGNO) ? PRODUCT_REGS : \
   IS_XPREFETCH_REG(REGNO) ? X_PREFETCH_REGS : \
   IS_YPREFETCH_REG(REGNO) ? Y_PREFETCH_REGS : \
   IS_ACCUM_REG(REGNO) ? ACCUM_REGS : \
   IS_EREG_REG(REGNO) ? E_REGS :   \
   IS_DREG_REG(REGNO) ? D_REGS :   \
   IS_WREG_REG(REGNO) ? W_REGS :   \
   IS_PSV_REG(REGNO) ? PSV_REGS :  \
   IS_SINK_REG(REGNO) ? SINK_REGS : \
   ALL_REGS)

/*
 * USED CONSTRAINT LETTERS:
  
 =   - result
 +   - read/write
 &   - early clobber
 *   - ignore constraint
 ?   - disparage
 %   - commutative
 !   - disparage more
 #   - ignore this alternative for reloading
 <   - pre/post decrement
 >   - pre/post increment
 0-9 - match opnd n
 a   - [C30 - see below]
 A   - [C30 - see below]
 b   - [C30 - see below]
 B   - [C30 - see below]
 c   - [C30 - see below]
 C   - [C30 - see below]
 d   - [C30 - see below]
 D   - [C3- - see below]
 e   - [C30 - see below]
 E   - const double vector
 f   - unused
 F   - const double vector
 g   - 
 G   - const double
 h   - unused
 H   - const double
 i   - integer
 I   - const int matching CONST_OK_FOR_CONSTRAINT_P
 j   - unused
 J   - const int matching CONST_OK_FOR_CONSTRAINT_P
 k   - unused
 K   - const int matching CONST_OK_FOR_CONSTRAINT_P
 l   - unused
 L   - const int matching CONST_OK_FOR_CONSTRAINT_P
 m   - memory operand
 M   - const int matching CONST_OK_FOR_CONSTRAINT_P
 n   - const int or const double
 N   - const int matching CONST_OK_FOR_CONSTRAINT_P
 o   - offsetable memory operand
 O   - const int matching CONST_OK_FOR_CONSTRAINT_P
 p   - pointer
 P   - const int matching CONST_OK_FOR_CONSTRAINT_P
 q   - [C30 - see below]
 Q   - [C30 - see below]
 r   - general register
 R   - [C30 - see below]
 s   - cosnt int or const double
 S   - [C30 - see below]
 t   - [C30 - see below]
 T   - [C30 - see below]
 u   - [C30 - see below]
 U   - [C30 - see below]
 v   - [C30 - see below]
 V   - non-offsetable memory operand
 w   - [C30 - see below]
 W   - const int matching CONST_OK_FOR_CONSTRAINT_P [C30]
 x   - [C30 - see below]
 X   - always a winner
 y   - [C30 - see below]
 Y   - const int matching CONST_OK_FOR_CONSTRAINT_P [C30]
 z   - [C30 - see below]
 Z   - const int matching CONST_OK_FOR_CONSTRAINT_P [C30]

 *
 */


/*
** A C expression which defines the machine-dependent operand constraint
** letters for register classes. If char is such a letter, the value should
** be the register class corresponding to it. Otherwise, the value should be
** NO_REGS. The register letter `r', corresponding to class GENERAL_REGS,
** will not be passed to this macro; you do not need to handle it.
**
** For the dsPIC, 
** `a' is the class of PIC-compatible accumulator registers (W0).
** 'A' is the class of PIC-compatible accumulator registers 32bit (W0-W1).
** `b' is the class of divide support registers (W1).
** `B' is the class of blank sink registers used in DSP instructions.
** `c' is the class of multiply support registers (W2).
** `C' is the class of multiply support registers 32bit (W2-W3).
** `d' is the class of general-purpose data registers (W1..W14).
** `D' is the class of even general-purpose data registers (W0..W12).
** `e' is the class of non-divide support general registers (W2..W14).
** 't' is the class of very restricted product registes (W7)
** 'u' is the class of restricted product registers (W4-W6)
** 'v' is the class of AWB registers (W13).
** 'w' is the class of ACCumulator registers (A..B).
** `x' is the class of x prefetch registers (W8..W9).
** `y' is the class of y prefetch registers (W10..W11).
** `z' is the class of mac product registers (W4..W7).
** `E' is sink
*/

#define REG_CLASS_FROM_LETTER(Q) (      \
     ((Q) == 'e') ? E_REGS :            \
     ((Q) == 'd') ? D_REGS :            \
     ((Q) == 'D') ? ER_REGS :           \
     ((Q) == 'c') ? C_REGS :            \
     ((Q) == 'C') ? CC_REGS :           \
     ((Q) == 'b') ? B_REGS :            \
     ((Q) == 'a') ? A_REGS :            \
     ((Q) == 't') ? VERY_RESTRICTED_PRODUCT_REGS : \
     ((Q) == 'u') ? RESTRICTED_PRODUCT_REGS :      \
     ((Q) == 'v') ? AWB_REGS :          \
     ((Q) == 'w') ? ACCUM_REGS :        \
     ((Q) == 'x') ? X_PREFETCH_REGS :   \
     ((Q) == 'y') ? Y_PREFETCH_REGS :   \
     ((Q) == 'z') ? PRODUCT_REGS :      \
     ((Q) == 'B') ? SINK_REGS:          \
     NO_REGS )

/*
** Return the maximum number of consecutive registers
** needed to represent mode MODE in a register of class CLASS.
** On the dsPIC, this is the size of MODE in words,
** since class doesn't make any difference.
*/
#define CLASS_MAX_NREGS(CLASS,MODE) \
   ((GET_MODE_SIZE (MODE) + UNITS_PER_WORD - 1) / UNITS_PER_WORD)

/*
** Return the register class of a scratch register needed to load
** or store a register of class CLASS in MODE.
** For dsPIC, we can only load/store QImode using WREG.
*/
/*
#define SECONDARY_RELOAD_CLASS(CLASS, MODE, X)         \
   pic30_secondary_reload_class(CLASS, MODE, X)
*/

/*
** On some machines, it is risky to let hard registers live across arbitrary
** insns. Typically, these machines have instructions that require values to
** be in specific registers (like an accumulator), and reload will fail if the
** required hard register is used for another purpose across such an insn.
** Define SMALL_REGISTER_CLASSES to be an expression with a non-zero value on
** these machines. When this macro has a non-zero value, the compiler will try
** to minimize the lifetime of hard registers. It is always safe to define this
** macro with a non-zero value, but if you unnecessarily define it, you will
** reduce the amount of optimizations that can be performed in some cases.
** If you do not define this macro with a non-zero value when it is required,
** the compiler will run out of spill registers and print a fatal error
** message. For most machines, you should not define this macro at all.
#define   SMALL_REGISTER_CLASSES   1
*/

/************************************************************************/
/* Macros to check register numbers against specific register classes.  */
/************************************************************************/

/*
** These assume that REGNO is a hard or pseudo reg number.
** They give nonzero only if REGNO is a hard reg of the suitable class
** or a pseudo reg currently allocated to a suitable hard reg.
** Since they use reg_renumber, they are safe only once reg_renumber
** has been allocated, which happens in local-alloc.c. 
*/

#define   IS_BASE_REG(REGNO) ((0 <= (REGNO)) && ((REGNO) <= 15))
#define   IS_INDX_REG(REGNO) ((0 <= (REGNO)) && ((REGNO) <= 15))

#define REGNO_OK_FOR_BASE_P(REGNO)  \
 (IS_BASE_REG(REGNO) || IS_BASE_REG(reg_renumber[REGNO]))
#define REGNO_OK_FOR_INDEX_P(REGNO) \
 (IS_INDX_REG(REGNO) || IS_INDX_REG(reg_renumber[REGNO]))

/*
** Maximum number of registers that can appear in a valid memory address.
*/
#define MAX_REGS_PER_ADDRESS 2

/*
** Given an rtx X being reloaded into a reg required to be
** in class CLASS, return the class of reg to actually use.
** In general this is just CLASS; but on some machines
** in some cases it is preferable to use a more restrictive class.
*/
#define PREFERRED_RELOAD_CLASS(X,CLASS) \
   pic30_preferred_reload_class(X,CLASS)

/*
** How to refer to registers in assembler output.
** This sequence is indexed by compiler's hard-register-number (see above).
*/
#define REGISTER_NAMES \
{ \
 "w0", "w1", "w2", "w3", "w4", "w5", "w6", "w7",   \
 "w8", "w9","w10","w11","w12","w13","w14","w15",   \
 "_RCOUNT", "A", "B", "PSVPAG", "PMADDR", "PMMODE", "PMDIN1", "PMDIN2", \
 "DSWPAG","SINK0", "SINK1", "SINK2", "SINK3", "SINK4", "SINK5", "SINK6", \
 "SINK7" \
}

/*
** The letters I,J,K,... to P in an operand constraint string
** can be used to stand for particular ranges of immediate operands.
** This macro defines what the ranges are.
** C is the letter, and VALUE is a constant value.
** Return 1 if VALUE is in the range specified by C.
**
** For the dsPIC, 
** `I' is a synonym for (const_int 1)
** `J' is used for positive 10-bit literals
** `K' is used for the mode1 constant range,
** `L' is not used
** `M' is used for negative 10-bit literals
** `N' is used for negative 5-bit literals
** `O' is a synonym for (const_int 0).
** `P' is used for positive 5-bit literals
** 'W' is used for -16 <= x <= 16
** 'Y' is used for -6,-4,-2,2,4,6
** 'Z' is for signed 4-bit literals
*/
#define   PIC30_DISP_MIN   -512   /* min(k) in [Wn+k] addressing   */
#define   PIC30_DISP_MAX    511   /* max(k) in [Wn+k] addressing   */
#define   PIC30_LNK_MAX   16382   /* lnk #lit14 (even)      */
#define   PIC30_ADD_MAX   511   /* add #Slit10         */

#define CONST_OK_FOR_LETTER_P(VALUE, C) \
( \
   (C) == 'I' ?                                                             \
     ((HOST_WIDE_INT)VALUE) == 1 :                                          \
   (C) == 'J' ?                                                             \
      0 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) <= 1023 :       \
   (C) == 'K' ?                                                             \
      0 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) <= 15 :         \
   (C) == 'L' ?                                                             \
      0 != ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) <= 2 &&         \
      ((HOST_WIDE_INT)VALUE) >= -2 :                                        \
   (C) == 'M' ?                                                             \
      -1023 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) < 0 :       \
   (C) == 'N' ?                                                             \
      -31 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) < 0 :         \
   (C) == 'O' ?                                                             \
      ((HOST_WIDE_INT)VALUE) == 0 :                                         \
   (C) == 'P' ?                                                             \
      0 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) <= 31 :         \
   (C) == 'W' ?                                                             \
      ((-16 <= (HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE <= 16)) :     \
   (C) == 'Y' ?                                                             \
      (((HOST_WIDE_INT)VALUE == -6) || ((HOST_WIDE_INT)VALUE == -4) ||      \
      ((HOST_WIDE_INT)VALUE == -2) || ((HOST_WIDE_INT)VALUE == 2) ||        \
      ((HOST_WIDE_INT)VALUE == 4) || ((HOST_WIDE_INT)VALUE == 6)) :         \
   (C) == 'Z' ?                                                             \
      -8 <= ((HOST_WIDE_INT)VALUE) && ((HOST_WIDE_INT)VALUE) <= 7 :         \
    0)

/* Similar, but for floating constants, and defining letter 'G'.
   Here VALUE is the CONST_DOUBLE rtx itself.  */
#define CONST_DOUBLE_OK_FOR_LETTER_P(VALUE, C)  \
  ((C) == 'G' ? ((VALUE) == CONST0_RTX (SFmode)         \
                 || (VALUE) == CONST0_RTX (DFmode)) : 0)

/*
** Optional extra constraints for this machine.
**
** For the dsPIC,
** Q: memory operand consisting of a base plus displacement.
** R: memory operand consisting of a base only.
** S: memory operand consisting of a base plus index.
** T: memory operand consisting of a direct address (far).
** U: memory operand consisting of a direct address (near).
*/

#define EXTRA_CONSTRAINT(OP, C) \
     (((C) == 'Q') ? (pic30_Q_constraint(OP)) \
    : ((C) == 'q') ? (pic30_q_constraint(OP)) \
    : ((C) == 'R') ? (pic30_R_constraint(OP)) \
    : ((C) == 'S') ? (pic30_S_constraint(OP)) \
    : ((C) == 'T') ? (pic30_T_constraint(OP)) \
    : ((C) == 'U') ? (pic30_U_constraint(OP,VOIDmode)) \
    : 0 )

/*
 * we don't want to reload memory addresses, our machine can handle it 
 *  but we do need to set these things so that asm statements can refer to
 *  memory address contraint letters :(
 */
#define EXTRA_MEMORY_CONSTRAINT(C, STR) \
  (reload_in_progress ?  0 : (((C) == 'S') || ((C) == 'T') || ((C) == 'U')))

#define EXTRA_ADDRESS_CONSTRAINT(C, STR) \
  (reload_in_progress ?  0 : (((C) == 'q')))


/************************************************************************/

/* Stack layout; function entry, exit and calling.  */

/*
** Define this if pushing a word on the stack
** makes the stack pointer a smaller address.
#define STACK_GROWS_DOWNWARD   1
*/

/*
** We use post increment on the dsPIC because the
** stack grows upward (toward higher addresses). This means that we
** assume the stack pointer always points at the next
** FREE location on the stack.
*/
#define STACK_PUSH_CODE POST_INC

#define STACK_POP_CODE PRE_DEC

/*
** Define this if the addresses of local variable slots
** are at negative offsets from the frame pointer.
** For the dsPIC, they are not.
#define FRAME_GROWS_DOWNWARD   1
*/

/*
** Define this if successive args to a function occupy decreasing addresses
** on the stack. 
** For the dsPIC, this is defined, since argumenst are pushed
** in the conventional right-to-left order, meaning that successive arguments
** (in left-to-right order) occupy decreasing addresses.
*/
#define ARGS_GROW_DOWNWARD   1

/*
** Offset from the frame pointer to the first local variable
** slot to be allocated.
** If FRAME_GROWS_DOWNWARD, find the next slot`s offset by
** subtracting the first slot`s length from STARTING_FRAME_OFFSET.
** Otherwise it is found by adding the length of the first slot to
** the value of STARTING_FRAME_OFFSET.
** For dsPIC, [FP] initially points to an unused stack word.
** Thus, for [FP] to directly address the first auto word, we have:
*/
#define STARTING_FRAME_OFFSET   0

/*
** Offset from the stack pointer register to the first location at which
** outgoing arguments are placed. If not specified, the default value of
** zero is used. This is the proper value for most machines.
** If ARGS_GROW_DOWNWARD, this is the offset to the location above the
** first location at which outgoing arguments are placed.
*/
#define   STACK_POINTER_OFFSET   0

/*
** Offset of first parameter from the argument pointer register value.
** If ARGS_GROW_DOWNWARD, this is the offset to the location above the
** first argument's address (which is the case for dsPIC).
** dsPIC note:
** FP points to the 1st local. Let n be the size of the auto/temp area.
** [FP+0] = 1st auto/temp
** [FP-2] = old FP
** [FP-4] = return address MSW
** [FP-6] = return address LSW
** [FP-8] = 1st parameter
*/

#define FIRST_PARM_OFFSET(FNDECL) (-6)

/*
** Offset of the CFA from the argument pointer register value.
** The CFA (canonical frame address) is defined to be the value of the
** stack pointer in the previous frame, just before the call.
** Hence, 6.
*/
#define ARG_POINTER_CFA_OFFSET(FNDECL)   -6

/*
** If STACK_CHECK_BUILTIN is zero, and you define a named pattern called
** check_stack in your 'md' file, GCC will call that pattern with one
** argument which is the address to compare the stack value against.
** You must arrange for this pattern to report an error if the stack
** pointer is out of range.
** For dsPIC, stack checking is done in hardware.
*/
#define   STACK_CHECK_BUILTIN   1

/*
** The number of bytes of stack needed to recover from a stack overflow,
** for languages where such a recovery is supported. The default value of
** 75 words should be adequate for most machines.
*/
#define STACK_CHECK_PROTECT   0

/*
** The maximum size of a stack frame, in bytes. GCC will generate
** probe instructions in non-leaf functions to ensure that at least
** this many bytes of stack are available. If a stack frame is larger
** than this size, stack checking will not be reliable and GCC will issue
** a warning. The default is chosen so that GCC only generates one instruction
** on most systems. You should not normally change the value of this macro.
*/
#define   STACK_CHECK_MAX_FRAME_SIZE   512

/*
** Given an rtx for the address of a frame,
** return an rtx for the address of the word in the frame
** that holds the dynamic chain--the previous frame's address.
*/
#define DYNAMIC_CHAIN_ADDRESS(frameaddr) \
  gen_rtx_PLUS (Pmode, frameaddr, GEN_INT (-1 * UNITS_PER_WORD))

/*
** A C expression whose value is RTL representing the value of the
** return address for the frame count steps up from the current frame,
** after the prologue. FRAMEADDR is the frame pointer of the COUNT frame.
** dsPIC return addresses are canonically located at [FP-6].
*/
#define RETURN_ADDR_RTX(count, frameaddr) \
   pic30_return_addr_rtx(count, frameaddr)

/*
** A C expression whose value is RTL representing the location of the
** incoming return address at the begining of any function, before
** the prologue.
** For dsPIC, before the prologue, the return address is [SP-4].
*/
#define INCOMING_RETURN_ADDR_RTX \
  gen_rtx_PLUS(Pmode, gen_rtx_REG(Pmode, SP_REGNO), \
               GEN_INT(-2 * UNITS_PER_WORD))

/*
** A C expression whose value is an integer giving the offset, in bytes,
** from the value of the stack pointer register to the top of the stack frame
** at the beginning of any function, before the prologue. The top of the frame
** is defined to be the value of the stack pointer in the previous frame, just
** before the call instruction. You only need to define this macro if you want
** to support call frame debugging information like that provided by DWARF 2.
*/
#define INCOMING_FRAME_SP_OFFSET -4

/*
** If we generate an insn to push BYTES bytes,
** this says how many the stack pointer really advances by.
** For dsPIC, this is always an even number.
*/

#define PUSH_ROUNDING(BYTES) (((BYTES) + 1) & ~1)

/* Define this macro if functions should assume that stack space has
   been allocated for arguments even when their values are passed in
   registers.
   Size, in bytes, of the area reserved for arguments passed in
   registers for the function represented by FNDECL. 
   #define REG_PARM_STACK_SPACE(FNDECL) 14 */

/* Define this if it is the responsibility of the caller to allocate
   the area reserved for arguments passed in registers. 
   #define OUTGOING_REG_PARM_STACK_SPACE */

/*
** Value is 1 if returning from a function call automatically
** pops the arguments described by the number-of-args field in the call.
** FUNDECL is the declaration node of the function (as a tree),
** FUNTYPE is the data type of the function (as a tree),
** or for a library call it is an identifier node for the subroutine name.
*/

#define RETURN_POPS_ARGS(FUNDECL,FUNTYPE,SIZE) 0

/*
** Define how to find the value returned by a function.
** VALTYPE is the data type of the value (as a tree).
** If the precise function being called is known, FUNC is its FUNCTION_DECL;
** otherwise, FUNC is 0.
*/

#define FUNCTION_VALUE(VALTYPE, FUNC)  \
  gen_rtx_REG(TYPE_MODE(VALTYPE),WR0_REGNO)

/*
** Define how to find the value returned by a library function
** assuming the value has mode MODE.
*/

#define LIBCALL_VALUE(MODE)  \
   gen_rtx_REG(MODE, WR0_REGNO)   /* Return in W0 */

/* Implicit Calls to Library Routines  */


/* 1 if N is a possible register number for a function value. */

#define FUNCTION_VALUE_REGNO_P(N)  ((N) == 0)

/* 1 if the tree TYPE should be returned in memory instead of in regs. 
   #define RETURN_IN_MEMORY(TYPE) \
   (int_size_in_bytes(TYPE) > 12)
*/

/* Define this if PCC uses the nonreentrant convention for returning
   structure and union values. 
   #define PCC_STATIC_STRUCT_RETURN  */

/************************************************************************/

/*
** Define a data type for recording info about an argument list
** during the scan of that argument list.  This data type should
** hold all necessary information about the function itself
** and about the args processed so far, enough to enable macros
** such as FUNCTION_ARG to determine where the next arg should go.
**
** For dsPIC, this is a single integer, which is a number of words
** of arguments scanned so far.
*/
typedef struct pic30_args
{
   int   parmregs[8];
}
   CUMULATIVE_ARGS;

/*
** Initialize a variable CUM of type CUMULATIVE_ARGS
** for a call to a function whose data type is FNTYPE.
** For a library call, FNTYPE is 0.
**
** For dsPIC, the offset starts at 0.
*/
/* CW N_NAMED_ARGS is a new parameter */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,INDIRECT,N_NAMED_ARGS) \
  (pic30_init_cumulative_args (&CUM, FNTYPE, LIBNAME))

/*
** Update the data in CUM to advance over an argument
** of mode MODE and data type TYPE.
** (TYPE is null for libcalls where that information may not be available.)
*/
#define FUNCTION_ARG_ADVANCE(CUM, MODE, TYPE, NAMED)   \
  (pic30_function_arg_advance (&CUM, MODE, TYPE, NAMED))

/* 1 if N is a possible register number for function argument passing. */

#define FUNCTION_ARG_REGNO_P(N)  ((N) <= PIC30_LAST_PARAM_REG)

/*
** Define where to put the arguments to a function.
** Value is zero to push the argument on the stack,
** or a hard register in which to store the argument.
**
** MODE is the argument's machine mode.
** TYPE is the data type of the argument (as a tree).
**  This is null for libcalls where that information may
**  not be available.
** CUM is a variable of type CUMULATIVE_ARGS which gives info about
**  the preceding args and about the function being called.
** NAMED is nonzero if this argument is a named parameter
**  (otherwise it is an extra parameter matching an ellipsis).
*/
#define FUNCTION_ARG(CUM, MODE, TYPE, NAMED) \
   pic30_function_arg (&CUM,MODE,TYPE,NAMED)

/*
** Define the following macro if function calls on the target machine
** do not preserve any registers; in other words, if `CALL_USED_REGISTERS'
** has 1 for all registers. This macro enables `-fcaller-saves' by
** default. Eventually that option will be enabled by default on all
** machines and both the option and this macro will be eliminated.
#define DEFAULT_CALLER_SAVES
*/

/*
** A C statement or compound statement to output to file some assembler code
** to call the profiling subroutine mcount. Before calling, the assembler code
** must load the address of a counter variable into a register where mcount
** expects to find the address. The name of this variable is `LP' followed by
** the number labelno, so you would generate the name using `LP%d' in a
** fprintf. 
** The details of how the address should be passed to mcount are determined by
** your operating system environment, not by GNU CC. To figure them out,
** compile a small program for profiling using the system's installed C
** compiler and look at the assembler code that results. 
*/
#define FUNCTION_PROFILER(FILE, LABELNO) pic30_function_profiler(FILE, LABELNO)

/*
** Define this macro as a C expression that is nonzero if the return
** instruction or the function epilogue ignores the value of the stack pointer;
** in other words, if it is safe to delete an instruction to adjust the stack
** pointer before a return from the function. Note that this macro's value is
** relevant only for functions for which frame pointers are maintained. It is
** never safe to delete a final stack adjustment in a function that has no
** frame pointer, and the compiler knows this regardless of EXIT_IGNORE_STACK.
*/
#define EXIT_IGNORE_STACK 0

/*
** If defined, this macro specifies a table of register pairs used to eliminate
** unneeded registers that point into the stack frame. If it is not defined,
** the only elimination attempted by the compiler is to replace references to
** the frame pointer with references to the stack pointer. The definition of
** this macro is a list of structure initializations, each of which specifies
** an original and replacement register. On some machines, the position of the
** argument pointer is not known until the compilation is completed. In such a
** case, a separate hard register must be used for the argument pointer. This
** register can be eliminated by replacing it with either the frame pointer or
** the argument pointer, depending on whether or not the frame pointer has been
** eliminated.
*/
#define ELIMINABLE_REGS { \
   { ARG_POINTER_REGNUM, STACK_POINTER_REGNUM },  \
   { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM } }

/*
** This macro is similar to INITIAL_FRAME_POINTER_OFFSET.
** It specifies the initial difference between the specified pair of registers.
** This macro must be defined if ELIMINABLE_REGS is defined.
*/
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET) \
   pic30_initial_elimination_offset(FROM, TO, &(OFFSET))

/*
** Length in units of the trampoline for entering a nested function.
*/
#define TRAMPOLINE_SIZE 2

/************************************************************************/
/* Addressing modes, and classification of registers for them.      */
/************************************************************************/

/*
** A C expression that is non-zero if the machine supports the addressing mode
*/
#define HAVE_POST_INCREMENT   1
#define HAVE_POST_DECREMENT   1 
#define HAVE_PRE_INCREMENT   1
#define HAVE_PRE_DECREMENT   1
/*
** A C expression that is non-zero if the machine supports 
** pre- or post-address side-effect generation involving constants
** other than the size of the memory operand.
*/
#define HAVE_PRE_MODIFY_DISP   0
#define HAVE_POST_MODIFY_DISP   0

/*
** Recognize any constant value that is a valid address.
*/
#define CONSTANT_ADDRESS_P(X)  CONSTANT_P(X)

/*
** Nonzero if the constant value X is a legitimate general operand.
** It is given that X satisfies CONSTANT_P or is a CONST_DOUBLE.
*/
#define LEGITIMATE_CONSTANT_P(X) 1

/*
** The macros REG_OK_FOR..._P assume that the arg is a REG rtx
** and check its validity for a certain class.
** We have two alternate definitions for each of them.
** The usual definition accepts all pseudo regs; the other rejects
** them unless they have been allocated suitable hard regs.
** The symbol REG_OK_STRICT causes the latter definition to be used.
**
** Most source files want to accept pseudo regs in the hope that
** they will get allocated to the class that the insn wants them to be in.
** Source files for reload pass need to be strict.
** After reload, it makes no difference, since pseudo regs have
** been eliminated by then.
*/

#ifdef REG_OK_STRICT

/* Nonzero if X is a hard reg that can be used as an index.  */
#define REG_OK_FOR_INDEX_P(X) REGNO_OK_FOR_INDEX_P(REGNO(X))
/* Nonzero if X is a hard reg that can be used as a base reg.  */
#define REG_OK_FOR_BASE_P(X)  REGNO_OK_FOR_BASE_P(REGNO(X))

#else

/*
** Nonzero if X is a hard reg that can be used as an index
** or if it is a pseudo reg.
*/
#define REG_OK_FOR_INDEX_P(X) (REGNO(X) >= 0)

/*
** Nonzero if X is a hard reg that can be used as a base reg
** or if it is a pseudo reg.
*/
#define REG_OK_FOR_BASE_P(X) (REGNO(X) >= 0)

#endif

/*
** Go to LABEL if ADDR (a legitimate address expression)
** has an effect that depends on the machine mode it is used for.
**
** dsPIC: this applies to auto-increment and auto-decrement,
** since the size of the increment depends on the operand type.
*/

#define GO_IF_MODE_DEPENDENT_ADDRESS(ADDR,LABEL) \
 if (GET_CODE (ADDR) == POST_INC \
   || GET_CODE (ADDR) == POST_DEC \
   || GET_CODE (ADDR) == PRE_DEC \
   || GET_CODE (ADDR) == PRE_INC) \
   goto LABEL

/*
** Define this macro if references to a symbol must be treated
** differently depending on something about the variable or
** function named by the symbol (such as what section it is in).
**
** The macro definition, if any, is executed immediately after the
** rtl for DECL or other node is created.
** The value of the rtl will be a `mem' whose address is a
** `symbol_ref'.
**
** The usual thing for this macro to do is to a flag in the
** `symbol_ref' (such as `SYMBOL_REF_FLAG') or to store a modified
** name string in the `symbol_ref' (if one bit is not enough
** information).
**
** On the dsPIC we use this to indicate if a symbol is in
** code or data or sfr space.
*/

/* the flags may be any length if surrounded by | */
#define PIC30_EXTENDED_FLAG  "|"
#define PIC30_PROG_FLAG       PIC30_EXTENDED_FLAG "pm"      PIC30_EXTENDED_FLAG
#define PIC30_DATA_FLAG       PIC30_EXTENDED_FLAG "dm"      PIC30_EXTENDED_FLAG
#define PIC30_X_FLAG          PIC30_EXTENDED_FLAG "xm"      PIC30_EXTENDED_FLAG
#define PIC30_Y_FLAG          PIC30_EXTENDED_FLAG "ym"      PIC30_EXTENDED_FLAG
#define PIC30_EXT_FLAG        PIC30_EXTENDED_FLAG "ext"     PIC30_EXTENDED_FLAG
#define PIC30_PMP_FLAG        PIC30_EXTENDED_FLAG "pmp"     PIC30_EXTENDED_FLAG
#define PIC30_APSV_FLAG       PIC30_EXTENDED_FLAG "apsv"    PIC30_EXTENDED_FLAG
#define PIC30_PRST_FLAG       PIC30_EXTENDED_FLAG "persist" PIC30_EXTENDED_FLAG
#define PIC30_PSV_FLAG        PIC30_EXTENDED_FLAG "psv"     PIC30_EXTENDED_FLAG
#define PIC30_EE_FLAG         PIC30_EXTENDED_FLAG "ee"      PIC30_EXTENDED_FLAG
#define PIC30_BSS_FLAG        PIC30_EXTENDED_FLAG "bss"     PIC30_EXTENDED_FLAG

#define PIC30_MERGE_FLAG      PIC30_EXTENDED_FLAG "mrg"     PIC30_EXTENDED_FLAG
#define PIC30_NOLOAD_FLAG     PIC30_EXTENDED_FLAG "nl"      PIC30_EXTENDED_FLAG
#define PIC30_ALGN_FLAG       PIC30_EXTENDED_FLAG "a"       PIC30_EXTENDED_FLAG
#define PIC30_RALGN_FLAG      PIC30_EXTENDED_FLAG "ra"      PIC30_EXTENDED_FLAG
#define PIC30_ADDR_FLAG       PIC30_EXTENDED_FLAG "addr"    PIC30_EXTENDED_FLAG

#define PIC30_FCNN_FLAG       PIC30_EXTENDED_FLAG "Nf"      PIC30_EXTENDED_FLAG
#define PIC30_FCNS_FLAG       PIC30_EXTENDED_FLAG "Sf"      PIC30_EXTENDED_FLAG
#define PIC30_EDS_FLAG        PIC30_EXTENDED_FLAG "eds"     PIC30_EXTENDED_FLAG
#define PIC30_PAGE_FLAG       PIC30_EXTENDED_FLAG "pge"     PIC30_EXTENDED_FLAG
#define PIC30_SFR_FLAG        PIC30_EXTENDED_FLAG "sfr"     PIC30_EXTENDED_FLAG
#define PIC30_NEAR_FLAG       PIC30_EXTENDED_FLAG "near"    PIC30_EXTENDED_FLAG
#define PIC30_DMA_FLAG        PIC30_EXTENDED_FLAG "dma"     PIC30_EXTENDED_FLAG
#define PIC30_BOOT_FLAG       PIC30_EXTENDED_FLAG "boot"
#define PIC30_SECURE_FLAG     PIC30_EXTENDED_FLAG "sec"
#define PIC30_AUXFLASH_FLAG   PIC30_EXTENDED_FLAG "aux"     PIC30_EXTENDED_FLAG
#define PIC30_AUXPSV_FLAG     PIC30_EXTENDED_FLAG "xpsv"    PIC30_EXTENDED_FLAG
#define PIC30_DF_FLAG         PIC30_EXTENDED_FLAG "df"      PIC30_EXTENDED_FLAG
#define PIC30_KEEP_FLAG       PIC30_EXTENDED_FLAG "keep"    PIC30_EXTENDED_FLAG

#define PIC30_SFR_NAME_P(NAME) (strstr(NAME, PIC30_SFR_FLAG))
#define PIC30_PGM_NAME_P(NAME) (strstr(NAME, PIC30_PROG_FLAG))
#define PIC30_FCNN_NAME_P(NAME) (strstr(NAME, PIC30_FCNN_FLAG))
#define PIC30_FCNS_NAME_P(NAME) (strstr(NAME, PIC30_FCNS_FLAG))
#define PIC30_FCN_NAME_P(NAME) (PIC30_FCNS_NAME_P(NAME) || \
                                 PIC30_FCNN_NAME_P(NAME))

#define PIC30_IS_NAME_P(NAME,IS) (strncmp(NAME, IS, sizeof(IS)-1) == 0)
#define PIC30_HAS_NAME_P(NAME,HAS) (strstr(NAME, HAS))

#define ENCODED_NAME_P(SYMBOL_NAME) \
  ((SYMBOL_NAME[0] == PIC30_EXTENDED_FLAG[0]) ? \
    (strrchr(SYMBOL_NAME,PIC30_EXTENDED_FLAG[0]) - SYMBOL_NAME) + 1 : 0)

/*
** Decode SYM_NAME and store the real name part in VAR, sans
** the characters that encode section info.  Define this macro if
** ENCODE_SECTION_INFO alters the symbol's name string.
*/
/* Note that we have to handle symbols like "%*start".  */
/*
#define STRIP_NAME_ENCODING(VAR, SYMBOL_NAME) \
 * now defined as a function
   const char *pic30_strip_name_encoding(const char *);
*/
/************************************************************************/

/*
** An alias for a machine mode name.
** This is the machine mode that elements of a jump-table should have.
*/
#define CASE_VECTOR_MODE SImode

/*
** Define as C expression which evaluates to nonzero if the tablejump
** instruction expects the table to contain offsets from the address of the
** table.
** Do not define this if the table should contain absolute addresses.
#define CASE_VECTOR_PC_RELATIVE 1
*/

/*
** Define this to be the smallest number of different values for which it
** is best to use a jump-table instead of a tree of conditional branches.
** The default is 4 for machines with a casesi instruction and 5 otherwise.
** For dsPIC, it may be best always to use a tree.
*/
#define CASE_VALUES_THRESHOLD (10)

/*
** Define this as 1 if `char' should by default be signed; else as 0.
*/
#define DEFAULT_SIGNED_CHAR 1

/*
** Max number of bytes we can move from memory to memory
** in one reasonably fast instruction.
*/
#define MOVE_MAX 4

/*
** If a memory-to-memory move would take MOVE_RATIO or more simple
** move-instruction pairs, we will do a movstr or libcall instead.
*/
#define MOVE_RATIO(optimize_size) 4

/*
** Define this if zero-extension is slow (more than one real instruction).
#define SLOW_ZERO_EXTEND
*/

/*
** Nonzero if access to memory by bytes is slow and undesirable.
*/
#define SLOW_BYTE_ACCESS 1

/* Define if shifts truncate the shift count
   which implies one can omit a sign-extension or zero-extension
   of a shift count. */
/* #define SHIFT_COUNT_TRUNCATED 1 */

/*
** Value is 1 if truncating an integer of INPREC bits to OUTPREC bits
** is done just by pretending it is already truncated.
*/
#define TRULY_NOOP_TRUNCATION(OUTPREC, INPREC) 1

/*
** We assume that the store-condition-codes instructions store 0 for false
** and some other value for true.  This is the value stored for true.
*/
#define STORE_FLAG_VALUE 1

/*
** When a prototype says `char' or `short', really pass an `int'.
#define PROMOTE_PROTOTYPES
*/

/*
** Specify the machine mode that pointers have.
** After generation of rtl, the compiler makes no further distinction
** between pointers and any other objects of this machine mode.
*/
/*
** Width in bits of a pointer.
** See also the macro `Pmode' defined below.
*/
#define POINTER_SIZE      (TARGET_EDS ? 32 : 16)

#define Pmode (TARGET_EDS ? P32EDSmode : HImode )
#if 1
#define STACK_Pmode HImode
#endif
#define TARGET_CONSTANT_PMODE P16APSVmode


/*
** A function address in a call instruction is a byte address
** (for indexing purposes)
*/
#define FUNCTION_MODE QImode

#define REGISTER_TARGET_PRAGMAS(PFILE) { \
  c_register_pragma(0, "interrupt", pic30_handle_interrupt_pragma); \
  c_register_pragma(0, "code", pic30_handle_code_pragma); \
  c_register_pragma(0, "idata", pic30_handle_idata_pragma); \
  c_register_pragma(0, "udata", pic30_handle_udata_pragma); \
  c_register_pragma(0, "config", mchp_handle_config_pragma); \
  c_register_pragma(0, "large_ararys", pic30_handle_large_arrays_pragma); \
  c_register_pragma(0, "align", mchp_handle_align_pragma); \
  c_register_pragma(0, "section", mchp_handle_section_pragma); \
  c_register_pragma(0, "printf_args", mchp_handle_printf_args_pragma); \
  c_register_pragma(0, "scanf_args", mchp_handle_scanf_args_pragma); \
  c_register_pragma(0, "keep", mchp_handle_keep_pragma); \
  c_register_pragma(0, "optimize", mchp_handle_optimize_pragma); \
  c_register_pragma(0, "inline", mchp_handle_inline_pragma); \
  }

extern void pic30_cpu_cpp_builtins(void *);

#define TARGET_CPU_CPP_BUILTINS() pic30_cpu_cpp_builtins(pfile)

/*
** Compute extra cost of moving data between one register class
** and another.
*/
#define REGISTER_MOVE_COST(MODE,FROM,TO)  ((MODE > P16PMPmode) ? 2 : 1)

/*
** A C expression for the cost of moving data of mode MODE between a register
** of class CLASS and memory; IN is zero if the value is to be written to
** memory, non-zero if it is to be read in. This cost is relative to those in
** REGISTER_MOVE_COST. If moving between registers and memory is more expensive
** than between two registers, you should define this macro to express the
** relative cost. 
** If you do not define this macro, GNU CC uses a default cost of 4 plus the
** cost of copying via a secondary reload register, if one is needed. If your
** machine requires a secondary reload register to copy between memory and a
** register of class but the reload mechanism is more complex than copying via
** an intermediate, define this macro to reflect the actual cost of the move. 
**
** GNU CC defines the function memory_move_secondary_cost if secondary reloads
** are needed. It computes the costs due to copying via a secondary register.
** If your machine copies from memory using a secondary register in the
** conventional way but the default base value of 4 is not correct for your
** machine, define this macro to add some other value to the result of that
** function. The arguments to that function are the same as to this macro
**
** Memory move cost is double that of register to register.
*/
#define MEMORY_MOVE_COST(MODE,CLASS,IN)      \
((MODE) == HImode ? 2 : 4)

/*
** Branches costs.
** A C expression for the cost of a branch instruction.
** A value of 1 is the default; other values are interpreted relative to that. 
** dsPIC is typical.
*/
#define BRANCH_COST(speed,predictable)         1

/* A C statement (sans semicolon) to update the integer variable COST
** based on the relationship between INSN that is dependent on
** DEP_INSN through the dependence LINK.  The default is to make no
** adjustment to COST.
#define ADJUST_COST(INSN,LINK,DEP_INSN,COST)                          \
  (COST) = pic30_adjust_cost (INSN, LINK, DEP_INSN, COST)
*/

/*
** Define this macro to inhibit strength reduction of memory
** addresses.  (On some machines, such strength reduction seems to do
** harm rather than good.)
#define DONT_REDUCE_ADDR
*/

/*
** Define this macro if it is good or better to call a constant function
** address than to call an address kept in a register.
*/
#define NO_FUNCTION_CSE

/*
** A C compound statement to set the components of cc_status appropriately
** for an insn insn whose body is exp. It is this macro's responsibility to
** recognize insns that set the condition code as a byproduct of other activity
** as well as those that explicitly set (cc0). This macro is not used on
** machines that do not use cc0. If there are insns that do not set the
** condition code but do alter other machine registers, this macro must check
** to see whether they invalidate the expressions that the condition code is
** recorded as reflecting. For example, on the 68000, insns that store in
** address registers do not set the condition code, which means that usually
** NOTICE_UPDATE_CC can leave cc_status unaltered for such insns. But suppose
** that the previous insn set the condition code based on location `a4@(102)'
** and the current insn stores a new value in `a4'. Although the condition code
** is not changed by this, it will no longer be true that it reflects the
** contents of `a4@(102)'. Therefore, NOTICE_UPDATE_CC must alter cc_status in
** this case to say that nothing is known about the condition code value. The
** definition of NOTICE_UPDATE_CC must be prepared to deal with the results of
** peephole optimization: insns whose patterns are parallel RTXs containing
** various reg, mem or constants which are just the operands. The RTL structure
** of these insns is not sufficient to indicate what the insns actually do.
** What NOTICE_UPDATE_CC should do when it sees one is just to run
** CC_STATUS_INIT. A possible definition of NOTICE_UPDATE_CC is to call a
** function that looks at an attribute (see section Instruction Attributes)
** named, for example, `cc'. This avoids having detailed information about
** patterns in two places, the `md' file and in NOTICE_UPDATE_CC.
*/
#define NOTICE_UPDATE_CC(EXP, INSN) pic30_notice_update_cc(EXP, INSN)

/*
** Return non-zero if SELECT_CC_MODE will never return MODE for a
** floating point inequality comparison.
*/
#define REVERSIBLE_CC_MODE(MODE) (1)

/*
** A C expression to initialize the mdep field to "empty".
** The default definition does nothing, since most machines don't use the field
** anyway. If you want to use the field, you should probably define this macro
** to initialize it. 
** This macro is not used on machines that do not use cc0
*/
#define CC_STATUS_MDEP_INIT cc_status.mdep = 0

/*
** One some machines not all possible comparisons are defined, but you can
** convert an invalid comparison into a valid one. For example, the Alpha does
** not have a GT comparison, but you can use an LT comparison instead and swap
** the order of the operands. 
** On such machines, define this macro to be a C statement to do any required
** conversions. code is the initial comparison code and op0 and op1 are the
** left and right operands of the comparison, respectively. You should modify
** code, op0, and op1 as required. 
**
** GNU CC will not assume that the comparison resulting from this macro is
** valid but will see if the resulting insn matches a pattern in the `md' file. 
**
** You need not define this macro if it would never change the comparison code
** or operands.
*/

#define CANONICALIZE_COMPARISON(CODE,OP0,OP1)         \
if (pic30_near_operand(OP1, GET_MODE(OP1)))         \
{                        \
     rtx tmp = OP0; OP0 = OP1 ; OP1 = tmp;         \
     CODE = swap_condition (CODE);            \
}

/************************************************************************/
/* Macros for SDB and DWARF Output                        */
/************************************************************************/

/*
** A C expression that returns the type of debugging output GCC should produce
** when the user specifies just `-g'. Define this if you have arranged for GCC
** to support more than one format of debugging output. Currently, the
** allowable values are DBX_DEBUG, SDB_DEBUG, DWARF_DEBUG, DWARF2_DEBUG,
** and XCOFF_DEBUG. When the user specifies `-ggdb', GCC normally also uses
** the value of this macro to select the debugging output format, but with two
** exceptions.
** If DWARF2_DEBUGGING_INFO is defined and LINKER_DOES_NOT_WORK_WITH_DWARF2 is
** not defined, GCC uses the value DWARF2_DEBUG.
** Otherwise, if DBX_DEBUGGING_INFO is defined, GCC uses DBX_DEBUG.
** The value of this macro only affects the default debugging output;
** the user can always get a specific type of output by using `-gstabs',
** `-gcoff', `-gdwarf-1', `-gdwarf-2', or `-gxcoff'
*/

#if (!PIC30_DWARF2)
#define PREFERRED_DEBUGGING_TYPE      SDB_DEBUG
#endif

/*
** Define if your assembler supports .uleb128.
*/
#if (PIC30_DWARF2)
#if !defined(HAVE_AS_LEB128)
#define HAVE_AS_LEB128
#endif
#endif

/*
** Define this macro if GCC should produce COFF-style debugging output
** for SDB in response to the `-g' option.
*/
#if (!PIC30_DWARF2)
#define SDB_DEBUGGING_INFO   1
#endif

/*
** Define this macro if GCC should produce dwarf version 2 format debugging
** output in response to the `-g' option.
*/
#if (PIC30_DWARF2)
#define   DWARF2_DEBUGGING_INFO   1
#endif

/*
** Some assemblers do not support a semicolon as a delimiter, even between SDB
** assembler directives. In that case, define this macro to be the delimiter to
** use (usually `\n'). It is not necessary to define a new set of PUT_SDB_op
** macros if this is the only change required
*/

#define SDB_DELIM "\n"

/*
** Define this macro to allow references to structure, union, or enumeration
** tags that have not yet been seen to be handled. Some assemblers choke if
** forward tags are used, while some require it.
*/
#define SDB_ALLOW_FORWARD_REFERENCES   1

/*
** Define this macro to be a nonzero value if the assembler can generate
** Dwarf 2 line debug info sections. This will result in much more compact
** line number tables, and hence is desirable if it works.
*/
#define   DWARF2_ASM_LINE_DEBUG_INFO   0

/*
** The size in bytes of a DWARF field indicating an offset or length
** relative to a debug info section, specified to be 4 bytes in the
** DWARF-2 specification.  The SGI/MIPS ABI defines it to be the same
** as PTR_SIZE.
*/
#define DWARF2_ADDR_SIZE   4

/*
** Define this macro to 0 if your target supports DWARF 2 frame unwind
** information, but it does not yet work with exception handling.
** Otherwise, if your target supports this information (if it defines
** INCOMING_RETURN_ADDR_RTX and either UNALIGNED_INT_ASM_OP or
** OBJECT_FORMAT_ELF), GCC will provide a default definition of 1. 
**
** If this macro is defined to 1, the DWARF 2 unwinder will be the default
** exception handling mechanism; otherwise, setjmp/longjmp will be used by
** default. 
**
** If this macro is defined to anything, the DWARF 2 unwinder will be used
** instead of inline unwinders and __unwind_function in the non-setjmp case. 
**
#if defined(DWARF2_UNWIND_INFO)
#undef   DWARF2_UNWIND_INFO
#endif
*/

/*
 *  Debug target hook for starting source file for pic30
 */
#if (!PIC30_DWARF2)
#define SDBOUT_START_SOURCE_FILE pic30_start_source_file
#endif

/*
 *  How to convert a line number to store in sdb_begin_function_line so
 *    that relative offsets may be produced.  dsPIC30 doesn't want relative
 *    offsets; so don't store a valid value into sdb_begin_function_line
 */
#define SDB_BEGIN_FUNCTION_LINE(X) (0)

#define SDB_END_PROLOGUE pic30_sdb_end_prologue


/************************************************************************/
/* Control the assembler format that we output.                         */
/************************************************************************/

#define GLOBAL_ASM_OP ".global\t"

/*
** We define this to prevent the name mangler from putting dollar signs
** into function names.
*/
#define NO_DOLLAR_IN_LABEL

/*
** Output to assembler file text saying following lines
** may contain character constants, extra white space, comments, etc.
*/
#define ASM_APP_ON   ""

/*
** Output to assembler file text saying following lines
** no longer contain unusual constructs.
*/
#define ASM_APP_OFF   ""

#define NAMED_SECTION_FUNCTION void pic30_no_section() { \
   in_section = in_named;                                \
   in_named_name = ""; }

#define NDATA_SECTION_FUNCTION                             \
void                                                       \
ndata_section ()                                           \
{                                                          \
   if (in_section != in_ndata)                             \
   {                                                       \
      fprintf(asm_out_file, "%s\n", NDATA_SECTION_ASM_OP); \
      in_section = in_ndata;                               \
   }                                                       \
}
      
/*
   CONST_SECTION_ASM_OP has been poisoned 
*/
#define CONST_SECTION_FUNCTION                                \
void                                                          \
const_section ()                                              \
{                                                             \
   if (in_section != in_const)                                \
   {                                                          \
      fprintf(asm_out_file, "%s\n", C30CONST_SECTION_ASM_OP); \
      in_section = in_const;                                  \
   }                                                          \
}
      
#define DCONST_SECTION_FUNCTION                             \
void                                                        \
dconst_section ()                                           \
{                                                           \
   if (in_section != in_dconst)                             \
   {                                                        \
      fprintf(asm_out_file, "%s\n", DCONST_SECTION_ASM_OP); \
      in_section = in_dconst;                               \
   }                                                        \
}
      
#define NDCONST_SECTION_FUNCTION                             \
void                                                         \
ndconst_section ()                                           \
{                                                            \
   if (in_section != in_ndconst)                             \
   {                                                         \
      fprintf(asm_out_file, "%s\n", NDCONST_SECTION_ASM_OP); \
      in_section = in_ndconst;                               \
   }                                                         \
}
      
/*
** On most machines, read-only variables, constants, and jump tables are
** placed in the text section. If this is not the case on your machine,
** this macro should be defined to be the name of a function (either
** data_section or a function defined in EXTRA_SECTIONS) that switches
** to the section to be used for read-only items. If these items should
** be placed in the text section, this macro should not be defined.
*/
#define READONLY_DATA_SECTION_ASM_OP   pic30_const_section_asm_op()

/*
** Output before program init section
*/
#define INIT_SECTION_ASM_OP  "\t.section\t.init"

/*
** Output before program text section
*/
#define TEXT_SECTION_ASM_OP pic30_text_section_asm_op()

/*
** Output before writable data.
*/
#define DATA_SECTION_ASM_OP pic30_data_section_asm_op()

/*
** Output before writable data (near).
*/
#define NDATA_SECTION_ASM_OP pic30_ndata_section_asm_op()

/*
** Output before read-only data in PSV space.
*/
#define C30CONST_SECTION_ASM_OP pic30_const_section_asm_op()

/*
** Output before read-only data in data space.
*/
#define DCONST_SECTION_ASM_OP pic30_dconst_section_asm_op()

/*
** Output before read-only data in near data space.
*/
#define NDCONST_SECTION_ASM_OP pic30_ndconst_section_asm_op()

/*
** A C statement to output DBX or SDB debugging information before code for
** line number line of the current source file to the stdio stream stream. 
** This macro need not be defined if the standard form of debugging information
** for the debugger in use is appropriate. 
*/
/* renamed */
#define SDB_OUTPUT_SOURCE_LINE(STREAM, LINE)         \
          fprintf(STREAM, "\t.ln\t%d\n", LINE)


/*
** How to renumber registers for dbx and gdb.
*/
#define DBX_REGISTER_NUMBER(REGNO) (REGNO == B_REGNO ?  0x14 : REGNO)

/******************  Assembler output formatting  **********************/

/*
** A C statement to output assembler commands which will identify the object
** file as having been compiled with GCC (or another GNU compiler). If you
** don't define this macro, the string `gcc_compiled.:' is output. This string
** is calculated to define a symbol which, on BSD systems, will never be
** defined for any other reason. GDB checks for the presence of this symbol
** when reading the symbol table of an executable. On non-BSD systems, you must
** arrange communication with GDB in some other fashion. If GDB is not used on
** your system, you can define this macro with an empty body.
#define ASM_IDENTIFY_GCC(FILE)  fputs ("gcc2_compiled:\n", FILE)
*/

/*
** A C string constant describing how to begin a comment in the target
** assembler language. The compiler assumes that the comment will end at
** the end of the line.
*/
#define ASM_COMMENT_START  ";"

/*
** Define this macro if you are using an unusual assembler that requires
** different names for the machine instructions. The definition is a C
** statement or statements which output an assembler instruction opcode to the
** stdio stream stream. The macro-operand ptr is a variable of type char *
** which points to the opcode name in its "internal" form--the form that is
** written in the machine description. The definition should output the opcode
** name to stream, performing any translation you desire, and increment the
** variable ptr to point at the end of the opcode so that it will not be output
** twice. In fact, your macro definition may process less than the entire
** opcode name, or more than the opcode name; but if you want to process text
** that includes `%'-sequences to substitute operands, you must take care of
** the substitution yourself. Just be sure to increment ptr over whatever text
** should not be output normally. If you need to look at the operand values,
** they can be found as the elements of recog_operand. If the macro definition
** does nothing, the instruction is output in the usual way.
**
** For dsPIC, the opcode is separated from the operands with a tab.
** If the 1st character is '.', this is a label, not an opcode.
*/
#define ASM_OUTPUT_OPCODE(FILE,PTR)         \
if (*(PTR) != '.')               \
{                     \
   while (*(PTR) != '\0' && *(PTR) != ' ')      \
   {                  \
       putc(*(PTR), FILE);            \
       (PTR)++;               \
   }                  \
   while (*(PTR) == ' ')            \
   {                  \
       (PTR)++;               \
   }                  \
   putc('\t', FILE);            \
}

/*
** A C statement (sans semicolon) to output to the stdio stream stream any text
** necessary for declaring the name NAME of a function which is being defined.
** This macro is responsible for outputting the label definition (perhaps using
** ASM_OUTPUT_LABEL). The argument decl is the FUNCTION_DECL tree node
** representing the function. 
*/
#define ASM_DECLARE_FUNCTION_NAME(FILE, NAME, DECL)   \
  pic30_asm_declare_function_name(FILE, NAME, DECL)

/*
** A C statement (sans semicolon) to output to the stdio stream stream any text
** necessary for declaring the name NAME of an initialized variable which is
** being defined. This macro must output the label definition (perhaps using
** ASM_OUTPUT_LABEL). The argument DECL is the VAR_DECL tree node representing
** the variable.  
*/
#define ASM_DECLARE_OBJECT_NAME(FILE, NAME, DECL)   \
  pic30_asm_declare_object_name(FILE, NAME, DECL)

/*
** A C expression that specifies the mnemonic for the SET assembler directive.
*/
#define   SET_ASM_OP   "\t.set\t"

/*
** A C statement to be executed just prior to the output of
** assembler code for INSN, to modify the extracted operands so
** they will be output differently.
**
** Here the argument OPVEC is the vector containing the operands
** extracted from INSN, and NOPERANDS is the number of elements of
** the vector which contain meaningful data for this insn.
** The contents of this vector are what will be used to convert the insn
** template into assembler code, so you can change the assembler output
** by changing the contents of the vector.
*/

#define FINAL_PRESCAN_INSN(INSN, OPVEC, NOPERANDS) \
  pic30_final_prescan_insn ((INSN), (OPVEC), (NOPERANDS))

/*
** A C statement (sans semicolon) to output to the stdio stream FILE the
** assembler definition of a label named NAME. Use the expression
** assemble_name (FILE, NAME) to output the name itself; before and after
** that, output the additional assembler syntax for defining the name,
** and a newline.
*/
#define ASM_OUTPUT_LABEL(FILE,NAME)   \
{ assemble_name (FILE, NAME); fputs (":\n", FILE); }

/*
** This is how to output a reference to a user-level label named NAME.
** `assemble_name' uses this.
*/
#define ASM_OUTPUT_LABELREF(FILE, NAME) \
do {                     \
  char * real_name;               \
  real_name = pic30_strip_name_encoding ((NAME));   \
  asm_fprintf (FILE, "%U%s", real_name);      \
} while (0)           

/*
** This is how we tell the assembler that a symbol is weak.
*/
#define ASM_WEAKEN_LABEL(FILE,NAME)   \
do {               \
 fputs ("\t.weak\t", FILE);      \
 assemble_name(FILE, NAME);      \
 fputc ('\n', FILE);         \
} while (0)

/*
** The prefix to add to user-visible assembler symbols.
*/
#define USER_LABEL_PREFIX   "_"
/*
** The prefix to add to immediate operands.
*/
#define IMMEDIATE_PREFIX   "#"
/*
** The prefix to add to local assembler symbols.
*/
#define LOCAL_LABEL_PREFIX   ".L"

/*
** This is how to output an internal numbered label where
** PREFIX is the class of label and NUM is the number within the class.
** A C statement to output to the stdio stream stream a label whose name is
** made from the string prefix and the number num. It is absolutely essential
** that these labels be distinct from the labels used for user-level functions
** and variables. Otherwise, certain programs will have name conflicts with
** internal labels. It is desirable to exclude internal labels from the symbol
** table of the object file. Most assemblers have a naming convention for
** labels that should be excluded; on many systems, the letter `L' at the
** beginning of a label has this effect. You should find out what convention
** your system uses, and follow it.
*/
#define ASM_GENERATE_INTERNAL_LABEL(BUF,PREFIX,NUM)      \
   sprintf (BUF, "*.%s%d", PREFIX, NUM)

/*
** Define this if the label before a jump-table needs to be output specially.
** The first three arguments are the same as for ASM_OUTPUT_INTERNAL_LABEL;
** the fourth argument is the jump-table which follows (a jump_insn containing
** an addr_vec or addr_diff_vec).
**
** If this macro is not defined, these labels are output with
** ASM_OUTPUT_INTERNAL_LABEL.
*/
#define ASM_OUTPUT_CASE_LABEL(STREAM, PREFIX, NUM, TABLE) \
      pic30_asm_output_case_label(STREAM, PREFIX, NUM, TABLE)

/*
** Epilogue for case labels.  This jump instruction is called by casesi
** to transfer to the appropriate branch instruction within the table.
**
** Define this if something special must be output at the end of a jump-table.
** The definition should be a C statement to be executed after the assembler
** code for the table is written. It should write the appropriate code to stdio
** stream STREAM. The argument TABLE is the jump-table insn, and NUM is the
** label-number of the preceding label.
*/

#define ASM_OUTPUT_CASE_END(STREAM, NUM, TABLE)            \
   pic30_asm_output_case_end(STREAM, NUM, TABLE)

/*
** Define this macro to be an expression with a non-zero value if jump tables
** (for tablejump insns) should be output in the text section, along with the
** assembler instructions. Otherwise, the readonly data section is used. This
** macro is irrelevant if there is no separate readonly data section.
*/
#define JUMP_TABLES_IN_TEXT_SECTION 1

/*
** This is how to output an assembler line defining a string constant.
** A C statement to output to the stdio stream stream an assembler instruction
** to assemble a string constant containing the len bytes at ptr. ptr will be
** a C expression of type char * and len a C expression of type int. If the
** assembler has a .ascii pseudo-op as found in the Berkeley Unix assembler,
** do not define the macro ASM_OUTPUT_ASCII.
*/

#define ASM_OUTPUT_ASCII(FILE, PTR, LEN) \
   pic30_asm_output_ascii(FILE, PTR, LEN)

/*
** A C statement to output to the stdio FILE an assembler instruction
** to assemble an integer of 1 byte. The argument VALUE will be an RTL
** expression which represents a constant value.
*/
#define ASM_OUTPUT_CHAR(FILE,VALUE)  do {   \
   fprintf(FILE, "\t.byte "),      \
   output_addr_const (FILE, (VALUE)),   \
   fprintf(FILE, "\n");         \
  } while (0)

/*
** A C statement to output to the stdio FILE an assembler instruction
** to assemble an integer of 2 bytes. The argument VALUE will be an RTL
** expression which represents a constant value.
*/
#define ASM_OUTPUT_SHORT(FILE,VALUE) pic30_asm_output_short(FILE,VALUE)

/*
** A C statement to output to the stdio FILE an assembler instruction
** to assemble an integer of 4 bytes. The argument VALUE will be an RTL
** expression which represents a constant value.
*/
#define ASM_OUTPUT_INT(FILE,VALUE) do {     \
   fprintf(FILE, "\t.long\t");               \
   output_addr_const(FILE, VALUE);             \
   fprintf(FILE, "\n");                  \
  } while (0)

/*
** This is how to output an assembler line for a numeric constant byte.
*/
#define ASM_OUTPUT_BYTE(FILE,VALUE) do {     \
   fprintf(FILE, "\t.byte\t%d\n", VALUE);            \
  } while (0)

/*
** This is how to output an insn to push a register on the stack.
** It need not be very fast code (used only in profiling).
*/
#define ASM_OUTPUT_REG_PUSH(FILE,REGNO)  \
  fprintf (FILE, "\tmov %s,[w15++]\n", reg_names[REGNO])

/*
** This is how to output an insn to pop a register from the stack.
** It need not be very fast code (used only in profiling).
*/
#define ASM_OUTPUT_REG_POP(FILE,REGNO)  \
   fprintf (FILE, "\tmov [--w15],%s\n", reg_names[REGNO])

/*
** This is how to output an element of a case-vector that is absolute.
*/
#define ASM_OUTPUT_ADDR_VEC_ELT(FILE, VALUE)   \
    asm_fprintf (FILE, "\tbra\t%L%d\n", VALUE);

/*
** This is how to output an element of a case-vector that is relative.
*/
#define ASM_OUTPUT_ADDR_DIFF_ELT(FILE, BODY, VALUE, REL)  \
   fprintf (FILE, "\t.word\t%L%d-%L%d ;addr_diff_elt\n", VALUE,REL)

/*
** This is how to output an assembler line that says to advance the
** location counter to a multiple of 2**LOG bytes.
*/
#define ASM_OUTPUT_ALIGN(FILE,LOG)   \
  pic30_asm_output_align(FILE,LOG)

/*
** A C statement to output to the stdio stream FILE an assembler instruction
** to advance the location counter by SIZE bytes. Those bytes should be zero
** when loaded. SIZE will be a C expression of type int.
*/
#define ASM_OUTPUT_SKIP(FILE, SIZE) \
   fprintf (FILE, "\t.skip\t%d\n", (SIZE))

/*
** A C statement (sans semicolon) to output to the stdio stream FILE the
** assembler definition of a common-label named NAME whose size is SIZE bytes.
** The variable ROUNDED is the size rounded up to whatever alignment the caller
** wants. Use the expression assemble_name (FILE, NAME) to output the name
** itself; before and after that, output the additional assembler syntax for
** defining the name, and a newline. This macro controls how the assembler
** definitions of uninitialized common global variables are output.
*/
#define ASM_OUTPUT_COMMON(FILE, NAME, SIZE, ROUNDED)  \
   pic30_asm_output_common(FILE, NAME, SIZE, ROUNDED)

/*
** Like ASM_OUTPUT_COMMON except takes the required alignment as a separate,
** explicit argument. If you define this macro, it is used in place of
** ASM_OUTPUT_COMMON, and gives you more flexibility in handling the required
** alignment of the variable. The alignment is specified as the number of bits.
*/
#define ASM_OUTPUT_ALIGNED_DECL_COMMON(FILE, DECL, NAME, SIZE, ALIGN) \
        pic30_asm_output_aligned_common(FILE, DECL, NAME, SIZE, ALIGN)

#define ASM_OUTPUT_ALIGNED_COMMON(FILE, NAME, SIZE, ALIGNMENT)  \
   pic30_asm_output_aligned_common(FILE, 0, NAME, SIZE, ALIGNMENT)

/*
** A C statement (sans semicolon) to output to the stdio stream FILE the
** assembler definition of a local-common-label named NAME whose size is SIZE
** bytes. The variable ROUNDED is the size rounded up to whatever alignment the
** caller wants. Use the expression assemble_name (FILE, NAME) to output the
** name itself; before and after that, output the additional assembler syntax
** for defining the name, and a newline. This macro controls how the assembler
** definitions of uninitialized static variables are output.
*/
#define ASM_OUTPUT_LOCAL(FILE, NAME, SIZE, ROUNDED)  \
   pic30_asm_output_local(FILE, NAME, SIZE, ROUNDED)

/*
** Like ASM_OUTPUT_LOCAL except takes a decl, and the required alignment as 
** a separate, explicit argument. If you define this macro, it is used in 
** place of ASM_OUTPUT_LOCAL, and gives you more flexibility in handling the 
** required alignment of the variable. The alignment is specified as the number
** of bits.
*/
#define ASM_OUTPUT_ALIGNED_DECL_LOCAL(FILE, DECL, NAME, SIZE, ALIGNMENT)  \
   pic30_asm_output_aligned_decl_local(FILE, DECL, NAME, SIZE, ALIGNMENT)

/*
** Store in OUTPUT a string (made with alloca) containing
** an assembler-name for a local static variable named NAME.
** LABELNO is an integer which is different for each call.
** The magic number 12 includes 1 for the period, 1 for the
** terminating null, and 10 for the maximum integer value of
** 4294967295.
*/
#define ASM_FORMAT_PRIVATE_NAME(OUTPUT, NAME, LABELNO)   \
( (OUTPUT) = (char *) alloca (strlen ((NAME)) + 10+1+1),   \
  sprintf ((OUTPUT), "%s.%u", (NAME), (LABELNO)))

/*
** Print operand X (an rtx) in assembler syntax to file FILE.
** CODE is a letter or dot (`z' in `%z0') or 0 if no letter was specified.
** For `%' followed by punctuation, CODE is the punctuation and X is null.
** dsPIC note:  there are these special CODE characters:
**   See pic30.c.
*/
#define PRINT_OPERAND(FILE, X, CODE)  pic30_print_operand(FILE,X,CODE)
#define PRINT_OPERAND_ADDRESS(FILE, ADDR) pic30_print_operand_address(FILE,ADDR)
#define PIC30_RPAIRSOVERLAP_P(idDst, idSrc) \
      (((idSrc+1) >= idDst) && ((idDst+1) >= idSrc))

/*
** Easy access check for function beginning 
**/
#define NOTE_INSN_FUNCTION_BEG_P(INSN) \
  ((GET_CODE(INSN) == NOTE) && (NOTE_KIND (INSN) == NOTE_INSN_FUNCTION_BEG))


/*
 *  On systems where we have a licence manager, call it
 */
#ifdef LICENSE_MANAGER
extern int pic30_license_valid;

#define NO_GCC_DRIVER_HOST_INITIALIZATION \
{  char *path;                                                             \
   char *exec;                                                             \
   char *args[] = { 0, "-k", 0 };                                          \
   char *c;                                                                \
   char *err_msg, *err_arg;                                                \
   int pid;                                                                \
   int status;                                                             \
                                                                           \
   pic30_license_valid = 0;                                                \
   path = make_relative_prefix(argv[0], standard_bindir_prefix,            \
                                        standard_exec_prefix);             \
                                                                           \
   if (!path) pfatal_pexecute("Could not locate `%s`\n", argv[0]);         \
   exec = xmalloc(strlen(path)+sizeof("pic30-lm.exe") + 1);                \
   sprintf(exec, "%spic30-lm.exe", path);                                  \
   args[0] = exec;                                                         \
   pid = pexecute(exec, args, programname, 0, &err_msg, &err_arg,          \
                  PEXECUTE_FIRST | PEXECUTE_LAST);                         \
   if (pid == -1) pfatal_pexecute (err_msg, exec);                         \
   pid = pwait(pid, &status, 0);                                           \
   if (pid < 0) abort();                                                   \
   if (WIFEXITED(status) && (WEXITSTATUS(status) == 0)) {                  \
     pic30_license_valid=1;                                                \
   }                                                                       \
   free(exec);                                                             \
}

#endif

/* define if any work needs to be done before the prologue is generated:
   prologue generation is a new sequence, and will therefore not have access
   to the current instruction flow */
#define PRE_PROLOGUE_FN \
  { \
    (void) pic30_asm_function_p(1); \
  }

#define SECTION_FLAGS_INT unsigned long long

#define NO1_GCC_DRIVER_HOST_INITIALIZATION { \
  int size;                              \
                                         \
  size = strlen(argv[0])+1;                                             \
  if (size < p-argv[0] + sizeof("c30_device.info")+1)                   \
    size = p-argv[0] + sizeof("c30_device.info")+1;                     \
  (char *) pic30_resource_file_f = xmalloc(size);                       \
                                                                        \
  sprintf(pic30_resource_file_f, "%s",argv[0]);                         \
  pic30_resource_file_f[p-argv[0]] = 0;                                 \
  strcat(pic30_resource_file_f,"c30_device.info");                      \
}

/*----------------------------------------------------------------------*/

/*END********************************************************************/

#define TARGET_APPLY_PRAGMA pic30_apply_pragmas
#define TARGET_LAYOUT_TYPE(TYPE)  pic30_layout_type(TYPE)
#define TARGET_POINTER_MODE(TYPE,DECL) (pic30_pointer_mode(TYPE,DECL))
#define TARGET_POINTER_SIZE(TYPE) (GET_MODE_SIZE(pic30_pointer_mode(TYPE)))
#define TARGET_CONVERT_POINTER    pic30_convert_pointer
#define TARGET_IS_POINTER_MODE(MODE) ((MODE == Pmode) || \
                                      (MODE == P24PROGmode) || \
                                      (MODE == P16PMPmode) || \
                                      (MODE == P32EXTmode) || \
                                      (MODE == P32EDSmode) || \
                                      (MODE == P32PEDSmode) || \
                                      (MODE == P16APSVmode) || \
                                      (MODE == P24PSVmode) || \
                                      (MODE == P32DFmode))

enum pic30_address_space {
  /* ADDR_SPACE_GENERIC = 0 */
  pic30_space_psv = 1,
  pic30_space_prog,
  pic30_space_pmp,
  pic30_space_external,
  pic30_space_eds,
  pic30_space_packed
};

#define TARGET_ADDR_SPACE_KEYWORDS \
  ADDR_SPACE_KEYWORD("__psv__", pic30_space_psv),            \
  ADDR_SPACE_KEYWORD("__prog__", pic30_space_prog),          \
  ADDR_SPACE_KEYWORD("__pmp__", pic30_space_pmp),            \
  ADDR_SPACE_KEYWORD("__external__", pic30_space_external),  \
  ADDR_SPACE_KEYWORD("__eds__", pic30_space_eds),            \
  ADDR_SPACE_KEYWORD("__pack_upper_byte", pic30_space_packed)

enum pic30_set_psv_results {
  pic30_set_nothing,
  pic30_set_on_call,
  pic30_set_on_return
};


extern tree pic30_expand_constant(tree);
#define TARGET_EXPAND_CONSTANT pic30_expand_constant
#define POINTERS_EXTEND_UNSIGNED 1

#define ASM_OUTPUT_FUNCTION_PRE(FILE, DECL, FNNAME) \
  pic30_function_pre(FILE,DECL,FNNAME)

enum pic30_special_trees {
  pst_none,
  pst_8,
  pst_16,
  pst_32,
  pst_64,
  pst_any,
  pst_length
};

extern tree pic30_read_externals(enum pic30_special_trees);
extern tree pic30_write_externals(enum pic30_special_trees);

extern void pic30_target_bind(tree name, tree decl);
#define TARGET_BIND pic30_target_bind

extern int pic30_emit_block_move(rtx x, rtx *y, rtx size, unsigned int align);
#define TARGET_EMIT_BLOCK_MOVE pic30_emit_block_move

#define EXTRA_RTL_FILE "config/pic30/pic30-rtl.def"

/*
 *   object file signatures
 */

typedef union {
  unsigned int mask;
  struct {
    unsigned int unsigned_long_size_t:1;  /* true if size_t is unsigned long */
    unsigned int dummy1:1;                /* place holder */
    unsigned int dummy2:1;                /* place holder */
    unsigned int dummy3:1;                /* place holder */
    unsigned int dummy4:1;                /* place holder */
    unsigned int dummy5:1;                /* place holder */
    unsigned int dummy6:1;                /* place holder */
    unsigned int dummy7:1;                /* place holder */
    unsigned int dummy8:1;                /* place holder */
    unsigned int dummy9:1;                /* place holder */
    unsigned int dummy10:1;               /* place holder */
    unsigned int dummy11:1;               /* place holder */
    unsigned int dummy12:1;               /* place holder */
    unsigned int dummy13:1;               /* place holder */
    unsigned int dummy14:1;               /* place holder */
    unsigned int dummy15:1;               /* place holder */
  } bits;
} object_signature_t;

extern object_signature_t options_set, external_options_mask;

#define C_COMMON_OVERRIDE_OPTIONS pic30_common_override_options()

#define USE_SELECT_SECTION_FOR_FUNCTIONS 1

#define TARGET_HAVE_SWITCHABLE_BSS_SECTIONS true

#ifdef MCHP_VERSION
#define PIC30_STRING1(x) #x
#define PIC30_STRING(x) PIC30_STRING1(x)
#define SET_MCHP_VERSION(x)                                        \
  { char *version = (char *)PIC30_STRING(MCHP_VERSION);            \
    char *c;                                                       \
    int mm = 0;                                                    \
    int major=0;                                                   \
    int minor=0;                                                   \
                                                                   \
    x = 0;                                                         \
    for (c = version; *c; c++) {                                   \
      if ((*c >= '0') && (*c <= '9')) {                            \
        if (mm == 1) minor = (minor * 10) + (*c - '0');            \
        else if (mm == 0) major = (major *10) + (*c - '0');        \
        /* else ignore it */                                       \
      } else if ((*c == '_') || (*c == '.')) mm++;                 \
    }                                                              \
    if (minor > 99) minor = 99;                                    \
    x = major*1000 + minor;                                        \
  }
#endif

enum pic30_acceptible_regs_flags {
  parf_reg_only = 0,
  parf_subreg_ok = 1,
  parf_D_regs_only = 2
};

#define TARGET_CHECK_SECTION_FLAGS pic30_check_section_flags

#define MCHP_CONFIGURATION_DATA_FILENAME "configuration.data"
#define MCHP_CONFIGURATION_HEADER_MARKER "Configuration Word Definitions: "
#define MCHP_CONFIGURATION_HEADER_VERSION "0001"
#define MCHP_CONFIGURATION_HEADER_SIZE \
  (sizeof (MCHP_CONFIGURATION_HEADER_MARKER) + 5)

#define mchp_processor_string pic30_target_cpu
extern const char *mchp_config_data_dir;

/*
 * some modes don't have a linear address map, so its not okay just to convert
 * them by gen_lowpart - there may be some 'work' required.
 */
/* all modes are linera, bar EDS */
#define TARGET_LINEAR_MODE(mode) \
  (!((mode == P32PEDSmode) || (mode == P32EDSmode)))


/* handle this pragma */
#define HANDLE_PRAGMA_PACK_PUSH_POP

#define TARGET_BUILD_VARIANT_TYPE_COPY pic30_build_variant_type_copy

#endif

