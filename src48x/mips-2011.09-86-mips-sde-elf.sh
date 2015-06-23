#! /usr/bin/env bash
# This file contains the complete sequence of commands
# Mentor Graphics used to build this version of Sourcery CodeBench.
# 
# For each free or open-source component of Sourcery CodeBench,
# the source code provided includes all of the configuration
# scripts and makefiles for that component, including any and
# all modifications made by Mentor Graphics.  From this list of
# commands, you can see every configuration option used by
# Mentor Graphics during the build process.
# 
# This file is provided as a guideline for users who wish to
# modify and rebuild a free or open-source component of
# Sourcery CodeBench from source. For a number of reasons,
# though, you may not be able to successfully run this script
# directly on your system. Certain aspects of the Mentor Graphics
# build environment (such as directory names) are included in
# these commands. Mentor Graphics uses Canadian cross compilers so
# you may need to modify various configuration options and paths
# if you are building natively. This sequence of commands
# includes those used to build proprietary components of
# Sourcery CodeBench for which source code is not provided.
# 
# Please note that Sourcery CodeBench support covers only your
# use of the original, validated binaries provided as part of
# Sourcery CodeBench -- and specifically does not cover either
# the process of rebuilding a component or the use of any
# binaries you may build.  In addition, if you rebuild any
# component, you must not use the --with-pkgversion and
# --with-bugurl configuration options that embed Mentor Graphics
# trademarks in the resulting binary; see the "Mentor Graphics
# Trademarks" section in the Sourcery CodeBench Software
# License Agreement.
set -e
inform_fd=2 
umask 022
exec < /dev/null

error_handler ()
{
    exit 1
}

check_status() {
    local status="$?"
    if [ "$status" -ne 0 ]; then
	error_handler
    fi
}

check_pipe() {
    local -a status=("${PIPESTATUS[@]}")
    local limit=$1
    local ix
    
    if [ -z "$limit" ] ; then
	limit="${#status[@]}"
    fi
    for ((ix=0; ix != $limit ; ix++)); do
	if [ "${status[$ix]}" != "0" ] ; then
	    error_handler
	fi
    done
}

error () {
    echo "$script: error: $@" >& $inform_fd
    exit 1
}

warning () {
    echo "$script: warning: $@" >& $inform_fd
}

verbose () {
    if $gnu_verbose; then
	echo "$script: $@" >& $inform_fd
    fi
}

copy_dir() {
    mkdir -p "$2"

    (cd "$1" && tar cf - .) | (cd "$2" && tar xf -)
    check_pipe
}

copy_dir_clean() {
    mkdir -p "$2"
    (cd "$1" && tar cf - \
	--exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc \
	--exclude="*~" --exclude=".#*" \
	--exclude="*.orig" --exclude="*.rej" \
	.) | (cd "$2" && tar xf -)
    check_pipe
}

update_dir_clean() {
    mkdir -p "$2"


    (cd "$1" && tar cf - \
	--exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc \
	--exclude="*~" --exclude=".#*" \
	--exclude="*.orig" --exclude="*.rej" \
	--after-date="$3" \
	. 2> /dev/null) | (cd "$2" && tar xf -)
    check_pipe
}

copy_dir_exclude() {
    local source="$1"
    local dest="$2"
    local excl="$3"
    shift 3
    mkdir -p "$dest"
    (cd "$source" && tar cfX - "$excl" "$@") | (cd "$dest" && tar xf -)
    check_pipe
}

copy_dir_only() {
    local source="$1"
    local dest="$2"
    shift 2
    mkdir -p "$dest"
    (cd "$source" && tar cf - "$@") | (cd "$dest" && tar xf -)
    check_pipe
}

clean_environment() {
    local env_var_list
    local var




    unset BASH_ENV CDPATH POSIXLY_CORRECT TMOUT

    env_var_list=$(export | \
	grep '^declare -x ' | \
	sed -e 's/^declare -x //' -e 's/=.*//')

    for var in $env_var_list; do
	case $var in
	    HOME|HOSTNAME|LOGNAME|PWD|SHELL|SHLVL|SSH_*|TERM|USER)


		;;
	    LD_LIBRARY_PATH|PATH| \
		FLEXLM_NO_CKOUT_INSTALL_LIC|LM_APP_DISABLE_CACHE_READ)


		;;
	    MAKEINFO)

		;;
	    *_LICENSE_FILE)












		if [ "" ]; then
		    local license_file_envvar
		    license_file_envvar=

		    if [ "$var" != "$license_file_envvar" ]; then
			export -n "$var" || true
		    fi
		else
		    export -n "$var" || true
		fi
		;;
	    *)

		export -n "$var" || true
		;;
	esac
    done


    export LANG=C
    export LC_ALL=C


    export CVS_RSH=ssh



    user_shell=$SHELL
    export SHELL=$BASH
    export CONFIG_SHELL=$BASH
}

pushenv() {
    pushenv_level=$(($pushenv_level + 1))
    eval pushenv_vars_${pushenv_level}=
}


pushenv_level=0
pushenv_vars_0=



pushenvvar() {
    local pushenv_var="$1"
    local pushenv_newval="$2"
    eval local pushenv_oldval=\"\$$pushenv_var\"
    eval local pushenv_oldset=\"\${$pushenv_var+set}\"
    local pushenv_save_var=saved_${pushenv_level}_${pushenv_var}
    local pushenv_savep_var=savedp_${pushenv_level}_${pushenv_var}
    eval local pushenv_save_set=\"\${$pushenv_savep_var+set}\"
    if [ "$pushenv_save_set" = "set" ]; then
	error "Pushing $pushenv_var more than once at level $pushenv_level"
    fi
    if [ "$pushenv_oldset" = "set" ]; then
	eval $pushenv_save_var=\"\$pushenv_oldval\"
    else
	unset $pushenv_save_var
    fi
    eval $pushenv_savep_var=1
    eval export $pushenv_var=\"\$pushenv_newval\"
    local pushenv_list_var=pushenv_vars_${pushenv_level}
    eval $pushenv_list_var=\"\$$pushenv_list_var \$pushenv_var\"
}

prependenvvar() {
    local pushenv_var="$1"
    local pushenv_newval="$2"
    eval local pushenv_oldval=\"\$$pushenv_var\"
    pushenvvar "$pushenv_var" "$pushenv_newval$pushenv_oldval"
}

popenv() {
    local pushenv_var=
    eval local pushenv_vars=\"\$pushenv_vars_${pushenv_level}\"
    for pushenv_var in $pushenv_vars; do
	local pushenv_save_var=saved_${pushenv_level}_${pushenv_var}
	local pushenv_savep_var=savedp_${pushenv_level}_${pushenv_var}
	eval local pushenv_save_val=\"\$$pushenv_save_var\"
	eval local pushenv_save_set=\"\${$pushenv_save_var+set}\"
	unset $pushenv_save_var
	unset $pushenv_savep_var
	if [ "$pushenv_save_set" = "set" ]; then
	    eval export $pushenv_var=\"\$pushenv_save_val\"
	else
	    unset $pushenv_var
	fi
    done
    unset pushenv_vars_${pushenv_level}
    if [ "$pushenv_level" = "0" ]; then
	error "Popping environment level 0"
    else
	pushenv_level=$(($pushenv_level - 1))
    fi
}

prepend_path() {
    if $(eval "test -n \"\$$1\""); then
	prependenvvar "$1" "$2:"
    else
	prependenvvar "$1" "$2"
    fi
}
pushenvvar CSL_SCRIPTDIR /scratch/cmoore/elf-64bit-lite/src/scripts-trunk
pushenvvar PATH /usr/local/tools/gcc-4.3.3/bin
pushenvvar LD_LIBRARY_PATH /usr/local/tools/gcc-4.3.3/i686-pc-linux-gnu/lib:/usr/local/tools/gcc-4.3.3/lib64:/usr/local/tools/gcc-4.3.3/lib
pushenvvar FLEXLM_NO_CKOUT_INSTALL_LIC 1
pushenvvar LM_APP_DISABLE_CACHE_READ 1
pushenvvar MAKEINFO 'makeinfo --css-ref=../cs.css'
clean_environment
# task [001/197] /init/dirs
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
mkdir -p /scratch/cmoore/elf-64bit-lite/obj
mkdir -p /scratch/cmoore/elf-64bit-lite/install
mkdir -p /scratch/cmoore/elf-64bit-lite/pkg
mkdir -p /scratch/cmoore/elf-64bit-lite/logs/data
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf
popenv

# task [002/197] /init/cleanup
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf.src.tar.bz2 /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf.backup.tar.bz2
popenv

# task [003/197] /init/source_package/binutils
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/binutils-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/binutils-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' binutils-2011.09
popd
popenv

# task [004/197] /init/source_package/gcc
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gcc-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gcc-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' gcc-4.5-2011.09
popd
popenv

# task [005/197] /init/source_package/gdb
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gdb-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gdb-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' gdb-2011.09
popd
popenv

# task [006/197] /init/source_package/zlib
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/zlib-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/zlib-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' zlib-1.2.3
popd
popenv

# task [007/197] /init/source_package/gmp
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gmp-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/gmp-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' gmp-2011.09
popd
popenv

# task [008/197] /init/source_package/mpfr
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/mpfr-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/mpfr-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' mpfr-2011.09
popd
popenv

# task [009/197] /init/source_package/mpc
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/mpc-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/mpc-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' mpc-0.9
popd
popenv

# task [010/197] /init/source_package/cloog
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/cloog-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/cloog-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' cloog-0.15
popd
popenv

# task [011/197] /init/source_package/ppl
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/ppl-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/ppl-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' ppl-0.10.2
popd
popenv

# task [012/197] /init/source_package/getting_started
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/getting_started-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/getting_started-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' getting-started-2011.09
popd
popenv

# task [013/197] /init/source_package/installanywhere
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/installanywhere-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/installanywhere-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' installanywhere-trunk
popd
popenv

# task [014/197] /init/source_package/fbench
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/fbench-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/fbench-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' fbench-trunk
popd
popenv

# task [015/197] /init/source_package/mips_sprite
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/mips_sprite-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/mips_sprite-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' mips_sprite-2011.09
popd
popenv

# task [016/197] /init/source_package/eembc
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/eembc-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/eembc-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' eembc-trunk
popd
popenv

# task [017/197] /init/source_package/dhrystone
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/dhrystone-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/dhrystone-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' dhrystone-trunk
popd
popenv

# task [018/197] /init/source_package/newlib
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/newlib-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/newlib-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' newlib-2011.09
popd
popenv

# task [019/197] /init/source_package/cs3
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/cs3-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/cs3-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' cs3-2011.09
popd
popenv

# task [020/197] /init/source_package/board_support
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/board_support-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/board_support-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' board-support-trunk
popd
popenv

# task [021/197] /init/source_package/csl_tests
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/csl_tests-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/csl_tests-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' csl-tests-trunk
popd
popenv

# task [022/197] /init/source_package/dejagnu_boards
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/dejagnu_boards-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/dejagnu_boards-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' dejagnu-boards-trunk
popd
popenv

# task [023/197] /init/source_package/scripts
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/scripts-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/scripts-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' scripts-trunk
popd
popenv

# task [024/197] /init/source_package/xfails
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/xfails-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/xfails-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' xfails-trunk
popd
popenv

# task [025/197] /init/source_package/portal
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/portal-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/portal-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' portal-trunk
popd
popenv

# task [026/197] /init/source_package/libiconv
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/libiconv-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/libiconv-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' libiconv-1.11
popd
popenv

# task [027/197] /init/source_package/libelf
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/libelf-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/libelf-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' libelf-2011.09
popd
popenv

# task [028/197] /init/source_package/expat
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/expat-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/expat-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' expat-mirror
popd
popenv

# task [029/197] /init/source_package/csl_docbook
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/csl_docbook-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/csl_docbook-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' csl-docbook-trunk
popd
popenv

# task [030/197] /init/source_package/make
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/make-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/make-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' make-3.81
popd
popenv

# task [031/197] /init/source_package/coreutils
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/coreutils-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf/coreutils-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' coreutils-5.94
popd
popenv

# task [032/197] /init/source_package/gdil
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
rm -f /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/gdil-2011.09-86.tar.bz2
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/src
tar cf /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup/gdil-2011.09-86.tar.bz2 --bzip2 --owner=0 --group=0 --exclude=CVS --exclude=.svn --exclude=.git --exclude=.pc '--exclude=*~' '--exclude=.#*' '--exclude=*.orig' '--exclude=*.rej' gdil-2011.09
popd
popenv

# task [033/197] /i686-pc-linux-gnu/host_cleanup
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
popenv

# task [034/197] /i686-pc-linux-gnu/zlib_first/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
rm -rf /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/zlib-1.2.3 /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
popenv

# task [035/197] /i686-pc-linux-gnu/zlib_first/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushenv
pushenvvar CFLAGS '-O3 -fPIC'
pushenvvar CC 'i686-pc-linux-gnu-gcc '
pushenvvar AR 'i686-pc-linux-gnu-ar rc'
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
./configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr
popenv
popd
popenv

# task [036/197] /i686-pc-linux-gnu/zlib_first/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv

# task [037/197] /i686-pc-linux-gnu/zlib_first/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv

# task [038/197] /i686-pc-linux-gnu/gmp/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/gmp-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=i686-pc-linux-gnu --host=i686-pc-linux-gnu --enable-cxx --disable-nls
popd
popenv
popenv
popenv

# task [039/197] /i686-pc-linux-gnu/gmp/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [040/197] /i686-pc-linux-gnu/gmp/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [041/197] /i686-pc-linux-gnu/gmp/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make check
popd
popenv
popenv
popenv

# task [042/197] /i686-pc-linux-gnu/mpfr/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/mpfr-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr
popd
popenv
popenv
popenv

# task [043/197] /i686-pc-linux-gnu/mpfr/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [044/197] /i686-pc-linux-gnu/mpfr/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [045/197] /i686-pc-linux-gnu/mpfr/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make check
popd
popenv
popenv
popenv

# task [046/197] /i686-pc-linux-gnu/mpc/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/mpc-0.9/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-mpfr=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr
popd
popenv
popenv
popenv

# task [047/197] /i686-pc-linux-gnu/mpc/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [048/197] /i686-pc-linux-gnu/mpc/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [049/197] /i686-pc-linux-gnu/mpc/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make check
popd
popenv
popenv
popenv

# task [050/197] /i686-pc-linux-gnu/ppl/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/ppl-0.10.2/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls --with-libgmp-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-gmp-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr CPPFLAGS=-I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include LDFLAGS=-L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib --disable-watchdog
popd
popenv
popenv
popenv

# task [051/197] /i686-pc-linux-gnu/ppl/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [052/197] /i686-pc-linux-gnu/ppl/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [053/197] /i686-pc-linux-gnu/cloog/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/cloog-0.15/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls --with-ppl=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr
popd
popenv
popenv
popenv

# task [054/197] /i686-pc-linux-gnu/cloog/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [055/197] /i686-pc-linux-gnu/cloog/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [056/197] /i686-pc-linux-gnu/cloog/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make check
popd
popenv
popenv
popenv

# task [057/197] /i686-pc-linux-gnu/libelf/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/libelf-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls
popd
popenv
popenv
popenv

# task [058/197] /i686-pc-linux-gnu/libelf/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [059/197] /i686-pc-linux-gnu/libelf/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [060/197] /i686-pc-linux-gnu/toolchain/binutils/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09 /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
touch /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/.gnu-stamp
popenv
popenv
popenv

# task [061/197] /i686-pc-linux-gnu/toolchain/binutils/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --with-sysroot=/opt/codesourcery/mips-sde-elf --enable-poison-system-directories
popd
popenv
popenv
popenv

# task [062/197] /i686-pc-linux-gnu/toolchain/binutils/libiberty
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4 all-libiberty
popd
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09/include /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/libiberty/libiberty.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
popenv
popenv
popenv

# task [063/197] /i686-pc-linux-gnu/toolchain/binutils/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [064/197] /i686-pc-linux-gnu/toolchain/binutils/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share
popd
popenv
popenv
popenv

# task [065/197] /i686-pc-linux-gnu/toolchain/binutils/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/install
rm ./lib/libiberty.a
rmdir ./lib
popd
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share install-html
popd
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share install-pdf
popd
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/bfd/.libs/libbfd.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/bfd/bfd.h /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
cp /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09/bfd/elf-bfd.h /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/opcodes/.libs/libopcodes.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/configure.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc/configure.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/configure.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly configure
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/standards.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc/standards.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/standards.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly standards
rmdir /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/bfd.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/bfd.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/bfd.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-ld.bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/bin/ld.bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/ld.bfd
popenv
popenv
popenv

# task [066/197] /i686-pc-linux-gnu/toolchain/gcc_first/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gcc-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gcc-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/gcc-4.5-2011.09/configure --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu --target=mips-sde-elf --enable-threads --disable-libmudflap --disable-libssp --disable-libstdcxx-pch --with-arch-32=mips32r2 --with-arch-64=mips64r2 --enable-sgxx-sde-multilibs --disable-threads --with-gnu-as --with-gnu-ld '--with-specs=%{save-temps: -fverbose-asm} -D__CS_SOURCERYGXX_MAJ__=2011 -D__CS_SOURCERYGXX_MIN__=9 -D__CS_SOURCERYGXX_REV__=86 %{O2:%{!fno-remove-local-statics: -fremove-local-statics}} %{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}}' --enable-languages=c,c++ --disable-shared --enable-lto --with-newlib '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --prefix=/opt/codesourcery --disable-shared --disable-threads --disable-libssp --disable-libgomp --without-headers --with-newlib --disable-decimal-float --disable-libffi --disable-libquadmath --disable-libitm --enable-languages=c --with-sysroot=/opt/codesourcery/mips-sde-elf --with-build-sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-mpfr=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-mpc=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-ppl=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' --with-cloog=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-libelf=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-libgomp --enable-poison-system-directories --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin
popd
popenv
popenv

# task [067/197] /i686-pc-linux-gnu/toolchain/gcc_first/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4 LDFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf CPPFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf build_tooldir=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf
popd
popenv
popenv

# task [068/197] /i686-pc-linux-gnu/toolchain/gcc_first/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-first-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install-html
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install-pdf
popd
popenv
popenv

# task [069/197] /i686-pc-linux-gnu/toolchain/gcc_first/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushd /scratch/cmoore/elf-64bit-lite/install
rm bin/mips-sde-elf-gccbug
rm ./lib/libiberty.a
rmdir include
popd
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/gccinstall /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/gccinstall.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/gccinstall.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly gccinstall
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/gccint /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/gccint.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/gccint.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly gccint
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/cppinternals /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/cppinternals.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/cppinternals.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly cppinternals
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
popenv
popenv

# task [070/197] /i686-pc-linux-gnu/toolchain/newlib/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS_FOR_TARGET '-g -O2'
rm -rf /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/newlib-2011.09/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --enable-newlib-io-long-long --enable-newlib-register-fini --disable-newlib-multithread --disable-libgloss --disable-newlib-supplied-syscalls --disable-nls
popd
popenv
popenv
popenv

# task [071/197] /i686-pc-linux-gnu/toolchain/newlib/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS_FOR_TARGET '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [072/197] /i686-pc-linux-gnu/toolchain/newlib/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS_FOR_TARGET '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share
popd
popenv
popenv
popenv

# task [073/197] /i686-pc-linux-gnu/toolchain/newlib/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CFLAGS_FOR_TARGET '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make pdf
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf
cp /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/mips-sde-elf/newlib/libc/libc.pdf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libc.pdf
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf
cp /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/mips-sde-elf/newlib/libm/libm.pdf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libm.pdf
make html
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html
copy_dir /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/mips-sde-elf/newlib/libc/libc.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libc
mkdir -p /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html
copy_dir /scratch/cmoore/elf-64bit-lite/obj/newlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/mips-sde-elf/newlib/libm/libm.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libm
popd
popenv
popenv
popenv

# task [074/197] /i686-pc-linux-gnu/toolchain/gcc_final/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/gcc-4.5-2011.09/configure --build=i686-pc-linux-gnu --host=i686-pc-linux-gnu --target=mips-sde-elf --enable-threads --disable-libmudflap --disable-libssp --disable-libstdcxx-pch --with-arch-32=mips32r2 --with-arch-64=mips64r2 --enable-sgxx-sde-multilibs --disable-threads --with-gnu-as --with-gnu-ld '--with-specs=%{save-temps: -fverbose-asm} -D__CS_SOURCERYGXX_MAJ__=2011 -D__CS_SOURCERYGXX_MIN__=9 -D__CS_SOURCERYGXX_REV__=86 %{O2:%{!fno-remove-local-statics: -fremove-local-statics}} %{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}}' --enable-languages=c,c++ --disable-shared --enable-lto --with-newlib '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --prefix=/opt/codesourcery --with-headers=yes --with-sysroot=/opt/codesourcery/mips-sde-elf --with-build-sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-mpfr=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-mpc=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-ppl=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' --with-cloog=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-libelf=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-libgomp --enable-poison-system-directories --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin
popd
popenv
popenv

# task [075/197] /i686-pc-linux-gnu/toolchain/gcc_final/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4 LDFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf CPPFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf build_tooldir=/scratch/cmoore/elf-64bit-lite/install/mips-sde-elf
popd
popenv
popenv

# task [076/197] /i686-pc-linux-gnu/toolchain/gcc_final/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install-html
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man install-pdf
popd
popenv
popenv

# task [077/197] /i686-pc-linux-gnu/toolchain/gcc_final/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushd /scratch/cmoore/elf-64bit-lite/install
rm bin/mips-sde-elf-gccbug
rm ./lib/libiberty.a
rmdir include
popd
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/usr
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/gccinstall /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/gccinstall.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/gccinstall.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly gccinstall
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/gccint /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/gccint.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/gccint.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly gccint
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/cppinternals /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gcc/cppinternals.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/cppinternals.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly cppinternals
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libstdc++.a-gdb.py
popenv
popenv

# task [078/197] /i686-pc-linux-gnu/toolchain/zlib/0/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
rm -rf /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/zlib-1.2.3 /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
popenv

# task [079/197] /i686-pc-linux-gnu/toolchain/zlib/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushenv
pushenvvar CFLAGS '-O3 -fPIC'
pushenvvar CC 'i686-pc-linux-gnu-gcc '
pushenvvar AR 'i686-pc-linux-gnu-ar rc'
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
./configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr
popenv
popd
popenv

# task [080/197] /i686-pc-linux-gnu/toolchain/zlib/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv

# task [081/197] /i686-pc-linux-gnu/toolchain/zlib/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv

# task [082/197] /i686-pc-linux-gnu/toolchain/expat/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/expat-mirror/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls
popd
popenv
popenv
popenv

# task [083/197] /i686-pc-linux-gnu/toolchain/expat/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [084/197] /i686-pc-linux-gnu/toolchain/expat/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [085/197] /i686-pc-linux-gnu/toolchain/gdb/0/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/gdb-2011.09 /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
touch /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/.gnu-stamp
popenv
popenv
popenv

# task [086/197] /i686-pc-linux-gnu/toolchain/gdb/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --with-libexpat-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-system-gdbinit=/opt/codesourcery/i686-pc-linux-gnu/mips-sde-elf/lib/gdbinit '--with-gdb-datadir='\''${prefix}'\''/mips-sde-elf/share/gdb'
popd
popenv
popenv
popenv

# task [087/197] /i686-pc-linux-gnu/toolchain/gdb/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [088/197] /i686-pc-linux-gnu/toolchain/gdb/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share
popd
popenv
popenv
popenv

# task [089/197] /i686-pc-linux-gnu/toolchain/gdb/0/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share install-html
make prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share install-pdf
popd
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/annotate /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/annotate.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/annotate.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly annotate
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/gdbint /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/gdbint.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/gdbint.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly gdbint
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/stabs /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/stabs.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/stabs.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly stabs
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/configure.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc/configure.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/configure.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly configure
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/standards.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc/standards.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/standards.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly standards
rmdir /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/etc
rm -rf /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/bfd.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/bfd.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info/bfd.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info --remove-exactly bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
popenv
popenv
popenv

# task [090/197] /i686-pc-linux-gnu/toolchain/cs3/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar CC mips-sde-elf-gcc
pushenvvar CXX mips-sde-elf-g++
pushenvvar AR mips-sde-elf-ar
pushenvvar RANLIB mips-sde-elf-ranlib
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/cs3-2011.09/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=mips-sde-elf '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --with-intermediate-dir=/scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf '--with-configs=public-*'
popd
popenv
popenv
popenv

# task [091/197] /i686-pc-linux-gnu/toolchain/cs3/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar CC mips-sde-elf-gcc
pushenvvar CXX mips-sde-elf-g++
pushenvvar AR mips-sde-elf-ar
pushenvvar RANLIB mips-sde-elf-ranlib
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [092/197] /i686-pc-linux-gnu/toolchain/cs3/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenvvar CC mips-sde-elf-gcc
pushenvvar CXX mips-sde-elf-g++
pushenvvar AR mips-sde-elf-ar
pushenvvar RANLIB mips-sde-elf-ranlib
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share
popd
popenv
popenv
popenv

# task [093/197] /i686-pc-linux-gnu/getting_started/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/getting-started-2011.09 /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
touch /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/.gnu-stamp
cat > /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/2011.09-mips-sde-elf-lite-libs.xml <<'EOF0'
<!DOCTYPE getting-started SYSTEM "getting-started.dtd">
<!-- automatically generated, do not edit -->
<section id="sec-multilibs">
  <title>Library Configurations</title>
  <para>
    &csl_prod; for &csl_target_name; includes the following library
    configuration.
  </para>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Big-Endian, O32</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry>default</entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>./</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Big-Endian, O32, mips16</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-mips16</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>mips16/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Soft-Float, O32</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Big-Endian, O32, mips16, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-mips16 -msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>mips16/sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32, mips16</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -mips16</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/mips16/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32, mips16, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -mips16 -msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/mips16/sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Big-Endian, O32, micromips</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-mmicromips</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>micromips/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Big-Endian, O32, micromips, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-mmicromips -msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>micromips/sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32, micromips</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -mmicromips</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/micromips/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS32 revision 2 - Little-Endian, O32, micromips, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -mmicromips -msoft-float</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/micromips/sof/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS64 revision 2 - Big Endian, N64</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-mabi=64</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>64/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS64 revision 2 - Big Endian, N64, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-msoft-float -mabi=64</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>sof/64/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS64 revision 2 - Little Endian, N64</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -mabi=64</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/64/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <informaltable>
    <?dbfo keep-together="always" ?>
    <?dbfo table-width="100%" ?>
    <?dbhtml table-width="100%" ?>
    <tgroup cols="2" align="left">
      <colspec colname="c1" colwidth="1*" />
      <colspec colname="c2" colwidth="2*" />
      <thead>
	<row>
	  <entry namest="c1" nameend="c2">MIPS64 revision 2 - Little Endian, N64, Soft-Float</entry>
	</row>
      </thead>
      <tbody>
	<row>
	  <entry>Command-line option(s):</entry>
	  <entry><option>-EL -msoft-float -mabi=64</option></entry>
	</row>
	<row>
	  <entry>Library subdirectory:</entry>
	  <entry><filename>el/sof/64/</filename></entry>
	</row>
      </tbody>
    </tgroup>
  </informaltable>
  <xi:include href="library-intro.xml" xpointer="xpointer(*/*)"/>
</section>
EOF0
cat > /scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/2011.09-mips-sde-elf-lite.dtd <<'EOF0'
<!-- automatically generated, do not edit -->
<!ENTITY csl_multilib_sec "<xi:include href='/scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/2011.09-mips-sde-elf-lite-libs.xml'/>">
<!ENTITY csl_subpackage_sec "">
<!ENTITY csl_binutils_component_license "&csl_gpl3.0_license;">
<!ENTITY csl_binutils_component_version "2.21.53">
<!ENTITY csl_gcc_component_license "&csl_gpl3.0_license;">
<!ENTITY csl_gcc_component_version "4.5.2">
<!ENTITY csl_gdb_component_license "&csl_gpl3.0_license;">
<!ENTITY csl_gdb_component_version "7.2.50">
<!ENTITY csl_mips_sprite_component_license "&csl_cs_license;">
<!ENTITY csl_newlib_component_license "&csl_newlib_license;">
<!ENTITY csl_newlib_component_version "1.18.0">
<!ENTITY csl_cs3_component_license "&csl_cs_license;">
<!ENTITY csl_make_component_license "&csl_gpl2.0_license;">
<!ENTITY csl_coreutils_component_license "&csl_gpl2.0_license;">
EOF0
popenv
popenv
popenv

# task [094/197] /i686-pc-linux-gnu/getting_started/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/obj/getting_started-src-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --disable-nls --with-release-config=2011.09-mips-sde-elf-lite '--with-components=binutils gcc gdb zlib gmp mpfr mpc cloog ppl getting_started installanywhere fbench mips_sprite eembc dhrystone newlib cs3 board_support branding contents comparison csl_tests dejagnu_boards scripts xfails portal harness libiconv libelf expat csl_docbook make coreutils gdil no-alf no-alp_examples no-android no-apm_examples no-argp no-arm_stub no-atlas_install no-bluecoat_http_proxy no-c6x_sprite no-ccs no-ccs_mdi_library no-cctools no-cfpe_stub no-cgen no-cml no-codewarrior_plugin no-com_codesourcery_util no-compiler_wrapper no-coremark no-csldoc no-cslibc_manual no-cula_install no-cxxabi no-cygwin_wrapper no-dbgrtool no-dinkum no-dtc no-eclipse no-eclipse_cdt no-eclipse_patches no-eclipse_plugin no-eclipse_zip no-edge_utils no-eglibc no-eglibc_configs no-elf2flt no-elfutils no-flexlm_utils no-glibc no-glibc_cs_opt no-glibc_linuxthreads no-glibc_localedef no-glibc_ports no-harness_test no-ide no-intel_sprite no-kinetis_examples no-libcsftdi no-libevent no-libfdt no-libftdi no-libkcompat no-libosip2 no-libpng no-libsdl no-libsdl_macosx no-libunwind no-liburcu no-libusb no-libusb_win32 no-license no-license_lib no-linux no-lsb no-mep_cs no-mepl no-mesp_docs no-mgls no-mingw no-mips_toolchain_manual no-mklibs no-mpatrol no-mqx no-mqx_support no-msvcrt no-ncurses no-nucleus_binary_demos no-ocdremote no-omf2elf no-openposix no-oprofile no-oprofile_plugin no-osawareness_plugin no-osprey no-ph no-plex_skel no-popt no-power_sprite no-prelink no-python no-python_macosx no-python_win32 no-qemu no-qmtest_ph no-redboot no-rpm no-sgxx_application_notes no-sgxx_demos no-simdmath no-sip_parser_benchmark no-spec2000 no-spec2000_configs no-spec2006 no-spec2xxx no-srpm no-stellarisware no-stm32_generate_sgxx no-stm32_stdperiph_lib no-stm32_usb_fs_device_lib no-svp_docs no-svxx_api_reference no-svxx_benchmarks no-svxx_binary no-svxx_documentation no-svxx_fftw no-svxx_source no-sysroot_utils no-tcl no-trace_processor no-uclibc no-uclibc_configs no-ust no-verifone_plugin no-w32api no-windows_script_wrapper no-xlp_mdi' '--with-features=sgxx_version gdbsimulator multilibs tarball public elf sprite mdimips newlib_newlib cs3' '--with-hosts=i686-pc-linux-gnu i686-mingw32' --with-target-arch-name=MIPS --with-target-os-name=ELF --with-intermediate-dir=/scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf --with-csl-docbook=/scratch/cmoore/elf-64bit-lite/src/csl-docbook-trunk --with-version-string=2011.09-86 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' '--with-brand=Sourcery CodeBench Lite' '--with-xml-catalog-files=/usr/local/tools/gcc-4.3.3/share/sgml/docbook/docbook-xsl/catalog.xml /etc/xml/catalog' --with-license=lite --with-cs3=/scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf/doc
popd
popenv
popenv
popenv

# task [095/197] /i686-pc-linux-gnu/getting_started/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [096/197] /i686-pc-linux-gnu/getting_started/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share docdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf
popd
popenv
popenv
popenv

# task [097/197] /i686-pc-linux-gnu/csl_docbook
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
cp /scratch/cmoore/elf-64bit-lite/src/csl-docbook-trunk/css/cs.css /scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html
popenv

# task [098/197] /i686-pc-linux-gnu/gdil/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/gdil-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu --with-csl-docbook=/scratch/cmoore/elf-64bit-lite/src/csl-docbook-trunk '--with-xml-catalog-files=/usr/local/tools/gcc-4.3.3/share/sgml/docbook/docbook-xsl/catalog.xml /etc/xml/catalog' --disable-nls --with-expat=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr '--with-features=xml autoconf driver config sprite doc'
popd
popenv
popenv
popenv

# task [099/197] /i686-pc-linux-gnu/gdil/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [100/197] /i686-pc-linux-gnu/gdil/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install
popd
popenv
popenv
popenv

# task [101/197] /i686-pc-linux-gnu/mips_sprite/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
/scratch/cmoore/elf-64bit-lite/src/mips_sprite-2011.09/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-pc-linux-gnu '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --enable-backends=mdimips --disable-nls --with-gdil=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-expat=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/usr --with-boards=/scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf/boards
popd
popenv
popenv
popenv

# task [102/197] /i686-pc-linux-gnu/mips_sprite/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make -j4
popd
popenv
popenv
popenv

# task [103/197] /i686-pc-linux-gnu/mips_sprite/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-pc-linux-gnu
make install prefix=/scratch/cmoore/elf-64bit-lite/install exec_prefix=/scratch/cmoore/elf-64bit-lite/install libdir=/scratch/cmoore/elf-64bit-lite/install/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/share
popd
popenv
popenv
popenv

# task [104/197] /i686-pc-linux-gnu/finalize_libc_installation
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
popenv

# task [105/197] /i686-pc-linux-gnu/pretidy_installation
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/install
rm ./lib/libiberty.a
popd
popenv

# task [106/197] /i686-pc-linux-gnu/remove_libtool_archives
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
find /scratch/cmoore/elf-64bit-lite/install -name '*.la' -exec rm '{}' ';'
popenv

# task [107/197] /i686-pc-linux-gnu/remove_copied_libs
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
popenv

# task [108/197] /i686-pc-linux-gnu/remove_fixed_headers
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
pushd /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/include-fixed
popd
popenv

# task [109/197] /i686-pc-linux-gnu/add_tooldir_readme
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
cat > /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/README.txt <<'EOF0'
The executables in this directory are for internal use by the compiler
and may not operate correctly when used directly.  This directory
should not be placed on your PATH.  Instead, you should use the
executables in ../../bin/ and place that directory on your PATH.
EOF0
popenv

# task [110/197] /i686-pc-linux-gnu/strip_host_objects
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-addr2line
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-ar
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-as
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-c++
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-c++filt
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-conv
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-cpp
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-elfedit
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-g++
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gcc
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gcc-4.5.2
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gcov
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gdb
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gdbtui
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-gprof
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-ld
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-nm
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-objcopy
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-objdump
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-ranlib
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-readelf
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-run
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-size
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-sprite
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-strings
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/bin/mips-sde-elf-strip
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/ar
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/as
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/c++
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/g++
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/gcc
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/ld
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/nm
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/objcopy
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/objdump
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/ranlib
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/bin/strip
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/cc1
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/collect2
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/install-tools/fixincl
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/cc1plus
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/lto-wrapper
/usr/local/tools/gcc-4.3.3/bin/i686-pc-linux-gnu-strip /scratch/cmoore/elf-64bit-lite/install/libexec/gcc/mips-sde-elf/4.5.2/lto1
popenv

# task [111/197] /i686-pc-linux-gnu/strip_target_objects
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/micromips/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/mips16/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/64/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/64/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/micromips/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/mips16/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/64/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3yamon.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libstdc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3hal.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libsupc++.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libg.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/64/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3-mips-fpemu.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/sof/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libm.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3unhosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3-mips-cp1.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/el/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/mips-sde-elf/lib/libcs3hosted.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/micromips/sof/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/mips16/sof/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/64/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/64/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/sof/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/micromips/sof/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/mips16/sof/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/64/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/libgcov.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/crti.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/crtend.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/crtn.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/crtbegin.o || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/64/libgcc.a || true
mips-sde-elf-objcopy -R .comment -R .note -R .debug_info -R .debug_aranges -R .debug_pubnames -R .debug_pubtypes -R .debug_abbrev -R .debug_line -R .debug_str -R .debug_ranges -R .debug_loc /scratch/cmoore/elf-64bit-lite/install/lib/gcc/mips-sde-elf/4.5.2/el/sof/libgcc.a || true
popenv

# task [112/197] /i686-pc-linux-gnu/package_tbz2
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
rm -f /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf-i686-pc-linux-gnu.tar.bz2
pushd /scratch/cmoore/elf-64bit-lite/obj
rm -f mips-2011.09
ln -s /scratch/cmoore/elf-64bit-lite/install mips-2011.09
tar cjf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf-i686-pc-linux-gnu.tar.bz2 --owner=0 --group=0 --exclude=host-i686-pc-linux-gnu --exclude=host-i686-mingw32 mips-2011.09/bin mips-2011.09/lib mips-2011.09/libexec mips-2011.09/mips-sde-elf mips-2011.09/share
rm -f mips-2011.09
popd
popenv

# task [113/197] /i686-pc-linux-gnu/package_installanywhere/unpack
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-unpack -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-pc-linux-gnu -f /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf/license.html sgxx/2011.09/mips-sde-elf-lite
popenv

# task [114/197] /i686-pc-linux-gnu/package_installanywhere/merge_modules
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper-mm -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-pc-linux-gnu -m iamm-common sgxx/2011.09/mips-sde-elf-lite
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper-mm -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-pc-linux-gnu -m iamm-jvm sgxx/2011.09/mips-sde-elf-lite
popenv

# task [115/197] /i686-pc-linux-gnu/package_installanywhere/installer
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-pc-linux-gnu-gcc
pushenvvar CXX i686-pc-linux-gnu-g++
pushenvvar AR i686-pc-linux-gnu-ar
pushenvvar RANLIB i686-pc-linux-gnu-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/install/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-pc-linux-gnu -f /scratch/cmoore/elf-64bit-lite/obj/ia-2011.09-86-mips-sde-elf-i686-pc-linux-gnu/license.html sgxx/2011.09/mips-sde-elf-lite
popenv

# task [116/197] /i686-mingw32/host_cleanup
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
popenv

# task [117/197] /i686-mingw32/host_unpack
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
rm -rf /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32
mkdir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32
ln -s . mips-2011.09
tar xf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf-i686-pc-linux-gnu.tar.bz2 --bzip2
rm mips-2011.09
popd
popenv

# task [118/197] /i686-mingw32/libiconv/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/libiconv-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/libiconv-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/libiconv-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/libiconv-1.11/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls
popd
popenv
popenv
popenv

# task [119/197] /i686-mingw32/libiconv/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libiconv-2011.09-86-mips-sde-elf-i686-mingw32
make -j1
popd
popenv
popenv
popenv

# task [120/197] /i686-mingw32/libiconv/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libiconv-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [121/197] /i686-mingw32/make/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/make-src-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/make-3.81 /scratch/cmoore/elf-64bit-lite/obj/make-src-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/make-src-2011.09-86-mips-sde-elf-i686-mingw32
touch /scratch/cmoore/elf-64bit-lite/obj/make-src-2011.09-86-mips-sde-elf-i686-mingw32/.gnu-stamp
popenv
popenv
popenv

# task [122/197] /i686-mingw32/make/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/make-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/make-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/make-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/obj/make-src-2011.09-86-mips-sde-elf-i686-mingw32/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --disable-nls --program-prefix=cs-
popd
popenv
popenv
popenv

# task [123/197] /i686-mingw32/make/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/make-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [124/197] /i686-mingw32/make/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/make-2011.09-86-mips-sde-elf-i686-mingw32
make install prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share
popd
popenv
popenv
popenv

# task [125/197] /i686-mingw32/coreutils/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/coreutils-src-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/coreutils-5.94 /scratch/cmoore/elf-64bit-lite/obj/coreutils-src-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/coreutils-src-2011.09-86-mips-sde-elf-i686-mingw32
touch /scratch/cmoore/elf-64bit-lite/obj/coreutils-src-2011.09-86-mips-sde-elf-i686-mingw32/.gnu-stamp
popenv
popenv
popenv

# task [126/197] /i686-mingw32/coreutils/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/coreutils-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/coreutils-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/coreutils-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/obj/coreutils-src-2011.09-86-mips-sde-elf-i686-mingw32/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --disable-nls --program-prefix=cs-
popd
popenv
popenv
popenv

# task [127/197] /i686-mingw32/coreutils/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/coreutils-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [128/197] /i686-mingw32/coreutils/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/coreutils-2011.09-86-mips-sde-elf-i686-mingw32
make install prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share
popd
popenv
popenv
popenv

# task [129/197] /i686-mingw32/zlib_first/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
rm -rf /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/zlib-1.2.3 /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
popenv

# task [130/197] /i686-mingw32/zlib_first/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
pushenv
pushenvvar CFLAGS '-O3 -fPIC'
pushenvvar CC 'i686-mingw32-gcc '
pushenvvar AR 'i686-mingw32-ar rc'
pushenvvar RANLIB i686-mingw32-ranlib
./configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr
popenv
popd
popenv

# task [131/197] /i686-mingw32/zlib_first/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv

# task [132/197] /i686-mingw32/zlib_first/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-first-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv

# task [133/197] /i686-mingw32/gmp/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/gmp-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=i686-mingw32 --host=i686-mingw32 --enable-cxx --disable-nls
popd
popenv
popenv
popenv

# task [134/197] /i686-mingw32/gmp/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [135/197] /i686-mingw32/gmp/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
pushd /scratch/cmoore/elf-64bit-lite/obj/gmp-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [136/197] /i686-mingw32/gmp/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CFLAGS '-g -O2'
popenv
popenv
popenv

# task [137/197] /i686-mingw32/mpfr/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/mpfr-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr
popd
popenv
popenv
popenv

# task [138/197] /i686-mingw32/mpfr/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [139/197] /i686-mingw32/mpfr/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpfr-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [140/197] /i686-mingw32/mpfr/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
popenv
popenv
popenv

# task [141/197] /i686-mingw32/mpc/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/mpc-0.9/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-mpfr=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr
popd
popenv
popenv
popenv

# task [142/197] /i686-mingw32/mpc/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [143/197] /i686-mingw32/mpc/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mpc-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [144/197] /i686-mingw32/mpc/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
popenv
popenv
popenv

# task [145/197] /i686-mingw32/ppl/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/ppl-0.10.2/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls --with-libgmp-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-gmp-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr CPPFLAGS=-I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include LDFLAGS=-L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib --disable-watchdog
popd
popenv
popenv
popenv

# task [146/197] /i686-mingw32/ppl/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [147/197] /i686-mingw32/ppl/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/ppl-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [148/197] /i686-mingw32/cloog/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/cloog-0.15/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls --with-ppl=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr
popd
popenv
popenv
popenv

# task [149/197] /i686-mingw32/cloog/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [150/197] /i686-mingw32/cloog/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/cloog-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [151/197] /i686-mingw32/cloog/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
popenv
popenv
popenv

# task [152/197] /i686-mingw32/libelf/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/libelf-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls
popd
popenv
popenv
popenv

# task [153/197] /i686-mingw32/libelf/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [154/197] /i686-mingw32/libelf/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/libelf-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [155/197] /i686-mingw32/toolchain/binutils/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09 /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-mingw32
touch /scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-mingw32/.gnu-stamp
popenv
popenv
popenv

# task [156/197] /i686-mingw32/toolchain/binutils/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/obj/binutils-src-2011.09-86-mips-sde-elf-i686-mingw32/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --with-sysroot=/opt/codesourcery/mips-sde-elf --enable-poison-system-directories
popd
popenv
popenv
popenv

# task [157/197] /i686-mingw32/toolchain/binutils/libiberty
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
make -j4 all-libiberty
popd
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09/include /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32/libiberty/libiberty.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
popenv
popenv
popenv

# task [158/197] /i686-mingw32/toolchain/binutils/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [159/197] /i686-mingw32/toolchain/binutils/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
make install prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share
popd
popenv
popenv
popenv

# task [160/197] /i686-mingw32/toolchain/binutils/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32
rm lib/charset.alias
rm ./lib/libiberty.a
rmdir ./lib
popd
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share install-html
popd
pushd /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share install-pdf
popd
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32/bfd/.libs/libbfd.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32/bfd/bfd.h /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
cp /scratch/cmoore/elf-64bit-lite/src/binutils-2011.09/bfd/elf-bfd.h /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
cp /scratch/cmoore/elf-64bit-lite/obj/binutils-2011.09-86-mips-sde-elf-i686-mingw32/opcodes/.libs/libopcodes.a /scratch/cmoore/elf-64bit-lite/obj/host-binutils-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/configure.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc/configure.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/configure.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly configure
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/standards.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc/standards.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/standards.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly standards
rmdir /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/bfd.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/bfd.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/bfd.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-ld.bfd.exe
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/ld.bfd.exe
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/ld.bfd.exe
popenv
popenv
popenv

# task [161/197] /i686-mingw32/toolchain/copy_libs
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/share/doc/mips-mips-sde-elf/html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/share/doc/mips-mips-sde-elf/pdf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/share/doc/mips-mips-sde-elf/info /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/share/doc/mips-mips-sde-elf/man /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man
cp /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/share/doc/mips-mips-sde-elf/LICENSE.txt /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/lib /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/lib
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/include /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/include
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/include/c++ /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/include/c++
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/lib/gcc/mips-sde-elf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib/gcc/mips-sde-elf
popenv

# task [162/197] /i686-mingw32/toolchain/gcc_final/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushenvvar CC_FOR_TARGET mips-sde-elf-gcc
pushenvvar GCC_FOR_TARGET mips-sde-elf-gcc
pushenvvar CXX_FOR_TARGET mips-sde-elf-g++
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/gcc-4.5-2011.09/configure --build=i686-pc-linux-gnu --host=i686-mingw32 --target=mips-sde-elf --enable-threads --disable-libmudflap --disable-libssp --disable-libstdcxx-pch --with-arch-32=mips32r2 --with-arch-64=mips64r2 --enable-sgxx-sde-multilibs --disable-threads --with-gnu-as --with-gnu-ld '--with-specs=%{save-temps: -fverbose-asm} -D__CS_SOURCERYGXX_MAJ__=2011 -D__CS_SOURCERYGXX_MIN__=9 -D__CS_SOURCERYGXX_REV__=86 %{O2:%{!fno-remove-local-statics: -fremove-local-statics}} %{O*:%{O|O0|O1|O2|Os:;:%{!fno-remove-local-statics: -fremove-local-statics}}}' --enable-languages=c,c++ --disable-shared --enable-lto --with-newlib '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --prefix=/opt/codesourcery --with-headers=yes --with-sysroot=/opt/codesourcery/mips-sde-elf --with-build-sysroot=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf --with-libiconv-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-gmp=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-mpfr=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-mpc=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-ppl=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr '--with-host-libstdcxx=-static-libgcc -Wl,-Bstatic,-lstdc++,-Bdynamic -lm' --with-cloog=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-libelf=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-libgomp --enable-poison-system-directories --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/bin --with-build-time-tools=/scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/bin
popd
popenv
popenv

# task [163/197] /i686-mingw32/toolchain/gcc_final/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushenvvar CC_FOR_TARGET mips-sde-elf-gcc
pushenvvar GCC_FOR_TARGET mips-sde-elf-gcc
pushenvvar CXX_FOR_TARGET mips-sde-elf-g++
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-mingw32
make -j4 LDFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf CPPFLAGS_FOR_TARGET=--sysroot=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf build_tooldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf all-gcc
popd
popenv
popenv

# task [164/197] /i686-mingw32/toolchain/gcc_final/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushenvvar CC_FOR_TARGET mips-sde-elf-gcc
pushenvvar GCC_FOR_TARGET mips-sde-elf-gcc
pushenvvar CXX_FOR_TARGET mips-sde-elf-g++
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
ln -s . /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
pushd /scratch/cmoore/elf-64bit-lite/obj/gcc-2011.09-86-mips-sde-elf-i686-mingw32
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man install-gcc
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man install-html-gcc
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man install-pdf-gcc
popd
popenv
popenv

# task [165/197] /i686-mingw32/toolchain/gcc_final/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenvvar AR_FOR_TARGET mips-sde-elf-ar
pushenvvar NM_FOR_TARGET mips-sde-elf-nm
pushenvvar OBJDUMP_FOR_TARET mips-sde-elf-objdump
pushenvvar STRIP_FOR_TARGET mips-sde-elf-strip
pushenvvar CC_FOR_TARGET mips-sde-elf-gcc
pushenvvar GCC_FOR_TARGET mips-sde-elf-gcc
pushenvvar CXX_FOR_TARGET mips-sde-elf-g++
pushd /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32
rm bin/mips-sde-elf-gccbug
rmdir include
popd
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/lib/gcc/mips-sde-elf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib/gcc/mips-sde-elf
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/lib /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/lib
copy_dir /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/mips-sde-elf/include/c++ /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/include/c++
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/usr
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/gccinstall /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/gcc/gccinstall.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/gccinstall.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly gccinstall
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/gccint /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/gcc/gccint.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/gccint.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly gccint
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/cppinternals /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/gcc/cppinternals.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/cppinternals.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly cppinternals
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
popenv
popenv

# task [166/197] /i686-mingw32/toolchain/zlib/0/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
rm -rf /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/zlib-1.2.3 /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
popenv

# task [167/197] /i686-mingw32/toolchain/zlib/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
pushenv
pushenvvar CFLAGS '-O3 -fPIC'
pushenvvar CC 'i686-mingw32-gcc '
pushenvvar AR 'i686-mingw32-ar rc'
pushenvvar RANLIB i686-mingw32-ranlib
./configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr
popenv
popd
popenv

# task [168/197] /i686-mingw32/toolchain/zlib/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv

# task [169/197] /i686-mingw32/toolchain/zlib/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/obj/zlib-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv

# task [170/197] /i686-mingw32/toolchain/expat/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/expat-mirror/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --disable-nls
popd
popenv
popenv
popenv

# task [171/197] /i686-mingw32/toolchain/expat/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [172/197] /i686-mingw32/toolchain/expat/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/expat-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [173/197] /i686-mingw32/toolchain/gdb/0/copy
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-mingw32
copy_dir_clean /scratch/cmoore/elf-64bit-lite/src/gdb-2011.09 /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-mingw32
chmod -R u+w /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-mingw32
touch /scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-mingw32/.gnu-stamp
popenv
popenv
popenv

# task [174/197] /i686-mingw32/toolchain/gdb/0/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/obj/gdb-src-2011.09-86-mips-sde-elf-i686-mingw32/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --disable-nls --with-libexpat-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-libiconv-prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-system-gdbinit=/opt/codesourcery/i686-mingw32/mips-sde-elf/lib/gdbinit '--with-gdb-datadir='\''${prefix}'\''/mips-sde-elf/share/gdb'
popd
popenv
popenv
popenv

# task [175/197] /i686-mingw32/toolchain/gdb/0/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [176/197] /i686-mingw32/toolchain/gdb/0/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
make install prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share
popd
popenv
popenv
popenv

# task [177/197] /i686-mingw32/toolchain/gdb/0/postinstall
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushenvvar CPPFLAGS -I/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/include
pushenvvar LDFLAGS -L/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr/lib
pushd /scratch/cmoore/elf-64bit-lite/obj/gdb-2011.09-86-mips-sde-elf-i686-mingw32
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share install-html
make prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share install-pdf
popd
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/annotate /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/annotate.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/annotate.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly annotate
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/gdbint /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/gdbint.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/gdbint.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly gdbint
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/stabs /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/stabs.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/stabs.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly stabs
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/configure.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc/configure.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/configure.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly configure
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/standards.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc/standards.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/standards.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly standards
rmdir /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/etc
rm -rf /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/bfd.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/bfd.pdf
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info/bfd.info
install-info --infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info --remove-exactly bfd
rm -f /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html/libiberty.html /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf/libiberty.pdf
popenv
popenv
popenv

# task [178/197] /i686-mingw32/gdil/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/gdil-2011.09/configure --prefix=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --disable-shared --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 --with-csl-docbook=/scratch/cmoore/elf-64bit-lite/src/csl-docbook-trunk '--with-xml-catalog-files=/usr/local/tools/gcc-4.3.3/share/sgml/docbook/docbook-xsl/catalog.xml /etc/xml/catalog' --disable-nls --with-expat=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr '--with-features=xml autoconf driver config sprite doc'
popd
popenv
popenv
popenv

# task [179/197] /i686-mingw32/gdil/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [180/197] /i686-mingw32/gdil/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/gdil-2011.09-86-mips-sde-elf-i686-mingw32
make install
popd
popenv
popenv
popenv

# task [181/197] /i686-mingw32/mips_sprite/configure
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
rm -rf /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-mingw32
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-mingw32
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-mingw32
/scratch/cmoore/elf-64bit-lite/src/mips_sprite-2011.09/configure --prefix=/opt/codesourcery --build=i686-pc-linux-gnu --target=mips-sde-elf --host=i686-mingw32 '--with-pkgversion=Sourcery CodeBench Lite 2011.09-86' --with-bugurl=https://support.codesourcery.com/GNUToolchain/ --enable-backends=mdimips --disable-nls --with-gdil=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-expat=/scratch/cmoore/elf-64bit-lite/obj/host-libs-2011.09-86-mips-sde-elf-i686-mingw32/usr --with-boards=/scratch/cmoore/elf-64bit-lite/obj/cs3-2011.09-86-mips-sde-elf/boards
popd
popenv
popenv
popenv

# task [182/197] /i686-mingw32/mips_sprite/build
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-mingw32
make -j4
popd
popenv
popenv
popenv

# task [183/197] /i686-mingw32/mips_sprite/install
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushenv
pushenv
pushd /scratch/cmoore/elf-64bit-lite/obj/mips_sprite-2011.09-86-mips-sde-elf-i686-mingw32
make install prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 exec_prefix=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 libdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib htmldir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/html pdfdir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/pdf infodir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/info mandir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share/doc/mips-mips-sde-elf/man datadir=/scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/share
popd
popenv
popenv
popenv

# task [184/197] /i686-mingw32/pretidy_installation
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32
rm ./lib/libiberty.a
popd
popenv

# task [185/197] /i686-mingw32/remove_libtool_archives
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
find /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 -name '*.la' -exec rm '{}' ';'
popenv

# task [186/197] /i686-mingw32/remove_copied_libs
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
popenv

# task [187/197] /i686-mingw32/remove_fixed_headers
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
pushd /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/lib/gcc/mips-sde-elf/4.5.2/include-fixed
popd
popenv

# task [188/197] /i686-mingw32/add_tooldir_readme
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
cat > /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/README.txt <<'EOF0'
The executables in this directory are for internal use by the compiler
and may not operate correctly when used directly.  This directory
should not be placed on your PATH.  Instead, you should use the
executables in ../../bin/ and place that directory on your PATH.
EOF0
popenv

# task [189/197] /i686-mingw32/strip_host_objects
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/cs-make.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/cs-rm.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-addr2line.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-ar.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-as.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-c++.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-c++filt.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-conv.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-cpp.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-elfedit.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-g++.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-gcc-4.5.2.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-gcc.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-gcov.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-gdb.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-gprof.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-ld.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-nm.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-objcopy.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-objdump.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-ranlib.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-readelf.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-run.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-size.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-sprite.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-strings.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/bin/mips-sde-elf-strip.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/ar.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/as.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/c++.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/g++.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/gcc.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/ld.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/nm.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/objcopy.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/objdump.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/ranlib.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/mips-sde-elf/bin/strip.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/cc1.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/collect2.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/install-tools/fixincl.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/cc1plus.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/lto-wrapper.exe
i686-mingw32-strip /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32/libexec/gcc/mips-sde-elf/4.5.2/lto1.exe
popenv

# task [190/197] /i686-mingw32/package_tbz2
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
rm -f /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf-i686-mingw32.tar.bz2
pushd /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32
popd
pushd /scratch/cmoore/elf-64bit-lite/obj
rm -f mips-2011.09
ln -s /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 mips-2011.09
tar cjf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf-i686-mingw32.tar.bz2 --owner=0 --group=0 --exclude=host-i686-pc-linux-gnu --exclude=host-i686-mingw32 mips-2011.09/bin mips-2011.09/lib mips-2011.09/libexec mips-2011.09/mips-sde-elf mips-2011.09/share
rm -f mips-2011.09
popd
popenv

# task [191/197] /i686-mingw32/package_installanywhere/unpack
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-unpack -i /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-mingw32 -f /scratch/cmoore/elf-64bit-lite/obj/getting_started-2011.09-86-mips-sde-elf/license.html sgxx/2011.09/mips-sde-elf-lite
popenv

# task [192/197] /i686-mingw32/package_installanywhere/merge_modules
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper-mm -i /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-mingw32 -m iamm-common sgxx/2011.09/mips-sde-elf-lite
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper-mm -i /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-mingw32 -m iamm-jvm sgxx/2011.09/mips-sde-elf-lite
popenv

# task [193/197] /i686-mingw32/package_installanywhere/installer
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
pushenvvar CC i686-mingw32-gcc
pushenvvar CXX i686-mingw32-g++
pushenvvar AR i686-mingw32-ar
pushenvvar RANLIB i686-mingw32-ranlib
prepend_path PATH /scratch/cmoore/elf-64bit-lite/obj/tools-i686-pc-linux-gnu-2011.09-86-mips-sde-elf-i686-mingw32/bin
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-ia-helper -i /scratch/cmoore/elf-64bit-lite/install/host-i686-mingw32 -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -h i686-mingw32 -f /scratch/cmoore/elf-64bit-lite/obj/ia-2011.09-86-mips-sde-elf-i686-mingw32/license.html sgxx/2011.09/mips-sde-elf-lite
popenv

# task [194/197] /fini/build_summary
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
cat > /scratch/cmoore/elf-64bit-lite/obj/gnu-2011.09-86-mips-sde-elf.txt <<'EOF0'
Version Information
===================

Version:           2011.09-86
Host spec(s):      i686-pc-linux-gnu i686-mingw32
Target:            mips-sde-elf

Build Information
=================

Build date:             20120304
Build machine:          build6-lucid-cs
Build operating system: squeeze/sid
Build uname:            Linux build6-lucid-cs 2.6.32-28-generic #55-Ubuntu SMP Mon Jan 10 23:42:43 UTC 2011 x86_64 GNU/Linux
Build user:             cmoore

EOF0
popenv

# task [195/197] /fini/extras_package
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.extras
pushd /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf
tar cjf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf.extras.tar.bz2 --owner=0 --group=0 mips-2011.09-86-mips-sde-elf.extras
popd
popenv

# task [196/197] /fini/backups_package
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf.backup
pushd /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf
tar cjf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf.backup.tar.bz2 --owner=0 --group=0 mips-2011.09-86-mips-sde-elf.backup
popd
popenv

# task [197/197] /fini/sources_package
pushenv
pushenvvar CC_FOR_BUILD i686-pc-linux-gnu-gcc
mkdir -p /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
cp /scratch/cmoore/elf-64bit-lite/obj/gnu-2011.09-86-mips-sde-elf.txt /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
cp /scratch/cmoore/elf-64bit-lite/logs/mips-2011.09-86-mips-sde-elf.sh /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf/mips-2011.09-86-mips-sde-elf
pushd /scratch/cmoore/elf-64bit-lite/obj/pkg-2011.09-86-mips-sde-elf
tar cjf /scratch/cmoore/elf-64bit-lite/pkg/mips-2011.09-86-mips-sde-elf.src.tar.bz2 --owner=0 --group=0 mips-2011.09-86-mips-sde-elf
popd
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-test -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -T /scratch/cmoore/elf-64bit-lite/obj/testlogs-2011.09-86-mips-sde-elf sgxx/2011.09/mips-sde-elf-lite
copy_dir /scratch/cmoore/elf-64bit-lite/obj/testlogs-2011.09-86-mips-sde-elf /scratch/cmoore/elf-64bit-lite/testlogs
/scratch/cmoore/elf-64bit-lite/src/scripts-trunk/gnu-test-package -i /scratch/cmoore/elf-64bit-lite/install -l /scratch/cmoore/elf-64bit-lite/logs -o /scratch/cmoore/elf-64bit-lite/obj -p /scratch/cmoore/elf-64bit-lite/pkg -s /scratch/cmoore/elf-64bit-lite/src -T /scratch/cmoore/elf-64bit-lite/testlogs -T /scratch/cmoore/elf-64bit-lite/obj/testlogs-2011.09-86-mips-sde-elf sgxx/2011.09/mips-sde-elf-lite
