/* Definitions for SH based uClinux system using ELF objects with
   special linker post-processing to produce FLAT executables.

   Copyright (C) 2008
   Free Software Foundation, Inc.

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

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!shared:crt1.o%s} crti.o%s \
   %{static:crtbeginT.o%s;shared|pie:crtbeginS.o%s;:crtbegin.o%s}" \
   FDPIC_STARTFILE_SPEC

#undef  ENDFILE_SPEC
#define ENDFILE_SPEC "%{shared|pie:crtendS.o%s;:crtend.o%s} crtn.o%s"

#define LINK_EH_SPEC "%{mfdpic:%{!static:--eh-frame-hdr}} "

#ifdef FDPIC_DEFAULT
#undef SUBTARGET_LINK_SPEC
#define SUBTARGET_LINK_SPEC \
  "%{shared:-shared} \
   %{!static: \
     %{rdynamic:-export-dynamic} \
     %{!dynamic-linker:-dynamic-linker /lib/ld-uClibc.so.0}} \
   %{static:-static}"
#undef SUBTARGET_LINK_EMUL_SUFFIX
#define SUBTARGET_LINK_EMUL_SUFFIX "_fd"
#undef SUBTARGET_DRIVER_SELF_SPECS
#define SUBTARGET_DRIVER_SELF_SPECS "%{!mno-fdpic:-mfdpic} "
#undef SUBTARGET_ASM_SPEC
#define SUBTARGET_ASM_SPEC "%{!mno-fdpic:--fdpic}"
#define FDPIC_STARTFILE_SPEC "%{!mno-fdpic: crtreloc.o%s}"
#else
#undef SUBTARGET_LINK_SPEC
#define SUBTARGET_LINK_SPEC "%{shared:-shared} %{static:-static} %{!elf2flt*:-elf2flt}"
#undef SUBTARGET_LINK_EMUL_SUFFIX
#define SUBTARGET_LINK_EMUL_SUFFIX "_uclinux"
#define FDPIC_STARTFILE_SPEC "%{mfdpic: crtreloc.o%s}"
#endif

/* While the speed-optimized implementations of udivsi3_i4i / sdivsi3_i4i
   in libgcc are not available for SH2, the space-optimized ones in
   libgcc-Os-4-200 are.  Thus, when not optimizing for space, link
   libgcc-Os-4-200 after libgcc, so that -mdiv=call-table works for -m2.  */
#undef LIBGCC_SPEC
#define LIBGCC_SPEC "%{Os: -lgcc-Os-4-200} -lgcc %{!Os: -lgcc-Os-4-200}"

/* we have init/fini section. */
#define HAS_INIT_SECTION

/* Bring in standard linux defines */
#undef TARGET_OS_CPP_BUILTINS
#define TARGET_OS_CPP_BUILTINS()		\
  do						\
    {						\
	builtin_define ("__gnu_linux__");	\
	builtin_define_std ("linux");		\
	builtin_define_std ("unix");		\
	builtin_assert ("system=linux");	\
	builtin_assert ("system=unix");		\
	builtin_assert ("system=posix");	\
	builtin_define ("__uClinux__");		\
    }						\
  while (0)

/* The GNU C++ standard library requires that these macros be defined.  */
#undef CPLUSPLUS_CPP_SPEC
#define CPLUSPLUS_CPP_SPEC "-D_GNU_SOURCE %(cpp)"

/* Since libgcc is compiled with -fpic for this target, we can't use
   __sdivsi3_1 as the division strategy for -O0 and -Os.  */
#undef SH_DIV_STRATEGY_DEFAULT
#define SH_DIV_STRATEGY_DEFAULT SH_DIV_CALL2
#undef SH_DIV_STR_FOR_SIZE
#define SH_DIV_STR_FOR_SIZE "call2"

/* The uclinux binary format relies on relocations against a segment being
   within that segment.  Conservatively apply this rule to individual
   sections.  */
#undef SH_OFFSETS_MUST_BE_WITHIN_SECTIONS_P
#define SH_OFFSETS_MUST_BE_WITHIN_SECTIONS_P 1

/* CSL LOCAL */
#undef SUBTARGET_OVERRIDE_OPTIONS
#define SUBTARGET_OVERRIDE_OPTIONS					\
do {									\
  /* __thread is not supported by uClibc.  */				\
  targetm.have_tls = 0;							\
} while (0)
