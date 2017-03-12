/* Definitions of target machine for GNU compiler.  MIPS version.
   Copyright (C) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996, 1997, 1998
   1999, 2000, 2001, 2002, 2003, 2004, 2005, 2007, 2008, 2009
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

/* Macro for conditional compilation of PIC32 only stuff */

#ifndef MCHP_H
#define MCHP_H

#include <safe-ctype.h>
#include "config/mchp-cci/cci-backend.h"

#undef TARGET_MCHP_PIC32MX
#define TARGET_MCHP_PIC32MX 1
#undef _BUILD_MCHP_
#define _BUILD_MCHP_ 1
#undef _BUILD_C32_
#define _BUILD_C32_ 1

#if 0
#define MCHP_DEBUG
#endif

extern const char *pic32_text_scn;
extern int         mchp_profile_option;

enum pic32_isa_mode
{
  pic32_isa_mips32r2  = 0,
  pic32_isa_mips16e   = 1,
  pic32_isa_micromips = 2,
  pic32_isa_unknown = 255
} ;

#undef DEFAULT_SIGNED_CHAR
#define DEFAULT_SIGNED_CHAR 1

/* Default to short double rather than long double */
/* chipKIT */
#undef TARGET_SHORT_DOUBLE
#define TARGET_SHORT_DOUBLE 0 

#define MCHP_CONFIGURATION_DATA_FILENAME "configuration.data"
#define MCHP_CONFIGURATION_HEADER_MARKER \
  "Daytona Configuration Word Definitions: "
#define MCHP_CONFIGURATION_HEADER_VERSION "0001"
#define MCHP_CONFIGURATION_HEADER_SIZE \
  (sizeof (MCHP_CONFIGURATION_HEADER_MARKER) + 5)

/*
** This is how to output a reference to a user-level label named NAME.
** `assemble_name_raw' uses this.
*/
#undef ASM_OUTPUT_LABELREF
#define ASM_OUTPUT_LABELREF(FILE, NAME) \
do {                     \
  const char * real_name;               \
  real_name = mchp_strip_name_encoding ((NAME));   \
  asm_fprintf (FILE, "%U%s", real_name);      \
} while (0)


/* Put at the end of the command given to the linker if -nodefaultlibs or
 * -nostdlib is not specified on the command line. This includes all the
 * standard libraries, the peripheral libraries if the -mno-peripheral-libs
 * option is not specified on the command line, and the processor-specific
 * peripheral library if -mno-peripheral-libs option is not specified, but
 * the -mprocessor option is specified.
 */

 /* chipKIT */
#undef  LIB_SPEC
#define LIB_SPEC "--start-group -lc -lsupc++ -lpic32 -lgcc -lm --end-group"

#undef LIBSTDCXX
#define LIBSTDCXX "supc++"
#undef LIBSTDCXX_STATIC
#define LIBSTDCXX_STATIC "supc++"


#define XC32CPPLIB_OPTION "-mxc32cpp-lib"

# undef  STARTFILE_SPEC
# define STARTFILE_SPEC "%{mmicromips: %s%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/crt0_micromips%O};\
  : %s%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/crt0_mips32r2%O} } \
  %{!mprocessor=* : crt0%O%s} \
  %{!A:%{!nostdlib:%{!mno-default-isr-vectors:%{mdebugger|mreserve=* : -l:software-debug-break.o} }}}\
  %{!A:%{!nostdlib:%{!mno-default-isr-vectors:%{!mdebugger : %{!mreserve=*: \
    %{mmicromips : -l:debug-exception-return-mm.o; \
    !mmicromips: -l:debug-exception-return.o}}} }}} \
 "

# undef STARTFILECXX_SPEC
# define STARTFILECXX_SPEC "%{mmicromips: %s%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/cpprt0_micromips%O} ;\
  : %s%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/cpprt0_mips32r2%O} } \
  %{!mprocessor=* : cpprt0%O%s} \
  crti%O%s crtbegin%O%s "

#undef ENDFILE_SPEC
#define ENDFILE_SPEC ""

#undef ENDFILECXX_SPEC
#define ENDFILECXX_SPEC "crtend%O%s crtn%O%s "

#undef LINK_COMMAND_SPEC
/* Add the PIC32 default linker script with the -T option */
/* When compiling with -mprocessor=32MX* or without the -mprocessor option, 
   use the ./ldscripts/elf32pic32mx.x file. When compiling for a newer device,
   Use ./proc/<procname>/p<procname>.ld. */
#define LINK_COMMAND_SPEC "\
    %{!fsyntax-only:%{!c:%{!M:%{!MM:%{!E:%{!S:\
    %(linker) \
    %{mprocessor=*:-p%*} \
    %{fuse-linker-plugin: \
    -plugin %(linker_plugin_file) \
    -plugin-opt=%(lto_wrapper) \
    -plugin-opt=%(lto_gcc) \
    %{static|static-libgcc:-plugin-opt=-pass-through=%(lto_libgcc)}	\
    %{static:-plugin-opt=-pass-through=-lc}	\
    %{O*:-plugin-opt=-O%*} \
    %{w:-plugin-opt=-w} \
    %{f*:-plugin-opt=-f%*} \
    %{m*:-plugin-opt=-m%*} \
    %{v:-plugin-opt=-v} \
    } \
    %{flto} %{fwhopr} %l " LINK_PIE_SPEC \
   "%X %{o*} %{A} %{d} %{e*} %{m} %{N} %{n} %{r}\
    %{s} %{t} %{u*} %{x} %{z} %{Z} %{!A:%{!nostdlib:%{!nostartfiles:%S} }}\
    %{Wno-poison-system-directories:--no-poison-system-directories}\
    %{Werror=poison-system-directories:--error-poison-system-directories}\
    %{static:} %{L*} %(mfwrap) %(link_libgcc) %o\
    %{fopenmp|ftree-parallelize-loops=*:%:include(libgomp.spec)%(link_gomp)} %(mflib)\
    %{fprofile-arcs|fprofile-generate*|coverage:-lgcov}\
    %{!pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: -l:default_isr_vectors.o} }}}}}\
    %{pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: -l:default_isr_vectors_pic.o} }}}}}\
    %{!pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: %{mprocessor=32*: \
      %{mmicromips : -l:pic32_software_reset-mm.o; \
      !mmicromips  : -l:pic32_software_reset.o}}}}}}}}\
    %{pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: %{mprocessor=32*|mprocessor=MGC*: \
      %{mmicromips : -l:pic32_software_reset_pic-mm.o; \
      !mmicromips  : -l:pic32_software_reset_pic.o}}}}}}}}\
    %{mreserve=*:--mreserve=%* } \
    %{!pie:%{T:%{T*};!T:-T %s%{mprocessor=32MX*:./ldscripts/elf32pic32mx.x; \
     :%{mprocessor=32mx*:./ldscripts/elf32pic32mx.x; \
     :%{!mprocessor=*:./ldscripts/elf32pic32mx.x; \
     :%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/p%*} %J%{mprocessor=*:.ld} }}}}} \
    %{pie:%{T:%{T*};!T:-T %s%{!mprocessor=*:elf32pic32mx_pic.x; \
     :%{mprocessor=*:./proc/%*} %J%{mprocessor=*:/p%*} %J%{mprocessor=*:_pic.ld} }}} \
    %{!nostdlib:%{!nodefaultlibs:%(link_ssp) %(link_gcc_c_sequence)}}\
    %{!A:%{!nostdlib:%{!nostartfiles:%E} }} \
    %{mlegacy-libc:%{mxc32cpp-lib:%e-legacy-libc not compatible with C++ projects}} \
    }}}}}}"

#undef LINK_COMMAND_SPEC_SUPPRESS_DEFAULT_SCRIPT
#define LINK_COMMAND_SPEC_SUPPRESS_DEFAULT_SCRIPT "\
%{!fsyntax-only:%{!c:%{!M:%{!MM:%{!E:%{!S:\
    %(linker) \
    %{fuse-linker-plugin: \
    -plugin %(linker_plugin_file) \
    -plugin-opt=%(lto_wrapper) \
    -plugin-opt=%(lto_gcc) \
    %{static|static-libgcc:-plugin-opt=-pass-through=%(lto_libgcc)}	\
    %{static:-plugin-opt=-pass-through=-lc}	\
    %{O*:-plugin-opt=-O%*} \
    %{w:-plugin-opt=-w} \
    %{f*:-plugin-opt=-f%*} \
    %{m*:-plugin-opt=-m%*} \
    %{v:-plugin-opt=-v} \
    } \
    %{flto} %{fwhopr} %l " LINK_PIE_SPEC \
   "%X %{o*} %{A} %{d} %{e*} %{m} %{N} %{n} %{r}\
    %{s} %{t} %{u*} %{x} %{z} %{Z} %{!A:%{!nostdlib:%{!nostartfiles:%S} }}\
    %{Wno-poison-system-directories:--no-poison-system-directories}\
    %{Werror=poison-system-directories:--error-poison-system-directories}\
    %{static:} %{L*} %(mfwrap) %(link_libgcc) %o\
    %{fopenmp|ftree-parallelize-loops=*:%:include(libgomp.spec)%(link_gomp)} %(mflib)\
    %{fprofile-arcs|fprofile-generate*|coverage:-lgcov}\
    %{!pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: -l:default_isr_vectors.o} }}}}}\
    %{pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: -l:default_isr_vectors_pic.o} }}}}}\
    %{!pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: %{mprocessor=32*: \
      %{mmicromips : -l:pic32_software_reset-mm.o; \
      !mmicromips  : -l:pic32_software_reset.o}}}}}}}} \
    %{pie:%{!A:%{!nostdlib:%{!nodefaultlibs:%{!nostartfiles:%{!mno-default-isr-vectors: %{mprocessor=32*|mprocessor=MGC*: \
      %{mmicromips : -l:pic32_software_reset_pic-mm.o; \
      !mmicromips  : -l:pic32_software_reset_pic.o}}}}}}}} \
    %{mreserve=*:--mreserve=%*} \
    %{T*} \
    %{!nostdlib:%{!nodefaultlibs:%(link_ssp) %(link_gcc_c_sequence)}}\
    %{!A:%{!nostdlib:%{!nostartfiles:%E} }} \
    %{mprocessor=*:-p%*} \
    %{mlegacy-libc:%{mxc32cpp-lib:%e-legacy-libc not compatible with C++ projects}} \
    }}}}}}"

/* Added on the linker command line after all user-specified -L options are
 * included.  This will add all the standard -L search paths, the
 * processor-specific library search path, and define _DEBUGGER if the
 * -mdebugger command-line option was specified. We define _DEBUGGER only
 * when memory isn't reserved with the newer -mreserve option.
 */
#define LINK_LIBGCC_SPEC "\
 %D -L %s%{mprocessor=*:./proc/%*; :./proc/32MXGENERIC} %{mdebugger:%{!mreserve=data*:--defsym _DEBUGGER=1}}"

#if 0 /* chipKIT */
#ifndef TARGET_EXTRA_PRE_INCLUDES
extern void pic32_system_include_paths(const char *root, const char *system,
                                       int nostdinc);
#define TARGET_EXTRA_PRE_INCLUDES pic32_system_include_paths
#endif
#endif

#ifdef DIR_SEPARATOR
#if DIR_SEPARATOR == '\\'
#define DIR_SEPARATOR_STR "\\"
#else
#define DIR_SEPARATOR_STR "/"
#endif
#endif

#ifndef MPLABC32_COMMON_INCLUDE_PATH
#define MPLABC32_COMMON_INCLUDE_PATH ""
#endif

#ifndef MPLABC32_LEGACY_COMMON_INCLUDE_PATH
#define MPLABC32_LEGACY_COMMON_INCLUDE_PATH DIR_SEPARATOR_STR \
                                     "lega-c"
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
%{mno-smart-io:--no-smart-io} %{msmart-io=0:--no-smart-io} \
%{mmemorysummary=*:--memorysummary %*} \
%{mfill=*:--fill=%*} "

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
%{mmicromips} %{mno-micromips} \
%{mmcu} %{mno-mcu} \
%{mdsp} %{mno-dsp} \
%{mdspr2} %{mno-dspr2} \
%(subtarget_asm_optimizing_spec) \
%(subtarget_asm_debugging_spec) \
%{mxgot:-xgot} \
%{!mno-abicalls:%{fPIC|fpic|fPIE|fpie:-KPIC}} \
%{mtune=*} %{v} \
%{!mpdr:-mno-pdr} \
%{mprocessor=*:-p%*} \
%{mfp64 : -mfp64 --no-construct-floats } \
%(target_asm_spec) \
%(subtarget_asm_spec)"

/* SUBTARGET_CC1_SPEC is passed to the compiler proper.  It may be
   overridden by subtargets.  */
#ifndef SUBTARGET_CC1_SPEC
#define SUBTARGET_CC1_SPEC ""
#endif

#ifndef MCHP_CCI_CC1_SPEC
#error MCHP_CCI_CC1_SPEC not defined
#endif

/* A spec that infers the -mdsp setting from an -march argument.  */
#undef BASE_DRIVER_SELF_SPECS
#define BASE_DRIVER_SELF_SPECS \
  "%{!mno-dsp: \
     %{march=24ke*|march=34kc*|march=34kf*|march=34kx*|march=1004k*: -mdsp} \
     %{march=74k*|march=m14ke*: %{!mno-dspr2: -mdspr2 -mdsp}}} \
   %{mprocessor=32MX* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs } \
   %{mprocessor=32mx* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs} \
   %{mprocessor=32MZ* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
   %{mprocessor=32mz* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
   %{mprocessor=MGC* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs } \
   %{mprocessor=mgc* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs} \
   %{mprocessor=IPSWICH : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
   %{mprocessor=MEC* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
   %{mprocessor=32MM*: -mmicromips -mpic32mmlibs} \
   %{mprocessor=32mm*: -mmicromips -mpic32mmlibs} \
   %{mpic32mxlibs : %{msmall-isa: -mips16}} \
   %{mpic32mzlibs : %{msmall-isa: -mmicromips}} \
   %{mpic32mmlibs : -mmicromips} \
   %{D__DEBUG : -mdebugger} \
   %{mprocessor=32*|mprocessor=MG* : ;:-mno-default-isr-vectors} \
   %{mhard-float : %{!mfp32 : -mfp64} } \
   %{mfp64 : -mhard-float } \
   %{!mfp64 : %{!mno-float : -msoft-float}} \
     "

/* CC1_SPEC is the set of arguments to pass to the compiler proper.  This
 * was copied from the one in mips.h, but that one had some problems and
 * contained the endian-selection options.
 */
#undef CC1_SPEC
#define CC1_SPEC "  \
 %{gline:%{!g:%{!g0:%{!g1:%{!g2: -g1}}}}} \
 %{G*} \
 %{minterlink-mips16: -minterlink-compressed} \
 %{minterlink-compressed: -mno-jals} \
 %{mmicromips : %{!minterlink-compressed : %{!mno-jals : -mjals } }} \
 %{msmall-isa : %{!minterlink-compressed : %{!mno-jals : -mjals } }} \
 %{mprocessor=32MX* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs } \
 %{mprocessor=32mx* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs} \
 %{mprocessor=32MZ* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
 %{mprocessor=32mz* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
 %{mprocessor=MGC* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs } \
 %{mprocessor=mgc* : %{msmall-isa:-mips16} %{!msmall-isa: %{mips16: -msmall-isa}} -mpic32mxlibs} \
 %{mprocessor=IPSWICH : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
 %{mprocessor=MEC* : %{msmall-isa:-mmicromips} %{!msmall-isa: %{mmicromips: -msmall-isa}} -mpic32mzlibs} \
 %{-mpic32mxlibs : %{msmall-isa: -mips16}} \
 %{-mpic32mzlibs : %{msmall-isa: -mmicromips}} \
 %{mno-float:-fno-builtin-fabs -fno-builtin-fabsf} \
 %{mlong-calls:-msmart-io=0} \
 %{msmart-io:%{msmart-io=*:%emay not use both -msmart-io and -msmart-io=LEVEL}} \
 %{mno-smart-io:%{msmart-io=*:%emay not use both -mno-smart-io and -msmart-io=LEVEL}} \
 %{mno-smart-io:-msmart-io=0} \
 %{msmart-io:-msmart-io=1} \
 %{save-temps: -fverbose-asm} \
 %{mips16e:-mips16} \
 %{mxc32cpp-lib:%{!mno-xc32cpp-lib: -msmart-io=0 }} \
 %{mips16: %{fexceptions: %{!mmips16-exceptions: %e-fexceptions with -mips16 not yet supported}}} \
 %{mips16: %{!mmips16-exceptions: -fno-exceptions}} \
 %{O2|Os|O3:%{!mtune:-mtune=4kec}} \
 %{D__DEBUG : -mdebugger} \
 %{fframe-base-loclist : %{!fno-var-tracking : -fvar-tracking}} \
 %{mit=profile : -fno-inline} \
 %{fnofallback : -mno-fallback } \
 %{-nofallback : -mno-fallback } \
 %{!fasynchronous-unwind-tables : -fno-asynchronous-unwind-tables } \
 %{!fdwarf2-cfi-asm : -fno-dwarf2-cfi-asm } \
 %{!mconfig-data-dir=* : -mconfig-data-dir= %J%s%{ mprocessor=* :./proc/%*; :./proc/32MXGENERIC}} \
 %{!ftoplevel-reorder : -fno-toplevel-reorder } \
 %{flto: %{!fno-fat-lto-objects: -ffat-lto-objects}} \
 %{O2|Os|O3:%{!mno-hi-addr-opt:-mhi-addr-opt}} \
 %(mchp_cci_cc1_spec) \
 %(subtarget_cc1_spec) \
"

#define CC1PLUS_SPEC " \
 %{!fenforce-eh-specs:-fno-enforce-eh-specs} \
 %{mxc32cpp-lib:%{!mno-xc32cpp-lib:%{!std=*:-std=gnu++11} -msmart-io=0 }} \
 %(subtarget_cc1plus_spec) \
 %{O2|Os|O3:%{!mno-hi-addr-opt:-mhi-addr-opt}} \
"

/* Preprocessor specs.  */

/* SUBTARGET_CPP_SPEC is passed to the preprocessor.  It may be
   overridden by subtargets.  */
#ifndef SUBTARGET_CPP_SPEC
#define SUBTARGET_CPP_SPEC ""
#endif

#undef CPP_SPEC
#define CPP_SPEC "%(subtarget_cpp_spec)\
    %{mreserve=*: -mdebugger} \
    %{mappio-debug:-D__APPIO_DEBUG} \
    "

/* This macro defines names of additional specifications to put in the specs
   that can be used in various specifications like CC1_SPEC.  Its definition
   is an initializer with a subgrouping for each command option.

   Each subgrouping contains a string constant, that defines the
   specification name, and a string constant that used by the GCC driver
   program.

   Do not define this macro if it does not need to do anything.  */
#undef EXTRA_SPECS
#define EXTRA_SPECS							\
  { "subtarget_cc1_spec", SUBTARGET_CC1_SPEC },				\
  { "subtarget_cpp_spec", SUBTARGET_CPP_SPEC },				\
  { "subtarget_asm_debugging_spec", SUBTARGET_ASM_DEBUGGING_SPEC },	\
  { "subtarget_asm_spec", SUBTARGET_ASM_SPEC },				\
  { "asm_abi_default_spec", "-" MULTILIB_ABI_DEFAULT },			\
  { "endian_spec", ENDIAN_SPEC },					\
  { "mchp_cci_cc1_spec", MCHP_CCI_CC1_SPEC },				\
  SUBTARGET_EXTRA_SPECS

#ifndef SUBTARGET_EXTRA_SPECS
#define SUBTARGET_EXTRA_SPECS
#endif

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
  /* -mcode-xonly is a traditional alias for -mcode-readable=pcrel and \
     -mno-data-in-code is a traditional alias for -mcode-readable=no. \
     The latter trumps the former.  */     \
  "%{mno-data-in-code: -mcode-readable=no}",    \
  "%{!mcode-readable=no: %{mcode-xonly: -mcode-readable=pcrel}}", \
  "%<mno-data-in-code %<mcode-xonly"

#undef SDB_DEBUGGING_INFO
#undef DWARF2_DEBUGGING_INFO
#define DWARF2_DEBUGGING_INFO 1         /* dwarf2 debugging info */

#ifndef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE DWARF2_DEBUG
#endif

/* By default, the GCC_EXEC_PREFIX_ENV prefix is "GCC_EXEC_PREFIX", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host GCC_EXEC_PREFIX */
#ifndef GCC_EXEC_PREFIX_ENV
#define GCC_EXEC_PREFIX_ENV "XC32_EXEC_PREFIX"
#endif

/* By default, the COMPILER_PATH_ENV is "COMPILER_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host COMPILER_PATH */
#ifndef COMPILER_PATH_ENV
#define COMPILER_PATH_ENV "XC32_COMPILER_PATH"
#endif

/* By default, the C_INCLUDE_PATH_ENV is "C_INCLUDE_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host C_INCLUDE_PATH */
#ifndef C_INCLUDE_PATH_ENV
#define C_INCLUDE_PATH_ENV "XC32_C_INCLUDE_PATH"
#endif

/* By default, the CPLUS_INCLUDE_PATH_ENV is "CPLUS_INCLUDE_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host CPLUS_INCLUDE_PATH */
#ifndef CPLUS_INCLUDE_PATH_ENV
#define CPLUS_INCLUDE_PATH_ENV "XC32_CPLUS_INCLUDE_PATH"
#endif

/* By default, the LIBRARY_PATH_ENV is "LIBRARY_PATH", however
   in a cross compiler, another environment variable might want to be used
   to avoid conflicts with the host any host LIBRARY_PATH */
#ifndef LIBRARY_PATH_ENV
#define LIBRARY_PATH_ENV "XC32_LIBRARY_PATH"
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

/*Enable parsing of #pragma pack(push,<n>) and #pragma pack(pop).
#define HANDLE_PRAGMA_PACK_PUSH_POP 1
*/

/* Use standard ELF-style local labels (not '$' as on early Irix).  */
#undef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."

/* Use periods rather than dollar signs in special g++ assembler names.  */
#define NO_DOLLAR_IN_LABEL

#define TARGET_ASM_OUTPUT_IDENT default_asm_output_ident_directive


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
    builtin_define_std ("PIC32");                           \
    builtin_define     ("__C32__");                         \
    builtin_define     ("__XC32");                          \
    builtin_define     ("__XC32__");                        \
    builtin_define     ("__XC");                            \
    builtin_define     ("__XC__");                          \
    if ((mchp_processor_string != NULL) && *mchp_processor_string) \
      {                                                     \
        if (strncmp (mchp_processor_string, "32MX", 4) == 0)  { \
        char *proc, *p;                                     \
        int setnum, memsize;                                \
        char *pinset;                                       \
        pinset = (char*)alloca(2);                          \
        pinset[1] = 0;                                      \
        gcc_assert(strlen(mchp_processor_string) < 20);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define_std ("PIC32MX");                     \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
                                                            \
        if (strchr(proc,'F') != NULL) {                     \
          sscanf (proc, "__32MX%6dF%6d%1c__", &setnum,      \
                  &memsize, &pinset[0]);                    \
          builtin_define_with_int_value                     \
             ("__PIC32_FEATURE_SET__",                      \
              setnum);                                      \
          builtin_define_with_int_value                     \
             ("__PIC32_FEATURE_SET",                        \
              setnum);                                      \
          builtin_define_with_int_value                     \
             ("__PIC32_MEMORY_SIZE__",                      \
              memsize);                                     \
          builtin_define_with_int_value                     \
             ("__PIC32_MEMORY_SIZE",                        \
              memsize);                                     \
          builtin_define_with_value                         \
             ("__PIC32_PIN_SET__",                          \
              &pinset[0], 1);                               \
          builtin_define_with_value                         \
             ("__PIC32_PIN_SET",                            \
              &pinset[0], 1);                               \
        }                                                   \
        }                                                   \
        else if (strncmp (mchp_processor_string, "32MZ", 4) == 0)  { \
        char *proc=NULL, *p=NULL;                           \
        int pincount, flashsize;                            \
        char *featureset=NULL;                              \
        char *productgroup=NULL;                            \
        char *macroname=NULL;                               \
        int index = 0;                                      \
        gcc_assert(strlen(mchp_processor_string) < 20);     \
        featureset = (char*)alloca(4);                      \
        featureset[2] = featureset[3] = '\0';               \
        productgroup = (char*)alloca(3);                    \
        productgroup[1] = productgroup[2] = '\0';           \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        macroname = (char*)alloca (                         \
            strlen("__PIC32_FEATURE_SETnnnn__")+1);         \
        builtin_define_std ("PIC32MZ");                     \
        proc = (char*)alloca (                              \
          strlen (mchp_processor_string) + 6);              \
        gcc_assert(proc!=NULL);                             \
        gcc_assert(featureset!=NULL);                       \
        gcc_assert(productgroup!=NULL);                     \
        gcc_assert(macroname!=NULL);                        \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
          sscanf (proc, "__32MZ%4d%2c%1c%4d__",             \
                  &flashsize, &featureset[0],               \
                  &productgroup[0], &pincount);             \
          builtin_define_with_int_value                     \
             ("__PIC32_FLASH_SIZE__",                       \
              flashsize);                                   \
          builtin_define_with_int_value                     \
             ("__PIC32_FLASH_SIZE",                         \
              flashsize);                                   \
          builtin_define_with_value                         \
             ("__PIC32_FEATURE_SET__",                      \
              &featureset[0], 1);                           \
          builtin_define_with_value                         \
             ("__PIC32_FEATURE_SET",                        \
              &featureset[0], 1);                           \
          index = strlen(featureset);                       \
          gcc_assert(index<3);                              \
          while (index--) {                                 \
          snprintf(macroname,                               \
            strlen("__PIC32_FEATURE_SETnn__")+1,            \
            "__PIC32_FEATURE_SET%d__", index);              \
          builtin_define_with_int_value                     \
             (macroname,                                    \
              featureset[index]);                           \
          };                                                \
          index = strlen(featureset);                       \
          while (index--) {                                 \
          snprintf(macroname,                               \
            strlen("__PIC32_FEATURE_SETnn")+1,              \
            "__PIC32_FEATURE_SET%d", index);                \
          builtin_define_with_int_value                     \
             (macroname,                                    \
              featureset[index]);                           \
          };                                                \
          builtin_define_with_int_value                     \
             ("__PIC32_PRODUCT_GROUP__",                    \
              productgroup[0]);                             \
          builtin_define_with_int_value                     \
             ("__PIC32_PRODUCT_GROUP",                      \
              productgroup[0]);                             \
          builtin_define_with_int_value                     \
             ("__PIC32_PIN_COUNT__",                        \
              pincount);                                    \
          builtin_define_with_int_value                     \
             ("__PIC32_PIN_COUNT",                          \
              pincount);                                    \
        }                                                   \
        else if (strncmp (mchp_processor_string, "BT55", 2) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define ("__BT55");                          \
        builtin_define ("__BT55__");                        \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else if (strncmp (mchp_processor_string, "MGC", 3) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define ("__MGC");                           \
        builtin_define ("__MGC__");                         \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else if (strncmp (mchp_processor_string, "MEC14", 5) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define ("__MEC");                           \
        builtin_define ("__MEC14");                         \
        builtin_define ("__IPSWICH");                       \
        builtin_define ("__MEC__");                         \
        builtin_define ("__MEC14__");                       \
        builtin_define ("__IPSWICH__");                     \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else if (strncmp (mchp_processor_string, "32MM", 4) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define_std ("PIC32MM");                     \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else if (strncmp (mchp_processor_string, "32MK", 4) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define_std ("PIC32MK");                     \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else if (strncmp (mchp_processor_string, "32WK", 4) == 0)  { \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        builtin_define_std ("PIC32WK");                     \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
        else {                                              \
        char *proc, *p;                                     \
        gcc_assert(strlen(mchp_processor_string) < 10);     \
        for (p = (char *)mchp_processor_string ; *p ; p++)  \
          *p = TOUPPER (*p);                                \
        proc = (char*)alloca (strlen (mchp_processor_string) + 6); \
        gcc_assert (proc!=NULL);                            \
        sprintf (proc, "__%s__", mchp_processor_string);    \
        gcc_assert (strlen(proc)>0);                        \
        builtin_define (proc);                              \
        }                                                   \
      }                                                     \
    else                                                    \
      {                                                     \
        builtin_define ("__32MXGENERIC__");                 \
        builtin_define ("__32MXGENERIC");                   \
      }                                                     \
    if (1)                                                  \
      {                                                     \
    if (mchp_it_transport && *mchp_it_transport)            \
      {                                                     \
        char *mchp_it_define = NULL;                        \
        if (strlen (mchp_it_transport) > 100)               \
          {                                                     \
            warning(0,"-mit=%s invalid; defaulting to -mit=dc", \
                    mchp_it_transport);                         \
            mchp_it_transport = "dc";                           \
          }                                                     \
          {                                                     \
            char *mchp_it_option = NULL;                        \
            char *mchp_it_option_arg = NULL;                    \
            mchp_it_option = (char *)alloca(strlen(mchp_it_transport)); \
            if (strcasecmp(mchp_it_transport,"profile") == 0) { \
               mchp_profile_option = 1;                         \
               mchp_it_option = NULL;                           \
               builtin_define ("__HAS_FUNCTIONLEVELPROF");      \
             }                                                  \
            else {                                              \
            if (strchr(mchp_it_transport, '(') < strchr(mchp_it_transport, ')'))  \
             {                                                        \
              mchp_it_option_arg =(char *) alloca(strlen(mchp_it_transport)); \
              sscanf (mchp_it_transport, "%10[^(](%90[^)])",          \
                mchp_it_option, mchp_it_option_arg);                  \
              }                                                       \
            else                                                      \
               {                                                      \
                 mchp_it_option = (char *)mchp_it_transport;          \
               }                                                      \
            mchp_it_define =(char *)                                  \
              alloca (strlen ("__IT_TRANSPORT=xx") +                  \
                      strlen(mchp_it_option) + 1);                    \
            snprintf (mchp_it_define, strlen ("__IT_TRANSPORT= ") +     \
                      strlen(mchp_it_option) + 1, "__IT_TRANSPORT=%s",  \
                      mchp_it_option);                                \
            builtin_define (mchp_it_define);                          \
            if (mchp_it_option_arg != NULL) {                         \
              int i=1;                                                \
              char *mchp_it_option_define = NULL;                     \
              char *s,*c;                                             \
              c = mchp_it_option_arg;                                 \
              do {                                                    \
                s = c;                                                \
                for (; *c && *c != ','; c++);                         \
                if (*c) *c++ = 0;                                     \
                mchp_it_option_define =(char *)                       \
                  alloca(strlen("__IT_TRANSPORT_OPTION%d=%s")+        \
                  strlen(s));                                         \
                snprintf(mchp_it_option_define,                       \
                  strlen("__IT_TRANSPORT_OPTION%d=%s")+               \
                  strlen(s), "__IT_TRANSPORT_OPTION%d=%s",i++,s);     \
                builtin_define(mchp_it_option_define);                \
               } while (*c);                                          \
              }                                                       \
          }}                                                          \
      }                                                               \
      }                                                               \
    if ((version_string != NULL) && *version_string)        \
      {                                                     \
        char *Microchip;                                    \
        int pic32_compiler_version;                         \
        gcc_assert(strlen(version_string) < 80);            \
        Microchip = (char *)strrchr (version_string, 'v');  \
        if (Microchip != NULL)                              \
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
             pic32_compiler_version = (major*1000) + (minor*10);  \
          }                                                 \
        else                                                \
          {                                                 \
            fatal_error ("internal error: version_string == NULL");     \
            builtin_define_with_int_value ("__C32_VERSION__", -1);      \
            builtin_define_with_int_value ("__XC32_VERSION__", -1);     \
            builtin_define_with_int_value ("__XC32_VERSION", -1);       \
            builtin_define_with_int_value ("__XC_VERSION__", -1);       \
            builtin_define_with_int_value ("__XC_VERSION", -1);       \
          }                                                             \
        builtin_define_with_int_value ("__C32_VERSION__", pic32_compiler_version);  \
        builtin_define_with_int_value ("__XC32_VERSION__", pic32_compiler_version); \
        builtin_define_with_int_value ("__XC32_VERSION", pic32_compiler_version);   \
        builtin_define_with_int_value ("__XC_VERSION__", pic32_compiler_version);   \
        builtin_define_with_int_value ("__XC_VERSION", pic32_compiler_version);     \
      }                                                     \
                                                            \
      mchp_init_cci(pfile);    \
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
  c_register_pragma(0, "vector", mchp_handle_vector_pragma); \
  c_register_pragma(0, "interrupt", mchp_handle_interrupt_pragma); \
  c_register_pragma(0, "config", mchp_handle_config_pragma); \
  c_register_pragma(0, "config_alt", mchp_handle_config_alt_pragma); \
  c_register_pragma(0, "config_bf1", mchp_handle_config_bf1_pragma); \
  c_register_pragma(0, "config_abf1", mchp_handle_config_abf1_pragma); \
  c_register_pragma(0, "config_bf2", mchp_handle_config_bf2_pragma); \
  c_register_pragma(0, "config_abf2", mchp_handle_config_abf2_pragma); \
  c_register_pragma(0, "align", mchp_handle_align_pragma); \
  c_register_pragma(0, "section", mchp_handle_section_pragma); \
  c_register_pragma(0, "printf_args", mchp_handle_printf_args_pragma); \
  c_register_pragma(0, "scanf_args", mchp_handle_scanf_args_pragma); \
  c_register_pragma(0, "keep", mchp_handle_keep_pragma); \
  c_register_pragma(0, "coherent", mchp_handle_coherent_pragma); \
  c_register_pragma(0, "optimize", mchp_handle_optimize_pragma); \
  c_register_pragma(0, "region", mchp_handle_region_pragma); \
  mchp_init_cci_pragmas(); \
  }

/* There are no additional prefixes to try after STANDARD_EXEC_PREFIX. */
#undef MD_EXEC_PREFIX

/* There are no additional prefixes to try after STANDARD_STARTFILE_PREFIX. */
#undef MD_STARTFILE_PREFIX

/* Disallow big endian even when the command-line option is passed */
#undef TARGET_BIG_ENDIAN
#define TARGET_BIG_ENDIAN 0

#if 1
#undef TARGET_STRIP_NAME_ENCODING
#define TARGET_STRIP_NAME_ENCODING mchp_strip_name_encoding
#endif

/*Disable options not supported by PIC32*/

#undef TARGET_64BIT
#define TARGET_64BIT ((target_flags & MASK_64BIT) != 0)

/*
#undef MASK_MIPS3D
#define MASK_MIPS3D 0
*/
#undef TARGET_MIPS3D
#define TARGET_MIPS3D ((target_flags & MASK_MIPS3D) != 0)

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

#undef SECTION_FLAGS_INT
#define SECTION_FLAGS_INT uint64_t

/* the flags may be any length if surrounded by | */
#define MCHP_EXTENDED_FLAG  "|"
#define MCHP_PROG_FLAG       MCHP_EXTENDED_FLAG "pm"        MCHP_EXTENDED_FLAG
#define MCHP_DATA_FLAG       MCHP_EXTENDED_FLAG "dm"        MCHP_EXTENDED_FLAG
#define MCHP_CONST_FLAG      MCHP_EXTENDED_FLAG "rd"        MCHP_EXTENDED_FLAG
#define MCHP_SERIALMEM_FLAG  MCHP_EXTENDED_FLAG "sm"        MCHP_EXTENDED_FLAG
#define MCHP_RAMFUNC_FLAG    MCHP_EXTENDED_FLAG "rf"        MCHP_EXTENDED_FLAG
#define MCHP_PRST_FLAG       MCHP_EXTENDED_FLAG "persist"   MCHP_EXTENDED_FLAG
#define MCHP_BSS_FLAG        MCHP_EXTENDED_FLAG "bss"       MCHP_EXTENDED_FLAG

#define MCHP_MERGE_FLAG      MCHP_EXTENDED_FLAG "mrg"       MCHP_EXTENDED_FLAG
#define MCHP_NOLOAD_FLAG     MCHP_EXTENDED_FLAG "nl"        MCHP_EXTENDED_FLAG
#define MCHP_ALGN_FLAG       MCHP_EXTENDED_FLAG "a"         MCHP_EXTENDED_FLAG
#define MCHP_RALGN_FLAG      MCHP_EXTENDED_FLAG "ra"        MCHP_EXTENDED_FLAG
#define MCHP_ADDR_FLAG       MCHP_EXTENDED_FLAG "addr"      MCHP_EXTENDED_FLAG

#define MCHP_FCNN_FLAG       MCHP_EXTENDED_FLAG "Nf"        MCHP_EXTENDED_FLAG
#define MCHP_FCNS_FLAG       MCHP_EXTENDED_FLAG "Sf"        MCHP_EXTENDED_FLAG
#define MCHP_SFR_FLAG        MCHP_EXTENDED_FLAG "sfr"       MCHP_EXTENDED_FLAG
#define MCHP_NEAR_FLAG       MCHP_EXTENDED_FLAG "near"      MCHP_EXTENDED_FLAG
#define MCHP_KEEP_FLAG       MCHP_EXTENDED_FLAG "keep"      MCHP_EXTENDED_FLAG
#define MCHP_COHERENT_FLAG   MCHP_EXTENDED_FLAG "coherent"  MCHP_EXTENDED_FLAG
#define MCHP_REGION_FLAG     MCHP_EXTENDED_FLAG "region"    MCHP_EXTENDED_FLAG
#define MCHP_CO_SHARED_FLAG  MCHP_EXTENDED_FLAG "shared"    MCHP_EXTENDED_FLAG

#define MCHP_IS_NAME_P(NAME,IS) (strncmp(NAME, IS, sizeof(IS)-1) == 0)
#define MCHP_HAS_NAME_P(NAME,HAS) (strstr(NAME, HAS))

#define ENCODED_NAME_P(SYMBOL_NAME) \
  ((SYMBOL_NAME[0] == MCHP_EXTENDED_FLAG[0]) ? \
    (strrchr(SYMBOL_NAME,MCHP_EXTENDED_FLAG[0]) - SYMBOL_NAME) + 1 : 0)

/*
** Output before program text section
*/
#undef TEXT_SECTION_ASM_OP
#define TEXT_SECTION_ASM_OP mchp_text_section_asm_op()
#undef READONLY_DATA_SECTION_ASM_OP
#define READONLY_DATA_SECTION_ASM_OP	mchp_rdata_section_asm_op()	/* read-only data */

#undef TARGET_ASM_SELECT_SECTION
#define TARGET_ASM_SELECT_SECTION mchp_select_section
/* CHANGE TO NAMED SECTION */
#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION mchp_asm_named_section

/*
** Output before writable data.
*/
#undef DATA_SECTION_ASM_OP
#define DATA_SECTION_ASM_OP mchp_data_section_asm_op()

#undef BSS_SECTION_ASM_OP
#define BSS_SECTION_ASM_OP mchp_bss_section_asm_op()

#undef SBSS_SECTION_ASM_OP
#define SBSS_SECTION_ASM_OP mchp_sbss_section_asm_op()

#undef SDATA_SECTION_ASM_OP
#define SDATA_SECTION_ASM_OP mchp_sdata_section_asm_op()

#if 1
#define HAS_INIT_SECTION 1
#undef INIT_SECTION_ASM_OP
#define INIT_SECTION_ASM_OP "\t.section .init, code"

#undef FINI_SECTION_ASM_OP
#define FINI_SECTION_ASM_OP "\t.section .fini, code"
#endif

#if 1
#undef CTORS_SECTION_ASM_OP
#define CTORS_SECTION_ASM_OP "\t.section .ctors, code"

#undef DTORS_SECTION_ASM_OP
#define DTORS_SECTION_ASM_OP "\t.section .dtors, code"
#endif

#undef TARGET_ASM_FILE_END
#define TARGET_ASM_FILE_END mchp_file_end

/* GET SECTION TYPE FLAGS */
#undef TARGET_SECTION_TYPE_FLAGS
#define TARGET_SECTION_TYPE_FLAGS mchp_section_type_flags

#undef MIPS_SUBTARGET_FUNCTION_END_PROLOGUE

#undef MIPS_SUBTARGET_SET_CURRENT_FUNCTION

#undef MIPS_SUBTARGET_PREPARE_FUNCTION_START
#define MIPS_SUBTARGET_PREPARE_FUNCTION_START(fndecl) \
  mchp_prepare_function_start(fndecl)

#undef MIPS_SUBTARGET_SUPPRESS_PROLOGUE
#define MIPS_SUBTARGET_SUPPRESS_PROLOGUE() mchp_suppress_prologue()

#undef MIPS_SUBTARGET_FUNCTION_PROFILING_EPILOGUE
#define MIPS_SUBTARGET_FUNCTION_PROFILING_EPILOGUE(sibcall_p)  mchp_function_profiling_epilogue(sibcall_p)

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

#undef MIPS_DISABLE_INTERRUPT_ATTRIBUTE
#define MIPS_DISABLE_INTERRUPT_ATTRIBUTE
#undef MIPS_DISABLE_NOMICROMIPS_ATTRIBUTE
#define MIPS_DISABLE_NOMICROMIPS_ATTRIBUTE

#undef MIPS_SUBTARGET_ATTRIBUTE_TABLE
#define MIPS_SUBTARGET_ATTRIBUTE_TABLE                                                        \
    /* { name, min_len, max_len, decl_req, type_req, fn_type_req, handler,om_diagnostics } */  \
    /* Microchip: allow functions to be specified as interrupt handlers */                    \
    { "interrupt",        0, 1,  false, true,  true, mchp_interrupt_attribute, false},        \
    { "vector",           1, 256, true,  false, false, mchp_vector_attribute, false },        \
    { "at_vector",        1, 1,   true,  false, false, mchp_at_vector_attribute, false },     \
    { "nomicromips",      0, 0,   true,  false, false, mchp_nomicromips_attribute, false },   \
    /* also allow functions to be created without prologue/epilogue code */                   \
    { "naked",            0, 0,  true,  false, false, mchp_naked_attribute, false },          \
    { "address",          1, 1,  false, false, false, mchp_address_attribute, false },        \
    { "space",            1, 1,  false, false, false, mchp_space_attribute, false },          \
    { "persistent",       0, 0,  false, false, false, mchp_persistent_attribute, false },     \
    { "ramfunc",          0, 0,  false, true,  true,  mchp_ramfunc_attribute, false },        \
    { "unsupported",      0, 1,  false, false, false, mchp_unsupported_attribute, false },    \
    { "target_error",     1, 1,  false, false, false, mchp_target_error_attribute, false },   \
    { "keep",             0, 0,  false, false, false, mchp_keep_attribute, false },           \
    { "coherent",         0, 0,  false, false, false, mchp_coherent_attribute, false },       \
    { "crypto",           0, 0,  false, false, false, mchp_crypto_attribute, false },	      \
    { "unique_section",   0, 0,  true,  false, false, mchp_unique_section_attribute, false }, \
    { "region",           1, 1,  false, false, false, mchp_region_attribute, false },         \
    { "function_replacement_prologue",  0, 0,  true, false,  false,  mchp_frp_attribute, false },        \
    { "shared",           0, 0,  false, false, false, mchp_shared_attribute, false },         \
    /* prevent FPU usage in ISRs */                                                           \
    { "no_fpu",           0, 0,  false, true,  true,  NULL, false },


//extern enum mips_function_type_tag current_function_type;

#undef MIPS_SUBTARGET_SAVE_REG_P
#define MIPS_SUBTARGET_SAVE_REG_P(regno) \
  mchp_subtarget_save_reg_p(regno)

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

#undef MIPS_SUBTARGET_OVERRIDE_OPTIONS
#define MIPS_SUBTARGET_OVERRIDE_OPTIONS() \
  mchp_subtarget_override_options()

#undef MIPS_SUBTARGET_OVERRIDE_OPTIONS1
#define MIPS_SUBTARGET_OVERRIDE_OPTIONS1() \
  mchp_subtarget_override_options1()

#undef MIPS_SUBTARGET_OVERRIDE_OPTIONS2
#define MIPS_SUBTARGET_OVERRIDE_OPTIONS2() \
  mchp_subtarget_override_options2()

#undef MIPS_SUBTARGET_MIPS16_ENABLED
#define MIPS_SUBTARGET_MIPS16_ENABLED(decl) \
  mchp_subtarget_mips16_enabled(decl)

#undef MIPS_SUBTARGET_MICROMIPS_ENABLED
#define MIPS_SUBTARGET_MICROMIPS_ENABLED(decl) \
  mchp_subtarget_micromips_enabled(decl)

#undef MIPS_SUBTARGET_MIPS32_ENABLED
#define MIPS_SUBTARGET_MIPS32_ENABLED() \
  mchp_subtarget_mips32_enabled()

#undef MIPS_SUBTARGET_ENCODE_SECTION_INFO
#define MIPS_SUBTARGET_ENCODE_SECTION_INFO(decl,rtl,first) \
  mchp_subtarget_encode_section_info(decl,rtl,first)

#define USE_SELECT_SECTION_FOR_FUNCTIONS 1

#undef JUMP_TABLES_IN_TEXT_SECTION
#define JUMP_TABLES_IN_TEXT_SECTION 1

#undef SUPPORTS_DISCRIMINATOR
#define SUPPORTS_DISCRIMINATOR 0

/* A few bitfield locations for the coprocessor registers */
#define CAUSE_IPL       10
#define SR_IPL          10
#define SR_IE           0
#define SR_EXL          1
#define SR_ERL          2

#undef TARGET_ASM_CONSTRUCTOR
#define TARGET_ASM_CONSTRUCTOR default_named_section_asm_out_constructor
#undef TARGET_ASM_DESTRUCTOR
#define TARGET_ASM_DESTRUCTOR default_named_section_asm_out_destructor

#undef TARGET_USE_JCR_SECTION
#define TARGET_USE_JCR_SECTION 0
#undef JCR_SECTION_NAME

#undef TARGET_APPLY_PRAGMA
#define TARGET_APPLY_PRAGMA mchp_apply_pragmas

/* Initialize the GCC target structure.  */
#if 0 /* chipKIT */
#undef TARGET_OVERRIDE_OPTIONS_AFTER_CHANGE
#define TARGET_OVERRIDE_OPTIONS_AFTER_CHANGE mchp_override_options_after_change
#endif

/* True if we can optimize sibling calls.  For simplicity, we only
   handle cases in which call_insn_operand will reject invalid
   sibcall addresses.  There are two cases in which this isn't true:

      - TARGET_MIPS16.  call_insn_operand accepts constant addresses
	but there is no direct jump instruction.  It isn't worth
	using sibling calls in this case anyway; they would usually
	be longer than normal calls.

      - TARGET_USE_GOT && !TARGET_EXPLICIT_RELOCS.  call_insn_operand
	accepts global constants, but all sibcalls must be indirect.  */
#undef TARGET_SIBCALLS
#define TARGET_SIBCALLS \
  (!TARGET_MIPS16 && (!TARGET_USE_GOT || TARGET_EXPLICIT_RELOCS) && !mchp_profile_option)

#define PIC32_SUPPORT_CRYPTO_ATTRIBUTE 1

#undef COLLECT2_RELATIVE_LD_FILE_NAME
#define COLLECT2_RELATIVE_LD_FILE_NAME "../../../../pic32-ld"

#if defined(__MINGW32__)
#define MCHP_CONVERT_BACKSLASH(string)  \
do {                                    \
  char *convert = string;               \
  while (*convert != '\0') {            \
  if (*convert == '\\')                 \
    *convert = '/';                     \
  convert++;                            \
  }                                     \
} while(0)
#else
#define MCHP_CONVERT_BACKSLASH(string)
#endif

/*  Open the pic32 device-specific resource file and append the -mhard-float and -mfp64 options
    if the device supports the hardware FPU.
 */
#undef GCC_DRIVER_HOST_INITIALIZATION
#define GCC_DRIVER_HOST_INITIALIZATION  \
    {                                                       \
        struct resource_introduction_block *rib = 0;        \
        struct resource_data d;                             \
        unsigned int pic32_device_mask=0;                   \
        int pic32_num_register_sets=2;                      \
        unsigned int pic32_procid=0;                        \
        unsigned int index=0;                               \
        const char *mprocessor_string = "none";             \
        char *pic32_resource_file = "none";                 \
        char has_explicit_float_option = 0;                 \
        char has_explicit_dsp_option = 0;                   \
        char* path = make_relative_prefix(decoded_options[0].arg, "/bin/bin", "/bin");        \
        for (index=1; index < decoded_options_count; index++) {                               \
            if (decoded_options[index].opt_index == OPT_mprocessor_) {                        \
                mprocessor_string = decoded_options[index].arg;                               \
                if (mprocessor_string == NULL) {                                              \
                   error("missing argument to \"-mprocessor=\"");                             \
                   return -1;                                                                 \
                  }                                                                           \
                pic32_resource_file = (char*)xmalloc(strlen(path) +                           \
                strlen("./xc32_device.info") + 1);                                            \
                if (pic32_resource_file) {                                                    \
                    sprintf(pic32_resource_file, "%s/xc32_device.info", path);                \
                    MCHP_CONVERT_BACKSLASH(pic32_resource_file);                              \
                    rib = read_device_rib(pic32_resource_file, (char*)mprocessor_string);     \
                    if (rib == 0) {                                                           \
                        error("Could not open resource file for: %qs at %qs", mprocessor_string, pic32_resource_file);  \
                        return -1;                                                            \
                    }                                                                         \
                    if (strcmp(rib->tool_chain,"XC32")) {     \
                        error("Invalid resource file\n");     \
                        close_rib();                          \
                        return -1;                            \
                    }                                         \
                    if (rib->field_count >= 4) {              \
                        int record;                           \
                        for (record = 0; move_to_record(record); record++) {       \
                            read_value(rik_string, &d);                            \
                            if (strcmp(d.v.s, mprocessor_string) == 0) {           \
                                read_value(rik_int, &d);                           \
                                if (d.v.i & IS_DEVICE_ID) {                        \
                                    pic32_device_mask = (d.v.i & (~IS_DEVICE_ID)); \
                                    read_value(rik_int, &d);                       \
                                    pic32_num_register_sets = d.v.i;               \
                                    read_value(rik_int, &d);                       \
                                    pic32_procid = d.v.i;                          \
                                }                                                  \
                            }                                                      \
                        }                                                          \
                    }                                                              \
                }                                                                  \
            } else if (decoded_options[index].opt_index == OPT_mno_float ||        \
                    decoded_options[index].opt_index == OPT_msoft_float ||         \
                    decoded_options[index].opt_index == OPT_mhard_float ||         \
                    decoded_options[index].opt_index == OPT_mfp32 ||               \
                    decoded_options[index].opt_index == OPT_mfp64)                 \
                { has_explicit_float_option++; }                                   \
            else if (decoded_options[index].opt_index == OPT_mno_dspr2 ||          \
                    decoded_options[index].opt_index == OPT_mdspr2 ||              \
                    decoded_options[index].opt_index == OPT_mdsp)                  \
                { has_explicit_dsp_option++; }                                     \
        }                                                                          \
      if ((pic32_device_mask & HAS_FLTPT) && (has_explicit_float_option == 0)) {   \
        decoded_options_count += 2;                                                \
        decoded_options = XRESIZEVEC (struct cl_decoded_option,                    \
                                      decoded_options,                             \
                                      decoded_options_count);                      \
        generate_option (OPT_mhard_float, NULL, 1, CL_DRIVER,                      \
          &(decoded_options)[decoded_options_count-2]);                            \
        generate_option (OPT_mfp64, NULL, 1, CL_DRIVER, &(decoded_options)[decoded_options_count-1]);  \
      }                                                                            \
      if ((pic32_device_mask & HAS_DSPR2) && (has_explicit_dsp_option == 0)) {     \
        decoded_options_count += 2;                                                \
        decoded_options = XRESIZEVEC (struct cl_decoded_option,                    \
                                      decoded_options,                             \
                                      decoded_options_count);                      \
        generate_option (OPT_mdspr2, NULL, 1, CL_DRIVER,                           \
          &(decoded_options)[decoded_options_count-2]);                            \
        generate_option (OPT_mdsp, NULL, 1, CL_DRIVER, &(decoded_options)[decoded_options_count-1]);  \
      }                                                                            \
    }

#endif /* MCHP_H */
