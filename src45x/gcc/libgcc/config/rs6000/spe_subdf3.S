#******************************************************************************
#*
#* Copyright (c) 2004 Freescale Semiconductor, Inc
#* All rights reserved.
#*
#* Redistribution and use in source and binary forms, with or without
#* modification, are permitted provided that the following conditions are met:
#*     * Redistributions of source code must retain the above copyright
#*       notice, this list of conditions and the following disclaimer.
#*     * Redistributions in binary form must reproduce the above copyright
#*       notice, this list of conditions and the following disclaimer in the
#*       documentation and/or other materials provided with the distribution.
#*     * Neither the name of Freescale Semiconductor nor the
#*       names of its contributors may be used to endorse or promote products
#*       derived from this software without specific prior written permission.
#*
#* THIS SOFTWARE IS PROVIDED BY Freescale Semiconductor ``AS IS'' AND ANY
#* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#* DISCLAIMED. IN NO EVENT SHALL Freescale Semiconductor BE LIABLE FOR ANY
#* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#*
#*  Functions:    __subdf3(_d_sub), __adddf3(_d_add), __packf1_add_sub
#*
#*  Description:  implements floating point double precision
#*                software emulation for ADD() and SUB() operation
#*                for GNU (libgcc) & ABI runtime libraries 
#*
#*  Notes:        none
#*
#******************************************************************************

#include <ppc-asm.h>


/* Local labels stripped out by the linker.  */
#undef L
#define L(x) .L##x

        .section    ".text"

#ifdef _SOFT_DOUBLE
#define cr0_lt 0
#define cr0_gt 1
#define cr0_eq 2
#define cr0_so 3
#define cr1_lt 4
#define cr1_gt 5
#define cr1_eq 6
#define cr1_so 7
#define cr5_lt 20
#define cr5_gt 21
#define cr5_eq 22
#define cr5_so 23
#define cr6_lt 24
#define cr6_gt 25
#define cr6_eq 26
#define cr6_so 27
#define cr7_lt 28
#define cr7_gt 29
#define cr7_eq 30
#define cr7_so 31

        #ARGUMENTS AND RESULT COMPONENTS
#define a_hi r3
#define a_lo r4
#define b_hi r5
#define b_lo r6
#define res_hi a_hi
#define res_lo a_lo
#define frac_a_hi a_hi
#define frac_a_lo a_lo
#define frac_b_hi b_hi
#define frac_b_lo b_lo
#define exp_a r7
#define exp_b r8
#define sign_a r9
#define sign_b r10
#define frac_res_hi res_hi
#define frac_res_lo res_lo
#define exp_res exp_a
#define sign_res sign_a

        #VARIABLES FOR FRACTION SHIFT
#define cnt_lz_hi r5
#define cnt_lz_lo r6
#define cnt_lz r5
#define lsh r11
#define rsh r12
        
        #VARIABLES FOR FRACTION ROUND
#define round_number r6
#define round_num_lo r5
#define round_num_hi r6
#define frac_shift 8
#define lz_normal (11-frac_shift)
#define exp_shift 20
#define temp r0
#define temp1 r8
#define temp2 r10
#define temp3 r5
#define temp4 r6
#define temp5 r11
#define temp6 r12
                
/****************************************
 *  fast implementation of SUB/ADD      * 
 ****************************************/
        .align  2
FUNC_START(__subdf3)
        /* INVERT SIGN OF SECOND ARGUMENT ("B") */
        xoris   b_hi, b_hi, 0x8000
        b       __adddf3@local
FUNC_END(__subdf3)
#else
#define lt0 0
#define gt0 1
#define eq0 2
#define so0 3
#define lt1 4
#define gt1 5
#define eq1 6
#define so1 7
#define lt2 8
#define gt2 9
#define eq2 10
#define so2 11
#define lt3 12
#define gt3 13
#define eq3 14
#define so3 15
#define lt4 16
#define gt4 17
#define eq4 18
#define so4 19
#define lt5 20
#define gt5 21
#define eq5 22
#define so5 23
#define lt6 24
#define gt6 25
#define eq6 26
#define so6 27
#define lt7 28
#define gt7 29
#define eq7 30
#define so7 31

#define a r10
#define b_ r12
#define c r4

#define ha r9
#define la r10
#define hb r11
#define lb r12
#define k r8
#define ea r3
#define eb r4
#define hc r3
#define lc r4

#define SPEFSCR 512

#define XPTN 0x0f00
#define FINV 0x0800
#define FDBZ 0x0400
#define FUNF 0x0200
#define FOVF 0x0100
#define OVUN 0x0300

/******************************************************************
*	fast implementation of subtraction IEEE compatible,
*       no exeption flags setting
******************************************************************/
	.align  2

FUNC_START(__subdf3)
	evmergelo	a,	r3,r4		# merge a
	evmergelo	b_,	r5,r6		# merge b

	efdsub		c,	a,	b_	# c = a-b -- subtraction in person

	mfspr		r0,	SPEFSCR
	andi.		r9, r0,	XPTN		# mask all exeptions

	evmergelohi	r3,	r4,	c	# split result
	beqlr+		cr0			# end exit if no exeptions

L(sub_exeption):
	efdneg		b_,	b_		# b = -b

	b		__spe_add_exception@local		# and go to add

#endif
