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

HIDDEN_FUNC(__spe_add_exception)
	mflr	r3				# store return point
	mtctr	r3

	evmergelohi	ha,	la,	a	# split a back to r9 -r10
	evmergelohi	hb,	lb,	b_	# split b back to r11-r12

	andi.	r7,	r0,	FOVF
	bne-	cr0,	L(add_overflow)

	andi.	r7,	r0,	FUNF
	bne-	cr0,	L(add_underflow)

	rlwinm	ea,	ha,	12,21,31	# exp of a
	rlwinm	eb,	hb,	12,21,31	# exp of b

	cmpwi	cr1,	ea,	0x7ff
	cmpwi	cr5,	eb,	0x7ff
	cmpwi	cr6,	ea,	0x001
	cmpwi	cr7,	eb,	0x001

	bge-	cr1,	L(add_a_nan_inf)
	bge-	cr5,	L(add_b_nan_inf)
	blt-	cr6,	L(add_a_zero_denorm)
	blt-	cr7,	L(add_b_zero_denorm)

L(add_return_nan):
	lis	hc,	0x7fff
	li	lc,	0
	bctr

L(add_return_a):
	mr	hc,	ha
	mr	lc,	la	
	bctr

L(add_return_b):
	mr	hc,	hb
	mr	lc,	lb	
	bctr

L(add_overflow):
	mr	r3,	ha
	mr	r4,	la
	li	r5,	-54
	bl	__spe_muldiv_scale2@local	# scale up a
	mr	ha,	r3
	mr	la,	r4

	mr	r3,	hb
	mr	r4,	lb
	li	r5,	-54
	bl	__spe_muldiv_scale2@local	# scale up b
	mr	hb,	r3
	mr	lb,	r4

	li	k,	+54
	b	L(add_normalized)
	
L(add_underflow):
	mr	r3,	ha
	mr	r4,	la
	li	r5,	+54
	bl	__spe_muldiv_scale2@local	# scale up a
	mr	ha,	r3
	mr	la,	r4

	mr	r3,	hb
	mr	r4,	lb
	li	r5,	+54
	bl	__spe_muldiv_scale2@local	# scale up b
	mr	hb,	r3
	mr	lb,	r4

	li	k,	-54
	b	L(add_normalized)

L(add_normalized):
	evmergelo	a,	ha,la		# merge normalized a
	evmergelo	b_,	hb,lb		# merge normalized b

	efdadd		c,	a,	b_	# add normalized
	evmergelohi	hc,	lc,	c	# split result

	mr	r5,	k

	bl	__spe_muldiv_scale2@local	# scale result back

	bctr

L(add_a_nan_inf):
	clrlwi	r3,	ha,	12
	or.	r3,	r3,	la
	bne-	cr0,	L(add_return_nan)		# NaN+any = NaN
	blt-	cr5,	L(add_return_a)		# Inf+any = Inf

	clrlwi	r4,	hb,	12
	or.	r4,	r4,	lb
	bne-	cr0,	L(add_return_nan)		# any+NaN = NaN

	xor	r3,	ha,	hb		# here a&b = Inf
	andis.	r3,	r3,	0x8000
	bne-	cr0,	L(add_return_nan)		# +Inf + -Inf = NaN

	b	L(add_return_a)			# else Inf+any = Inf

L(add_b_nan_inf):
	clrlwi	r4,	hb,	12
	or.	r4,	r4,	lb
	bne-	cr0,	L(add_return_nan)		# any+NaN = NaN

	b	L(add_return_b)			# else Inf+any = Inf

L(add_a_zero_denorm):
	clrlwi	r7,	ha,	1
	or.	r7,	r7,	la
	beq-	cr0,	L(add_a_zero)

	sub	r7,	ea,	eb
	cmpwi	cr0,	r7,	52
	cmpwi	cr1,	r7,	-52

	bgt-	cr0,	L(add_return_a)
	blt-	cr1,	L(add_return_b)

	b	L(add_underflow)

L(add_b_zero_denorm):
	clrlwi	r7,	hb,	1
	or.	r7,	r7,	lb
	beq-	cr0,	L(add_return_a)

	sub	r7,	ea,	eb
	cmpwi	cr0,	r7,	52
	cmpwi	cr1,	r7,	-52

	bgt-	cr0,	L(add_return_a)
	blt-	cr1,	L(add_return_b)

	b	L(add_underflow)

L(add_a_zero):
	clrlwi	r7,	hb,	1
	or.	r7,	r7,	lb
	bne-	cr0,	L(add_return_b)

	and	r3,	ha,	hb
	li	r4,	0

	bctr
FUNC_END(__spe_add_exception)
#endif
