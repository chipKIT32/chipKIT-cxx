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
#*  Functions:    entry_cmp, entry_cmpu, __cmpdf2, __unorddf2, 
#*                __gtdf2, __gedf2, __ltdf2, __ledf2, __eqdf2, __nedf2, 
#*                _d_cmp, _d_cmpe, _d_feq, _d_fne, 
#*                _d_fgt, _d_fge, _d_flt, _d_fle
#*
#*  Description:  implements floating point double precision
#*                software emulation for comparison operations
#*                for GNU (libgcc) & ABI runtime libraries 
#*
#*  Notes:        Here GNU and e500 ABI functions are different
#*                but built around the same core.
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

#define a_hi r3
#define a_lo r4
#define b_hi r5
#define b_lo r6

#define a_hi_abs r7
#define b_hi_abs r8
#define sign_a r9
#define sign_b r10

#define inf_hi r0
#define temp r11
#define ret_addr r12
                
#define res r3

#define temp1 r10
#define temp2 r11
#define temp3 r12
                
/****************************************
 *  fast implementation of UNORD        * 
 ****************************************/
        .align  2

FUNC_START(__unorddf2)
/*
L(GNU): return a nonzero value if either argument is NaN, otherwise 0.
*/
        rlwinm  a_hi_abs, a_hi, 0, 1, 31
        lis     inf_hi, 0x7ff0
        rlwinm  b_hi_abs, b_hi, 0, 1, 31
        cmpli   cr5, 0, a_lo, 0x0000
        cmpl    cr7, 0, a_hi_abs, inf_hi
        cmpli   cr1, 0, b_lo, 0x0000
        cmpl    cr6, 0, b_hi_abs, inf_hi
        /*
        (A>INF) = CR7_GT || (CR7_EQ & CR5_GT) 
        (B>INF) = CR6_GT || (CR6_EQ & CR1_GT)
        (A<=INF) = CR7_LT || (CR7_EQ & !CR5_GT) -> move in CR7_LT
        (B<=INF) = CR6_LT || (CR6_EQ & !CR1_GT) -> move in CR6_LT 
        */
        crandc  cr7_eq, cr7_eq, cr5_gt 
        crandc  cr6_eq, cr6_eq, cr1_gt 
        li      temp, +1
        cror    cr7_lt, cr7_lt, cr7_eq
        cror    cr6_lt, cr6_lt, cr6_eq
        isel    res, 0, temp, cr7_lt
        isel    res, res, temp, cr6_lt
        blr
FUNC_END(__unorddf2)
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

#define SPEFSCR 512

#define XPTN 0x0f00
#define FINV 0x0800
#define FDBZ 0x0400
#define FUNF 0x0200
#define FOVF 0x0100
#define OVUN 0x0300

#define a r10
#define b_ r12

/*******************************************************************************
 *	Fast implementation of GNU unordered IEEE compatible comparison
 *	Return a nonzero value if either argument is NaN, otherwise 0.
 ******************************************************************************/
	.align  2

FUNC_START(__unorddf2)

	evmergelo	a,	r3,r4		# merge a
	evmergelo	b_,	r5,r6		# merge b

	li		r3,	0
	li		r4,	1

	b	__unord_check@local
FUNC_END(__unorddf2)
#endif
