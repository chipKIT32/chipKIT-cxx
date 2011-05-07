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
                
        #ARGUMENTS AND RESULT COMPONENTS
#define a_hi r3
#define a_lo r4
#define b_hi r5
#define b_lo r6
#define frac_a_hi a_hi
#define frac_a_lo a_lo
#define frac_b_hi b_hi
#define frac_b_lo b_lo
#define exp_a r7
#define exp_b r8
                
#define res_hi a_hi
#define res_lo a_lo
#define frac_res_hi res_hi
#define frac_res_lo res_lo
#define exp_res exp_a
#undef sign_res
#define sign_res r0

        #VARIABLES FOR FRACTION SHIFT
#undef cnt_lz_hi
#define cnt_lz_hi r9
#undef cnt_lz_lo
#define cnt_lz_lo r10
#undef cnt_lz
#define cnt_lz r10
#define lsh r11
#define rsh r12

        #VARIABLES FOR FRACTION ROUND
#define round_one r9
#undef round_number
#define round_number r10
#define round_lsb r10
#undef round_num_lo
#define round_num_lo r9
#undef round_num_hi
#define round_num_hi r10

        #VARIABLES FOR DIVISION
#define mask r8
#define val_frac_hi r9
#define val_frac_lo r10
                
        #UNPACKF2 PARAMETER
#define unpack_op r12
        #PACKF1 PARAMETER
#define rem_hi r5
#define rem_lo r6
#define pack_op r9

#define frac_arg_shift 0
#define lz_arg_normal (11-frac_arg_shift)
#define frac_res_shift 0
#define lz_res_normal (11-frac_res_shift)

#define frac_mul_shift 0
#define lz_mul_normal (11-frac_mul_shift)
#define frac_div_shift 1
#define lz_div_normal (11-frac_div_shift)

#define exp_shift 20

#undef temp
#define temp r8
#undef temp1
#define temp1 r9
#define temp2 r10
#undef temp3
#define temp3 r11
#undef temp4
#define temp4 r12
/****************************************
 *  fast implementation of MUL          * 
 ****************************************/
        .align 2

FUNC_START(__muldf3)
        mflr r11
        li      unpack_op, 0                                    # set r12=0 to indicate MUL operation
        mtctr   r11                                             # now return address in CTR
        bl      __spe_unpackdf2_mul_div@local

L(exp_mul):
        add     exp_res, exp_b, exp_a
        addi    exp_res, exp_res, -1023
        /*
        This early termination caused some cases 
        in POW IEEE tests (1.0/DENORM<INF) to be calculated incorrectly !
        So in DIV "BGE" corrected to "BGT", 
        because "exp" decrement by 1 is possible: 
        (exp_a-exp_b+1023)=0x7ff -> -1_if_B_bigger = 0x7fe.
        However in MUL this never happens, so MUL's "BGE" is unchanged.
        */
        cmpi    cr1, 0, exp_res, 2047
        blt+    cr1, L(perform_mul)
        b       __spe_return_inf_mul_div@local
        /*Note: exp_b is not needed now, so r8 used as temporary variable */

L(perform_mul):        
        /*
        basic calculation: res {1,2,3,4} = (<<64)(a_hi*b_hi)+(<<32)(a_hi*b_lo+a_lo*b_hi)+(<<0)(a_lo*b_lo) 
        uses 4*mulu64+2*addu64 operations 
        */
        mullw   r12, frac_a_lo, frac_b_hi
        mullw   r10, frac_a_hi, frac_b_lo
        mulhwu  r11, frac_a_lo, frac_b_hi
        mulhwu   r9, frac_a_hi, frac_b_lo
        addc    r10, r10, r12     
        mulhwu   r8, frac_a_lo, frac_b_lo
        adde     r9,  r9, r11     
        mulhwu  r12, frac_a_hi, frac_b_hi
        mullw   r11, frac_a_hi, frac_b_hi
        addze    r3,  r12
        mullw    r6, frac_a_lo, frac_b_lo                       #frac_b_lo is no longer available !
        addc     r5,  r8, r10
        adde     r4,  r9, r11
        addze    r3,  r3
        
        /*
        similar calculation using vector commands: 
        the only advantage is 64-bit multiplication of a_hi*b_lo, a_lo*b_hi 
        merged with their 64-bit addition in accumulator.
        uses 4*vmuladd+3*vmove+1*addu64  
	evmwumia        r11, frac_a_hi, frac_b_lo
	evmwumiaa       r11, frac_a_lo, frac_b_hi
	evmwumi	        r6,  frac_a_lo, frac_b_lo               #frac_b_lo is no longer available !
	evmergelohi     r9,  r11, r11
	evmwumi	        r10, frac_a_hi, frac_b_hi
	evmergelohi     r12, r6,  r6
	evmergelohi     r3,  r10, r10
	addc	        r5,  r11, r12
	adde	        r4,  r9,  r10
	addze	        r3,  r3  
        */
        /*
        similar calculation using vector commands: 
        the only advantage is 64-bit multiplication of a_hi*b_lo, a_lo*b_hi 
        merged with their 64-bit addition in accumulator.
        uses 4*vmuladd+3*vmove+1*addu64  
	evmwumia        r11, frac_a_hi, frac_b_lo
	evmwumi	        r6,  frac_a_lo, frac_b_lo               #frac_b_lo is no longer available !
	evmwumiaa       r11, frac_a_lo, frac_b_hi
	evmwumi	        r10, frac_a_hi, frac_b_hi
	evmergelohi     r12, r6,  r6
	evmergelohi     r9,  r11, r11
	evmergelohi     r3,  r10, r10
	addc	        r5,  r11, r12
	adde	        r4,  r9,  r10
	addze	        r3,  r3  
        */
        /*
        alternate calculation: res {1,2,3,4} = (<<64+<<32)(a_hi*b_hi)-(<<32)(a_lo-a_hi)(b_lo-b_hi)+(<<32+<<0)a_lo*b_lo
        uses 2*sub32+3*mulu64+3*adds64 operations 
        
        ATTENTION!!! THIS DOESN'T WORK:
        need to check that "lo-hi" is not negative ! 
        else need to negate and remember this fact in CRx.
        */
        /*
        subf    r9,  a_hi, a_lo # assume that "lo" is bigger than "hi", else we in trouble
        subf    r10, b_hi, b_lo # assume that "lo" is bigger than "hi", else we in trouble
        mullw  r12,  r9, r10
        mulhwu r11,  r9, r10
        mullw   r9,  r3,  r5
        mulhwu  r3,  r3,  r5
        mulhwu r10,  r4,  r6
        mullw   r6,  r4,  r6 
        subfc   r5, r12,  r9
        subfe   r4, r11,  r3
        subfze  r3,  r3
        addc   r12, r10,  r6
        adde   r11,  r9, r10
        addze   r3,  r3
        addc    r5,  r5, r12
        adde    r4,  r4, r11
        addze   r3,  r3
        */

        /*fraction in r3-r6 now in "1or2.8.32.(12+20x).32x" format, r8-r12 are free*/
       
L(performed_mul):
        /*
        Note: depending on "2.8..." or "1.8..." fraction format in r3 we need to shift left by 11 or 12
        */
        andi.   temp, r3, 0x0200
        bne+    cr0, L(shift_2_mul)

L(shift_1_mul): /* "1.8" fraction format, shift {r3,r4,r5,r6} left by 12 */
        rlwinm  frac_res_hi, r3, 12, 11, 19
        addic.  exp_res, exp_res, +0
        rlwimi  frac_res_hi, r4, 12, 20, 31
        rlwinm  frac_res_lo, r4, 12, 0, 19
        li      pack_op, +0
        rlwimi  frac_res_lo, r5, 12, 20, 31
        rlwinm  rem_hi, r5, 12, 0, 19
        cmpi    cr1, 0, exp_res, 2047
        /*
        TBD: there is no need to shift/split r6 into rem_hi, rem_lo ...
        We need rem_hi shifted correctly to detect "<0",
        but rem_lo we check only for "!=0".
        */
        #rlwimi  rem_hi, r6, 12, 20, 31
        #rlwinm  rem_lo, r6, 12, 0, 19
        b       __spe_packf1_mul_div@local

L(shift_2_mul): /* "2.8" fraction format, shift {r3,r4,r5,r6} left by 11 */
        rlwinm  frac_res_hi, r3, 11, 11, 20
        addic.  exp_res, exp_res, +1
        rlwimi  frac_res_hi, r4, 11, 21, 31
        rlwinm  frac_res_lo, r4, 11, 0, 20
        li      pack_op, +0
        rlwimi  frac_res_lo, r5, 11, 21, 31
        rlwinm  rem_hi, r5, 11, 0, 20
        cmpi    cr1, 0, exp_res, 2047
        /*
        TBD: there is no need to shift/split r6 into rem_hi, rem_lo ...
        We need rem_hi shifted correctly to detect "<0",
        but rem_lo we check only for "!=0".
        */
        #rlwimi  rem_hi, r6, 11, 21, 31
        #rlwinm  rem_lo, r6, 11, 0, 20
        b       __spe_packf1_mul_div@local

FUNC_END(__muldf3)
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

/*********************************************************************
*	fast implementation of multiplication IEEE compatible,
*       no exeption flags setting
*********************************************************************/
	.align  2

FUNC_START(__muldf3)
	evmergelo	a,	r3,r4		# merge a
	evmergelo	b_,	r5,r6		# merge b

	efdmul		c,	a,	b_	# c = a*b

	mfspr		r0,	SPEFSCR
	andi.		r9, r0,	XPTN		# mask all exeptions

	evmergelohi	r3,	r4,	c	# split result
	beqlr+		cr0			# end exit if no exeptions

L(mul_exeption):
	mflr	r3				# store return point
	mtctr	r3

	evmergelohi	ha,	la,	a	# split a back to r9 -r10
	evmergelohi	hb,	lb,	b_	# split b back to r11-r12

	andi.	k,	r0,	OVUN
	li      k,	0
	bne-	cr0,	L(mul_normalized)

	rlwinm	ea,	ha,	12,21,31	# exp of a
	rlwinm	eb,	hb,	12,21,31	# exp of b

	cmpwi	cr1,	ea,	0x7ff
	cmpwi	cr5,	eb,	0x7ff
	cmpwi	cr6,	ea,	0x001
	cmpwi	cr7,	eb,	0x001

	bge-	cr1,	L(mul_a_nan_inf)
	bge-	cr5,	L(mul_b_nan_inf)

	blt+	cr6,	L(mul_a_zero_denorm)
	blt+	cr7,	L(mul_b_zero_denorm)

L(mul_return_error):
	lis	hc,	0x1234
	ori	hc, hc,	0x5678
	lis	lc,	0x9abc
	ori	lc, lc,	0xdef0
	bctr

L(mul_return_zero):
	li	hc,	0
	li	lc,	0
	b	L(mul_return_signed)

L(mul_return_inf):
	lis	hc,	0x7ff0
	li	lc,	0
L(mul_return_signed):
	xor	r5,	ha,	hb		# get sign
	andis.	r5,	r5,	0x8000
	or	hc,	hc,	r5
	bctr

L(mul_return_nan):
	lis	hc,	0x7fff
	li	lc,	0
	bctr

L(mul_a_zero_denorm):
	clrlwi	r3,	ha,	1
	or.	r3,	r3,	la

	beq+	cr0,	L(mul_return_zero)		# 0*any = 0

	mr	r3,	ha
	mr	r4,	la
	li	r5,	+54
	addi	k,	k,	-54
	bl	__spe_muldiv_scale2@local	# scale up a
	mr	ha,	r3
	mr	la,	r4

	bge+	cr7,	L(mul_normalized)

L(mul_b_zero_denorm):
	clrlwi	r4,	hb,	1
	or.	r4,	r4,	lb

	beq-	cr0,	L(mul_return_zero)		# any*0 = 0

	mr	r3,	hb
	mr	r4,	lb
	li	r5,	+54
	addi	k,	k,	-54
	bl	__spe_muldiv_scale2@local	# scale up b
	mr	hb,	r3
	mr	lb,	r4

L(mul_normalized):
	rlwinm	ea,	ha,	12,21,31	# exp of a
	rlwinm	eb,	hb,	12,21,31	# exp of b

	xor	r7,	ha,	hb		# store sign
	andis.	r7,	r7,	0x8000

	add	r3,	ea,	eb		# ec = ea + eb
	add	r5,	k,	r3
	addi	r5,	r5,	-2046

	clrlwi	ha,	ha,	12		# remove exp
	clrlwi	hb,	hb,	12		# remove exp
	oris	ha,	ha,	0x3ff0		# normalize a
	oris	hb,	hb,	0x3ff0		# normalize b

	evmergelo	a,	ha,la		# merge normalized a
	evmergelo	b_,	hb,lb		# merge normalized b

	efdmul		c,	a,	b_	# mutiply normalized
	mfspr   r0, SPEFSCR

	evmergelohi	hc,	lc,	c	# split result

	or	hc,	hc,	r7		# apply sign

	bl	__spe_muldiv_scale2@local	# scale result back

	bctr

L(mul_a_nan_inf):
	clrlwi	r3,	ha,	12
	or.	r3,	r3,	la
	bne-	cr0,	L(mul_return_nan)		# NaN*any = NaN

	clrlwi	r4,	hb,	1
	or.	r4,	r4,	lb
	beq-	cr0,	L(mul_return_nan)		# Inf*0 = NaN

	blt-	cr5,	L(mul_return_inf)		# Inf*any = Inf

L(mul_b_nan_inf):					# here a is not NaN or Inf !
	clrlwi	r4,	hb,	12
	or.	r4,	r4,	lb
	bne-	cr0,	L(mul_return_nan)		# any*NaN = NaN

	clrlwi	r3,	ha,	1
	or.	r3,	r3,	la
	beq-	cr0,	L(mul_return_nan)		# 0*Inf = NaN

	b	L(mul_return_inf)			# else any*Inf = Inf

FUNC_END(__muldf3)
#endif
