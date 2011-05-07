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
 *  fast implementation of DIV          * 
 ****************************************/
        .align 2
FUNC_START(__divdf3)
        mflr    r11
        li      unpack_op, -1                                   # set r12!=0 to indicate DIV operation
        mtctr   r11                                             # now return address in CTR
        bl      __spe_unpackdf2_mul_div@local

L(exp_div):
        subf    exp_res, exp_b, exp_a
        addi    exp_res, exp_res, +1023
        /*
        This early termination caused some cases 
        in POW IEEE tests (1.0/DENORM<INF) to be calculated incorrectly !
        So in DIV "BGE" corrected to "BGT", 
        because "exp" decrement by 1 is possible: 
        (exp_a-exp_b+1023)=0x7ff -> -1_if_B_bigger = 0x7fe.
        However in MUL this never happens, so MUL's "BGE" is unchanged.
        */
        cmpi    cr1, 0, exp_res, 2047
        #bgt-    cr1, return_inf_mul_div                        # move INF check later 
        /*Note: exp_b is not needed now, so r8 used as temporary variable */

L(perform_div):
        cmpl    cr6, 0, frac_a_hi, frac_b_hi  
        cmpl    cr7, 0, frac_a_lo, frac_b_lo 
        #bgt-    cr1, return_inf_mul_div
        bgt+    cr6, L(a_bigger_div)
        blt-    cr6, L(b_bigger_div)
        bge+    cr7, L(a_bigger_div)

L(b_bigger_div):
        /*shift "A" fraction 1 bit left and decrease exponent*/
        rlwinm  frac_a_hi, frac_a_hi, 1, 0, 30
        cmpi    cr1,    0, exp_res, 2047+1
        addi    exp_res, exp_res, -1
        rlwimi  frac_a_hi, frac_a_lo, 1, 31, 31
        rlwinm  frac_a_lo, frac_a_lo, 1, 0, 30

L(a_bigger_div):        
        /* we form result in "1.53" format in {r9,r10} */
        blt+    cr1, 1f
        b       __spe_return_inf_mul_div@local
1:
        li      val_frac_hi, 0x0000
        li      val_frac_lo, 0x0000

        /*FIRST PART OF THE DIV LOOP - process upper 22 bits of fraction !*/
        lis     mask, 0x0020                                    # =(1 << 53) mask

L(loop1_div):
        cmpl    cr6, 0, frac_a_hi, frac_b_hi  
        cmpl    cr7, 0, frac_a_lo, frac_b_lo
        /*
        one with early termination  = cr6_eq && cr7_eq
        one = cr6_gt || (cr6_eq && cr7_gt)
        zero = cr6_lt || (cr6_eq && cr7_lt)
        */
        blt-    cr6, L(bit_zero_loop1_div) 
        bgt-    cr6, L(bit_one_loop1_div) 
        blt-    cr7, L(bit_zero_loop1_div) 
        beq-    cr7, L(loop1_exact_div)
        /*bgt-    cr7, bit_one_loop1_div */
L(bit_one_loop1_div):
        subfc   frac_a_lo, frac_b_lo, frac_a_lo
        or      val_frac_hi, val_frac_hi, mask
        subfe   frac_a_hi, frac_b_hi, frac_a_hi
L(bit_zero_loop1_div):        
        rlwinm  frac_a_hi, frac_a_hi, 1, 0, 30                  # shift "A" fraction 1 bit left
        rlwinm. mask, mask, 31, 1, 31                           # can't roll back to bit #0 !
        rlwimi  frac_a_hi, frac_a_lo, 1, 31, 31
        rlwinm  frac_a_lo, frac_a_lo, 1, 0, 30
        bne+    cr0, L(loop1_div)

        /*SECOND PART OF THE DIV LOOP - process lower 32 bits of fraction !*/
        lis mask, 0x8000                                        # = (1<<31) mask
L(loop2_div):
        cmpl    cr6, 0, frac_a_hi, frac_b_hi  
        cmpl    cr7, 0, frac_a_lo, frac_b_lo
        /*
        one with early termination  = cr6_eq && cr7_eq
        one = cr6_gt || (cr6_eq && cr7_gt)
        zero = cr6_lt || (cr6_eq && cr7_lt)
        */
        blt-    cr6, L(bit_zero_loop2_div) 
        bgt-    cr6, L(bit_one_loop2_div) 
        blt-    cr7, L(bit_zero_loop2_div) 
        beq-    cr7, L(loop2_exact_div)
        /*bgt-    cr7, bit_one_loop2_div */
L(bit_one_loop2_div):
        subfc   frac_a_lo, frac_b_lo, frac_a_lo
        or      val_frac_lo, val_frac_lo, mask
        subfe   frac_a_hi, frac_b_hi, frac_a_hi
L(bit_zero_loop2_div):        
        rlwinm  frac_a_hi, frac_a_hi, 1, 0, 30                  # shift "A" fraction 1 bit left
        rlwinm. mask, mask, 31, 1, (32 - frac_div_shift)        # can't roll back to bit #0, omit last extra bits !
        rlwimi  frac_a_hi, frac_a_lo, 1, 31, 31
        rlwinm  frac_a_lo, frac_a_lo, 1, 0, 30
        bne+    cr0, L(loop2_div)

L(performed_div):
        /* "1.(52+...)" fraction format, shift {r3,r4} right by ... */
        rlwinm  frac_res_lo, val_frac_lo, (32-frac_div_shift), (frac_div_shift), 31 
        rlwinm  rem_hi, val_frac_lo, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      rem_lo, 0
        cmpi    cr0, 0, exp_res, 0x0000
        cmpi    cr1, 0, exp_res, 2047
        rlwinm  frac_res_hi, val_frac_hi, (32-frac_div_shift), 11, 31 
        rlwimi  frac_res_lo, val_frac_hi, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      pack_op, +1                                     #inexact result, round to bigger
        #b       __packdf1_mul_div
        b      __spe_packf1_mul_div@local

L(loop1_exact_div):
        or      val_frac_hi, val_frac_hi, mask
        rlwinm  frac_res_lo, val_frac_lo, (32-frac_div_shift), (frac_div_shift), 31 
        cmpi    cr0, 0, exp_res, 0x0000
        cmpi    cr1, 0, exp_res, 2047
        rlwinm  frac_res_hi, val_frac_hi, (32-frac_div_shift), 11, 31 
        rlwinm  rem_hi, val_frac_lo, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      rem_lo, 0
        rlwimi  frac_res_lo, val_frac_hi, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      pack_op, +0                                     #exact result, accurate rounding
        #b       __packdf1_mul_div
        b	__spe_packf1_mul_div@local

L(loop2_exact_div):
        or      val_frac_lo, val_frac_lo, mask
        rlwinm  frac_res_hi, val_frac_hi, (32-frac_div_shift), 11, 31 
        cmpi    cr0, 0, exp_res, 0x0000
        cmpi    cr1, 0, exp_res, 2047
        rlwinm  frac_res_lo, val_frac_lo, (32-frac_div_shift), (frac_div_shift), 31 
        rlwinm  rem_hi, val_frac_lo, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      rem_lo, 0
        rlwimi  frac_res_lo, val_frac_hi, (32-frac_div_shift), 0, (frac_div_shift-1) 
        li      pack_op, +0                                     #exact result, accurate rounding
        #b       __packdf1_mul_div
        b	__spe_packf1_mul_div@local

FUNC_END(__divdf3)
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

FUNC_START(__divdf3)
	evmergelo	a,	r3,r4		# merge a
	evmergelo	b_,	r5,r6		# merge b

	efddiv		c,	a,	b_	# c = a/b -- division in person

	mfspr		r0,	SPEFSCR
	andi.		r9, r0,	XPTN		# mask all exeptions

	evmergelohi	r3,	r4,	c	# split result
	beqlr+		cr0			# end exit if no exeptions

L(div_exeption):
	mflr	r3				# store return point
	mtctr	r3

	evmergelohi	ha,	la,	a	# split a back to r9 -r10
	evmergelohi	hb,	lb,	b_	# split b back to r11-r12

	andi.	k,	r0,	OVUN
	li	k,	0
	bne-	cr0,	L(div_normalized)

	rlwinm	ea,	ha,	12,21,31	# exp of a
	rlwinm	eb,	hb,	12,21,31	# exp of b

	cmpwi	cr1,	ea,	0x7ff
	cmpwi	cr5,	eb,	0x7ff
	cmpwi	cr6,	ea,	0x001
	cmpwi	cr7,	eb,	0x001

	bge-	cr1,	L(div_a_nan_inf)
	bge-	cr5,	L(div_b_nan_inf)

	blt+	cr6,	L(div_a_zero_denorm)
	blt+	cr7,	L(div_b_zero_denorm)

L(div_return_error):
	lis	hc,	0x1234
	ori	hc, hc,	0x5678
	lis	lc,	0x9abc
	ori	lc, lc,	0xdef0
	bctr

L(div_return_zero):
	li	hc,	0
	li	lc,	0
	b	L(div_return_signed)

L(div_return_inf):
	lis	hc,	0x7ff0
	li	lc,	0
L(div_return_signed):
	xor	r5,	ha,	hb		# get sign
	andis.	r5,	r5,	0x8000
	or	hc,	hc,	r5
	bctr

L(div_return_nan):
	lis	hc,	0x7fff
	li	lc,	0
	bctr

L(div_a_zero_denorm):
	clrlwi	r3,	ha,	1
	or.	r3,	r3,	la

	beq-	cr0,	L(div_a_zero)		# if a = 0

	mr	r3,	ha
	mr	r4,	la
	li	r5,	+54
	addi	k,	k,	-54
	bl	__spe_muldiv_scale2@local	# scale up a
	mr	ha,	r3
	mr	la,	r4

	bge+	cr7,	L(div_normalized)

L(div_b_zero_denorm):
	clrlwi	r4,	hb,	1		# check if b = 0
	or.	r4,	r4,	lb

	beq-	cr0,	L(div_return_inf)		# any/0 = Inf

	mr	r3,	hb
	mr	r4,	lb
	li	r5,	+54
	addi	k,	k,	+54
	bl	__spe_muldiv_scale2@local		# scale up b
	mr	hb,	r3
	mr	lb,	r4

L(div_normalized):
	rlwinm	ea,	ha,	12,21,31	# exp of a
	rlwinm	eb,	hb,	12,21,31	# exp of b

	xor	r7,	ha,	hb		# store sign
	andis.	r7,	r7,	0x8000

	sub	r3,	ea,	eb		# ec = ea - eb
	add	r5,	k,	r3

	clrlwi	ha,	ha,	12		# remove exp
	clrlwi	hb,	hb,	12		# remove exp
	oris	ha,	ha,	0x3ff0		# normalize a
	oris	hb,	hb,	0x3ff0		# normalize b

	evmergelo	a,	ha,la		# merge normalized a
	evmergelo	b_,	hb,lb		# merge normalized b

	efddiv		c,	a,	b_	# divide normalized
	evmergelohi	hc,	lc,	c	# split result

	or	hc,	hc,	r7		# apply sign

	bl	__spe_muldiv_scale2@local	# scale result back

	bctr

L(div_a_nan_inf):
	clrlwi	r3,	ha,	12
	or.	r3,	r3,	la
	bne-	cr0,	L(div_return_nan)		# NaN/any = NaN
	bge-	cr5,	L(div_return_nan)		# any/NaN of Inf/Inf = NaN

	b	L(div_return_inf)			# else Inf/any = Inf

L(div_b_nan_inf):					# here a is not NaN or Inf !
	clrlwi	r4,	hb,	12
	or.	r4,	r4,	lb
	bne-	cr0,	L(div_return_nan)		# any/NaN = NaN

	b	L(div_return_zero)			# any/Inf = 0

L(div_a_zero):					# here a = 0
	clrlwi	r4,	hb,	1		# check if b = 0
	or.	r4,	r4,	lb

	beq-	cr0,	L(div_return_nan)		# 0/0 = NaN

	b	L(div_return_zero)			# else return zero

FUNC_END(__divdf3)
#endif
