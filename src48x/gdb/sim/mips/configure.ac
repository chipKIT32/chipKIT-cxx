dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.64)dnl
AC_INIT(Makefile.in)
sinclude(../common/acinclude.m4)

SIM_AC_COMMON

dnl Options available in this module
SIM_AC_OPTION_INLINE()
SIM_AC_OPTION_ALIGNMENT(NONSTRICT_ALIGNMENT)
SIM_AC_OPTION_HOSTENDIAN
SIM_AC_OPTION_WARNINGS
SIM_AC_OPTION_RESERVED_BITS(1)

# DEPRECATED
#
# Instead of defining a `subtarget' macro, code should be checking
# the value of {STATE,CPU}_ARCHITECTURE to identify the architecture
# in question.
#
case "${target}" in
  mips64vr*-*-*)	SIM_SUBTARGET="-DTARGET_ENABLE_FR=1" ;;
  mips*tx39*)           SIM_SUBTARGET="-DSUBTARGET_R3900=1";;
  mips*-sde-elf*)       SIM_SUBTARGET="-DTARGET_ENABLE_FR=1";;
  mips*-mti-elf*)       SIM_SUBTARGET="-DTARGET_ENABLE_FR=1";;
  mips*-img-elf*)       SIM_SUBTARGET="-DTARGET_ENABLE_FR=1";;
  mipsisa32*-*-*)       SIM_SUBTARGET="-DTARGET_ENABLE_FR=1";;
  mipsisa64*-*-*)       SIM_SUBTARGET="-DTARGET_ENABLE_FR=1";;
  *)                    SIM_SUBTARGET="";;
esac
AC_SUBST(SIM_SUBTARGET)



#
# Select the byte order of the target
#
mips_endian=
default_endian=
case "${target}" in
  mips64el*-*-*)	mips_endian=LITTLE_ENDIAN ;;
  mips64vr*el-*-*)      default_endian=LITTLE_ENDIAN ;;
  mips64*-*-*)          default_endian=BIG_ENDIAN ;;
  mips16*-*-*)          default_endian=BIG_ENDIAN ;;
  mipsisa32*-*-*)       default_endian=BIG_ENDIAN ;;
  mipsisa64*-*-*)       default_endian=BIG_ENDIAN ;;
  mips*-*-*)            default_endian=BIG_ENDIAN ;;
  *)                    default_endian=BIG_ENDIAN ;;
esac
SIM_AC_OPTION_ENDIAN($mips_endian,$default_endian)



#
# Select the bitsize of the target
#
mips_addr_bitsize=
case "${target}" in
  mips*-sde-elf*)       mips_bitsize=64 ; mips_msb=63 ;;
  mips*-mti-elf*)       mips_bitsize=64 ; mips_msb=63 ;;
  mips*-img-elf*)       mips_bitsize=64 ; mips_msb=63 ;;
  mips64*-*-*)          mips_bitsize=64 ; mips_msb=63 ;;
  mips16*-*-*)          mips_bitsize=64 ; mips_msb=63 ;;
  mipsisa32*-*-*)       mips_bitsize=32 ; mips_msb=31 ;;
  mipsisa64*-*-*)       mips_bitsize=64 ; mips_msb=63 ;;
  mips*-*-*)            mips_bitsize=32 ; mips_msb=31 ;;
  *)                    mips_bitsize=64 ; mips_msb=63 ;;
esac
SIM_AC_OPTION_BITSIZE($mips_bitsize,$mips_msb,$mips_addr_bitsize)



#
# Select the floating hardware support of the target
#
mips_fpu=HARDWARE_FLOATING_POINT
mips_fpu_bitsize=
case "${target}" in
  mips*tx39*)           mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=32 ;;
  mips*-sde-elf*)       mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=64 ;;
  mips*-mti-elf*)       mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=64 ;;
  mips*-img-elf*)       mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=64 ;;
  mips64*-*-*)          mips_fpu=HARD_FLOATING_POINT ;;
  mips16*-*-*)          mips_fpu=HARD_FLOATING_POINT ;;
  mipsisa32*-*-*)       mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=64 ;;
  mipsisa64*-*-*)       mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=64 ;;
  mips*-*-*)            mips_fpu=HARD_FLOATING_POINT ; mips_fpu_bitsize=32 ;;
  *)                    mips_fpu=HARD_FLOATING_POINT ;;
esac
SIM_AC_OPTION_FLOAT($mips_fpu,$mips_fpu_bitsize)



#
# Select the level of SMP support
#
case "${target}" in
  *)                    mips_smp=0 ;;
esac
SIM_AC_OPTION_SMP($mips_smp)



#
# Select the IGEN architecture
#
sim_gen=IGEN
sim_igen_machine="-M mipsIV"
sim_m16_machine="-M mips16,mipsIII"
sim_igen_filter="32,64,f"
sim_m16_filter="16"
sim_mach_default="mips8000"

case "${target}" in
  mips*tx39*)		sim_gen=IGEN
			sim_igen_filter="32,f"
			sim_igen_machine="-M r3900"
			;;
  mips64vr43*-*-*)	sim_gen=IGEN
			sim_igen_machine="-M mipsIV"
			sim_mach_default="mips8000"
			;;
  mips64vr5*-*-*)	sim_gen=IGEN
			sim_igen_machine="-M vr5000"
			sim_mach_default="mips5000"
			;;
  mips64vr41*)		sim_gen=M16
			sim_igen_machine="-M vr4100"
			sim_m16_machine="-M vr4100"
			sim_igen_filter="32,64,f"
			sim_m16_filter="16"
			sim_mach_default="mips4100"
                        ;;
  mips64vr-*-* | mips64vrel-*-*)
			sim_gen=MULTI
			sim_multi_configs="\
			  vr4100:mipsIII,mips16,vr4100::32,64:mips4100,mips4111:\
			  vr4120:mipsIII,mips16,vr4120::32,64:mips4120:\
			  vr5000:mipsIV::32,64,f:mips4300,mips5000:\
			  vr5400:mipsIV,vr5400::32,64,f:mips5400:\
			  vr5500:mipsIV,vr5500::32,64,f:mips5500:"
			sim_multi_default=mips5000
			;;
  mips*-sde-elf* | mips*-mti-elf*)
			sim_gen=MULTI
			sim_multi_configs="\
			  micromips:micromips64,micromipsdsp:mips64r2,mips3d,mdmx,dsp,dsp2,smartmips:32,64,f:mipsisa64r2,mipsisa32r2:EF_MIPS_ARCH_ASE_MICROMIPS\
			  mips64r2:mips64r2,mips3d,mips16,mips16e,mdmx,dsp,dsp2,smartmips::32,64,f:mipsisa64r2:"
			sim_multi_default=mipsisa64r2
			;;
  mips*-img-elf*)
			sim_gen=MULTI
			sim_multi_configs="\
			  micror6sim:micromips64r6,micromipsdsp:mips64r6:32,64,f:mipsisa64r6,mipsisa32r6:EF_MIPS_ARCH_ASE_MICROMIPS
			  micror2sim:micromips64,micromipsdsp:mips64r2:32,64,f:mipsisa64r2,mipsisa32r2:EF_MIPS_ARCH_ASE_MICROMIPS
			  r2sim:mips64r2,mips16,mips16e,mdmx,dsp,dsp2,mips3d,smartmips::32,64,f:mipsisa64r2,mipsisa32r2,mipsisa64r5,mipsisa32r5:\
			  r6sim:mips64r6::32,64,f:mipsisa64r6,mipsisa32r6:"
			sim_multi_default=mipsisa64r6
			;;
  mips64*-*-*)		sim_igen_filter="32,64,f"
			sim_gen=IGEN
			;;
  mips16*-*-*)		sim_gen=M16
			sim_igen_filter="32,64,f"
			sim_m16_filter="16"
			;;
  mipsisa32r2*-*-*)	sim_gen=MULTI
			sim_multi_configs="\
			  micromips:micromips32,micromipsdsp::32,f:mipsisa32r2,EF_MIPS_ARCH_ASE_MICROMIPS\
			  mips32r2:mips32r2,mips3d,mips16,mips16e,mdmx,dsp,dsp2,smartmips::32,f:mipsisa32r2:"
			sim_multi_default=mipsisa32r2
			;;
  mipsisa32r6*-*-*)	sim_gen=M16
			sim_igen_machine="-M mips32r6,mips16,mips16e,mdmx,dsp,dsp2,smartmips"
			sim_m16_machine="-M mips16,mips16e,mips32r6"
			sim_igen_filter="32,f"
			sim_mach_default="mipsisa32r6"
			;;
  mipsisa32*-*-*)	sim_gen=M16
			sim_igen_machine="-M mips32,mips16,mips16e,smartmips"
			sim_m16_machine="-M mips16,mips16e,mips32"
			sim_igen_filter="32,f"
			sim_mach_default="mipsisa32"
			;;
  mipsisa64r2*-*-*)	sim_gen=M16
			sim_igen_machine="-M mips64r2,mips3d,mips16,mips16e,mdmx,dsp,dsp2"
			sim_m16_machine="-M mips16,mips16e,mips64r2"
			sim_igen_filter="32,64,f"
			sim_mach_default="mipsisa64r2"
			;;
  mipsisa64r6*-*-*)	sim_gen=M16
			sim_igen_machine="-M mips64r6,mips16,mips16e,mdmx,dsp,dsp2"
			sim_m16_machine="-M mips16,mips16e,mips64r6"
			sim_igen_filter="32,64,f"
			sim_mach_default="mipsisa64r6"
			;;
  mipsisa64sb1*-*-*)	sim_gen=IGEN
			sim_igen_machine="-M mips64,mips3d,sb1"
			sim_igen_filter="32,64,f"
			sim_mach_default="mips_sb1"
			;;
  mipsisa64*-*-*)	sim_gen=M16
			sim_igen_machine="-M mips64,mips3d,mips16,mips16e,mdmx"
			sim_m16_machine="-M mips16,mips16e,mips64"
			sim_igen_filter="32,64,f"
			sim_mach_default="mipsisa64"
			;;
  mips*lsi*)		sim_gen=M16
			sim_igen_machine="-M mipsIII,mips16"
			sim_m16_machine="-M mips16,mipsIII"
			sim_igen_filter="32,f"
			sim_m16_filter="16"
			sim_mach_default="mips4000"
			;;
  mips*-*-*)		sim_gen=IGEN
			sim_igen_filter="32,f"
			;;
esac

# The MULTI generator can combine several simulation engines into one.
# executable.  A configuration which uses the MULTI should set two
# variables: ${sim_multi_configs} and ${sim_multi_default}.
#
# ${sim_multi_configs} is the list of engines to build.  Each
# space-separated entry has the form NAME:MACHINE:SUBMACH:FILTER:BFDMACHS,
# where:
#
# - NAME is a C-compatible prefix for the engine,
# - MACHINE is a -M argument,
# - SUBMACH is the machine type used as a secondary (micromips targets),
# - FILTER is a -F argument, and
# - BFDMACHS is a single bfd_mach_ prefixed architecture that will be
#     returned if requested for this engine, and
# - EFLAGS is a comma-separated list of elf flags, is an && joined list of
#     flags required to be met for this engine to be chosen
#
# The order of the entries does matter, particularly if flags are involved,
# as the first matching entry will be the chosen engine.
#
# Each entry will have a separate simulation engine whose prefix is
# m32<NAME>.  If the machine list includes "mips16", there will also
# be a mips16 engine, prefix m16<NAME>.  The mips16 engine will be
# generated using the same machine list as the 32-bit version,
# but the filter will be "16" instead of FILTER.
#
# FPU Bitsize state is taken from the original definitions and so will be the
# same for all machines in a multisim.
#
# The simulator compares the elf header against EMACHS/EFLAGS to decide
# which engine to use. ${sim_multi_default} says which entry should be the
# default.
if test ${sim_gen} = MULTI; then

  # Simple sanity check.
  if test -z "${sim_multi_configs}" || test -z "${sim_multi_default}"; then
    AC_MSG_ERROR(Error in configure.in: MULTI simulator not set up correctly)
  fi

  # Start in a known state.
  rm -f multi-include.h multi-run.c
  sim_multi_flags=
  sim_multi_src=
  sim_multi_obj=multi-run.o
  sim_multi_igen_configs=
  sim_seen_default=no

  cat << __EOF__ > multi-run.c
/* Main entry point for MULTI simulators.
   Copyright (C) 2003-2015 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   --

   This file was generated by sim/mips/configure.  */

#include "sim-main.h"
#include "multi-include.h"
#include "elf-bfd.h"
#include "elf/mips.h"

#define SD sd
#define CPU cpu

void
sim_engine_run (SIM_DESC sd,
		int next_cpu_nr,
		int nr_cpus,
		int signal) /* ignore */
{
  unsigned long bfdmach;
  unsigned long e_flags = elf_elfheader (sd->base.prog_bfd)->e_flags;

  if (STATE_ARCHITECTURE(SD) == NULL)
    bfdmach = bfd_mach_${sim_multi_default};
  else
    bfdmach = STATE_ARCHITECTURE(SD)->mach;

__EOF__

  bfdlist=""
  micromips_configs=""
  for fc in ${sim_multi_configs}; do

    # Split up the entry.  ${c} contains MACH/SUBMACH elements.
    c=`echo ${fc} | sed 's/.*:\(.*\):\(.*\):.*:.*:.*/\1\2/'`
    name=`echo ${fc} | sed 's/:.*//'`
    machine=`echo ${fc} | sed 's/.*:\(.*\):.*:.*:.*:.*/\1/'`
    filter=`echo ${fc} | sed 's/.*:.*:.*:\(.*\):.*:.*/\1/'`
    bfdmachs=`echo ${fc} | sed 's/.*:.*:.*:.*:\(.*\):.*/\1/'`
    eflags=`echo ${fc} | sed 's/.*://'`

    # Build the following lists:
    #
    #   sim_multi_flags: all -M and -F flags used by the simulator
    #   sim_multi_src: all makefile-generated source files
    #   sim_multi_obj: the objects for ${sim_multi_src}
    #   sim_multi_igen_configs: igen configuration strings.
    #
    # Each entry in ${sim_multi_igen_configs} is a prefix (m32
    # or m16) followed by the NAME, MACHINE and FILTER part of
    # the ${sim_multi_configs} entry.
    sim_multi_flags="${sim_multi_flags} -F ${filter} -M ${machine}"

    # Check whether special handling is needed.
    case ${c} in
      *mips16*)
	# Run igen twice, once for normal mode and once for mips16.
	ws="m32 m16"

	# The top-level function for the mips16 simulator is
	# in a file m16${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	sim_multi_src="${sim_multi_src} m16${name}_run.c"
	sim_multi_obj="${sim_multi_obj} m16${name}_run.o"
	sim_multi_flags="${sim_multi_flags} -F 16"
	;;
      *micromips32*)
	# Run igen thrice, once for micromips32, once for micromips16,
        # and once for m32.
	ws="micromips_m32 micromips16 micromips32"
	micromips_configs="${micromips_configs} micromips32${fc}"

	# The top-level function for the micromips simulator is
	# in a file micromips${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	sim_multi_src="${sim_multi_src} micromips${name}_run.c"
	sim_multi_obj="${sim_multi_obj} micromips${name}_run.o"
	sim_multi_flags="${sim_multi_flags} -F 16,32"
	;;
      *micromips64*)
	# Run igen thrice, once for micromips64, once for micromips16,
        # and once for m64.
	ws="micromips_m64 micromips16 micromips64"
	micromips_configs="${micromips_configs} micromips64${fc}"

	# The top-level function for the micromips simulator is
	# in a file micromips${name}_run.c, generated by the
	# tmp-run-multi Makefile rule.
	sim_multi_src="${sim_multi_src} micromips${name}_run.c"
	sim_multi_obj="${sim_multi_obj} micromips${name}_run.o"
	sim_multi_flags="${sim_multi_flags} -F 16,32,64"
	;;
      *)
	ws=m32
	;;
    esac

    # Now add the list of igen-generated files to ${sim_multi_src}
    # and ${sim_multi_obj}.
    for w in ${ws}; do
      for base in engine icache idecode model semantics support; do
	sim_multi_src="${sim_multi_src} ${w}${name}_${base}.c"
	sim_multi_src="${sim_multi_src} ${w}${name}_${base}.h"
	sim_multi_obj="${sim_multi_obj} ${w}${name}_${base}.o"
      done
      sim_multi_igen_configs="${sim_multi_igen_configs} ${w}${fc}"
    done

    # Add an include for the engine.h file.  This file declares the
    # top-level foo_engine_run() function.
    echo "#include \"${w}${name}_engine.h\"" >> multi-include.h

    # Generate conditionals for machs and flags
    machcond=""
    for mach in `echo ${bfdmachs} | sed 's/,/ /g'`; do
      machcond="${machcond} bfdmach == bfd_mach_${mach} ||"
      bfdlist="${bfdlist} bfd_mach_${mach}"
      if test ${mach} = ${sim_multi_default}; then
	default_wname="${w}${name}"
	sim_seen_default=yes
      fi
    done
    machcond=`echo ${machcond} | sed 's/\(.*\) ||/\1/'`

    flagcond=""
    for flag in `echo ${eflags} | sed 's/,/ /g'`; do
      flagcond="${flagcond} (e_flags & ${flag}) &&"
    done
    flagcond=`echo ${flagcond} | sed 's/\(.*\) &&/\1/'`

    # Add if statements for this engine to sim_engine_run().
    if [[ "${eflags}" == "" ]]; then
      echo "  if (${machcond})" >> multi-run.c
    else
      echo "  if ((${machcond}) && (${flagcond}))" >> multi-run.c
    fi
    cat << __EOF__ >> multi-run.c
    {
      ${w}${name}_engine_run (sd, next_cpu_nr, nr_cpus, signal);
    }
  else
__EOF__
  done

  # Check whether we added a 'default:' label.
  if test ${sim_seen_default} = no; then
    AC_MSG_ERROR(Error in configure.in: \${sim_multi_configs} doesn't have an entry for \${sim_multi_default})
  fi

  # Add the else-clause, and finish the rest of the file
  cat << __EOF__ >> multi-run.c
    {
      ${default_wname}_engine_run (sd, next_cpu_nr, nr_cpus, signal);
    }
}

unsigned long
mips_mach_multi (SIM_DESC sd)
{
  if (STATE_ARCHITECTURE (sd) == NULL)
    return bfd_mach_${sim_multi_default};

  switch (STATE_ARCHITECTURE (SD)->mach)
    {
__EOF__

  # Add case statements for this engine to mips_mach_multi().
  for bfd in $(echo ${bfdlist} | tr ' ' '\n' | sort -nu); do
    echo "    case ${bfd}:" >> multi-run.c
  done

  cat << __EOF__ >> multi-run.c
      return (STATE_ARCHITECTURE (SD)->mach);
    default:
      return bfd_mach_${sim_multi_default};
    }
}

address_word
micromips_instruction_decode_multi (SIM_DESC sd,
				    sim_cpu* cpu,
				    address_word cia,
				    int instruction_size)
{
  unsigned long bfdmach;
  if (STATE_ARCHITECTURE(SD) == NULL)
    bfdmach = bfd_mach_${sim_multi_default};
  else
    bfdmach = STATE_ARCHITECTURE(SD)->mach;

  switch (bfdmach)
    {
__EOF__

  # Add a case statement for each micromips-enabled engine
  for fc in ${micromips_configs}; do
    machine=`echo ${fc} | sed 's/.*:\(.*\):.*:.*:.*:.*/\1/'`
    name=`echo ${fc} | sed 's/:.*//'`
    bfdmachs=`echo ${fc} | sed 's/.*:.*:.*:.*:\(.*\):.*/\1/'`
    case ${machine} in
      micromips*)
	for bfdmach in `echo ${bfdmachs} | sed 's/,/ /g'`; do
	  echo "      case bfd_mach_${bfdmach}:" >> multi-run.c
	done
	echo "	return ${name}_instruction_decode (sd, cpu, cia, instruction_size);" >> multi-run.c
	;;
      *)
	;;
    esac
  done

  cat << __EOF__ >> multi-run.c
      default:
	fprintf(stderr, "no valid micromips instruction decoder for this micromips engine\n");
	abort();
	break;
    }
}
__EOF__

  SIM_SUBTARGET="$SIM_SUBTARGET -DMIPS_MACH_MULTI"
else
  # For clean-extra
  sim_multi_src=doesnt-exist.c

  if test x"${sim_mach_default}" = x""; then
    AC_MSG_ERROR(Error in configure.in: \${sim_mach_default} not defined)
  fi
  SIM_SUBTARGET="$SIM_SUBTARGET -DMIPS_MACH_DEFAULT=bfd_mach_${sim_mach_default}"
fi
sim_igen_flags="-F ${sim_igen_filter} ${sim_igen_machine} ${sim_igen_smp}"
sim_m16_flags=" -F ${sim_m16_filter}  ${sim_m16_machine}  ${sim_igen_smp}"
sim_micromips16_flags=" -F ${sim_micromips16_filter}  ${sim_micromips16_machine}  ${sim_igen_smp}"
sim_micromips_flags=" -F ${sim_micromips_filter}  ${sim_micromips_machine}  ${sim_igen_smp}"
AC_SUBST(sim_igen_flags)
AC_SUBST(sim_m16_flags)
AC_SUBST(sim_micromips_flags)
AC_SUBST(sim_micromips16_flags)
AC_SUBST(sim_gen)
AC_SUBST(sim_multi_flags)
AC_SUBST(sim_multi_igen_configs)
AC_SUBST(sim_multi_src)
AC_SUBST(sim_multi_obj)


#
# Add simulated hardware devices
#
hw_enabled=no
case "${target}" in
  mips*tx39*)
	hw_enabled=yes
	hw_extra_devices="tx3904cpu tx3904irc tx3904tmr tx3904sio" 
	SIM_SUBTARGET="$SIM_SUBTARGET -DTARGET_TX3904=1"
	;;
  *)
	;;
esac
SIM_AC_OPTION_HARDWARE($hw_enabled,$hw_devices,$hw_extra_devices)
mips_extra_objs="$SIM_DV_SOCKSER_O"
AC_SUBST(mips_extra_objs)

if test "$sim_hw_p" = yes -a -z "$SIM_DV_SOCKSER_O"; then
	case " $sim_hw " in
	*" tx3904sio "*)
		AC_MSG_ERROR([Sorry, but tx3904sio hardware support is
unavailable for your target.  Please use --disable-sim-hardware, or pass a
list of devices to enable that does not include that.])
	esac
fi


# Choose simulator engine
case "${target}" in
  *)    mips_igen_engine="engine.o"
	;;
esac
AC_SUBST(mips_igen_engine)


AC_PATH_X
mips_extra_libs=""
AC_SUBST(mips_extra_libs)

AC_CHECK_HEADERS(string.h strings.h stdlib.h stdlib.h)
AC_CHECK_LIB(m, fabs)
AC_CHECK_FUNCS(aint anint sqrt)

SIM_AC_OUTPUT
