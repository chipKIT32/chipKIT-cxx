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
 *  common pack code for MUL/DIV/etc    * 
 ****************************************/
        .align  2
/*
Input parameters:
sign_res (r0) - sign of the number
frac_res_hi & frac_res_lo (r3&r4) - fraction of the number
exp_res (r7) - exponent of the number
operation to perform is defined in r9 (0 - MUL, +1 - DIV).
Optional entry point for MUL/DIV requires CR0=(exp_res<=>0), CR7=(exp_res<=>2047)
CTR contains return address (exits via "bctr"). 

After "pack" work res_hi & res_lo (r3&r4) contain correctly formatted double precision number.
*/
HIDDEN_FUNC(__spe_packf1_mul_div)
        ble-    cr0, L(res_denorm_mul_div)
        bge-    cr1, L(return_inf_mul_div)

L(res_norm_mul_div):
        /*round*/
        cmpi    cr5, 0, rem_hi, 0x0000
        cmpi    cr7, 0, pack_op, 0x0000
        cmpi    cr6, 0, rem_lo, 0x0000
        rlwimi. rem_hi, frac_res_lo, 31, 0, 0        
        bge+    cr5, L(return_norm_mul_div)
        bne-    cr7, L(round_norm_mul_div)
        bne-    cr6, L(round_norm_mul_div)
        beq+    cr0, L(return_norm_mul_div)
L(round_norm_mul_div):
        addic   res_lo, frac_res_lo, +1
        addi    temp1, exp_res, +1
        addze   res_hi, frac_res_hi
        andis.  temp, res_hi, 0x0020 
        isel    exp_res, exp_res, temp1, cr0_eq 
L(return_norm_mul_div):
        /*pack & return*/
        or      res_hi, res_hi, sign_res
        rlwimi  res_hi, exp_res, exp_shift, 1, 11 
        bctr

L(res_denorm_mul_div):
        subfic  rsh, exp_res, (1+frac_res_shift)                # =0or6or8+1-exp_res
        cmpi    cr6, 0, rsh, (53+frac_res_shift)
        cmpi    cr7, 0, rsh, 32
        or      temp2, rem_hi, rem_lo                           # cumulated remainder
        bgt-    cr6, L(return_zero_mul_div)
        beq-    cr6, L(res_53_right_shift_mul_div)                 #special case for 53-bit shift (it doesn't use pack_op)
        beq-    cr7, L(res_32_right_shift_mul_div)
        bgt-    cr7, L(res_big_right_shift_mul_div)
        /*blt-    cr7, res_small_right_shift_mul_div*/

L(res_small_right_shift_mul_div):
        subfic  lsh, rsh, 32                                    # =32-rsh=31+exp_res
        /*shift*/
        slw.    rem_hi, frac_res_lo, lsh
        srw     frac_res_lo, frac_res_lo, rsh
        slw     temp, frac_res_hi, lsh
        srw     res_hi, frac_res_hi, rsh
        or      res_lo, temp, frac_res_lo
        /*round*/
        bge+    cr0, L(res_no_round_small_right_shift_mul_div)
        or      temp2, pack_op, temp2                           # to round 0.5->1.0 instead of 0.5->0.0
        rlwimi  rem_hi, frac_res_lo, 31, 0, 0
        addic   temp, res_lo, +1
        or.     temp2, temp2, rem_hi
        addze   temp1, res_hi
        isel    res_lo, res_lo, temp, cr0_eq
        isel    res_hi, res_hi, temp1, cr0_eq
L(res_no_round_small_right_shift_mul_div):
        /*
        if we got DENORM->NORM during rounding, 
        then leading "1.0" also works as exp == 1 !
        So don't clear bit #11, but use as exp !
        */
        /*pack & return*/
        rlwimi  res_hi, sign_res, 0, 0, (11-1)
        bctr

L(res_32_right_shift_mul_div):
        or.     rem_hi, frac_res_lo, pack_op                    # to round 0.5->1.0 instead of 0.5->0.0
        mr      res_lo, frac_res_hi
        mr      res_hi, sign_res
        /*round*/
        bgectr+ cr0
        rlwimi  rem_hi, frac_res_lo, 31, 0, 0
        addi    temp, res_lo, +1
        or.     temp1, temp2, rem_hi
        isel    res_lo, res_lo, temp, cr0_eq
        bctr
        
L(res_big_right_shift_mul_div):
        subfic  lsh, rsh, +64                                   # = 32-new_rsh = 64-old_rsh
        addi    rsh, rsh, -32
        or      temp1, frac_res_lo, pack_op                     # to round 0.5->1.0 instead of 0.5->0.0
        /*shift*/
        slw.    rem_hi, frac_res_hi, lsh
        srw     res_lo, frac_res_hi, rsh
        mr      res_hi, sign_res
        /*round*/
        bgectr+ cr0
        or      temp2, temp1, temp2
        rlwimi  rem_hi, res_lo, 31, 0, 0
        addi    temp, res_lo, +1
        or.     temp2, temp2, rem_hi
        isel    res_lo, res_lo, temp, cr0_eq
        bctr
        
L(res_53_right_shift_mul_div):
        /*shift*/
        /*or      temp1, frac_res_lo, pack_op                    # to round 0.5->1.0 instead of 0.5->0.0*/
        rlwinm. rem_hi, frac_res_hi, (64-53), 0, 10
        or      temp2, frac_res_lo, temp2                       #use "frac_res_lo" instead of "temp1" (to avoid "pack_op" influence)
        li      res_lo, 0
        mr      res_hi, sign_res
        /*round*/
        bgectr+ cr0
        rlwimi  rem_hi, res_lo, 31, 0, 0
        addi    temp, res_lo, +1
        or.     temp2, temp2, rem_hi
        isel    res_lo, res_lo, temp, cr0_eq
        bctr
FUNC_END(__spe_packf1_mul_div)
/****************************************
 *  common unpack code for MUL/DIV      * 
 ****************************************/
        .align 2

        /*
        "double" values to unpack are located in r3,r4 & r5,r6,
        operation to perform is defined in r12 (0 - MUL, !0 - DIV).
        MUL & DIV differ only in ZERO & INF processing of second argument ("B"). 
        All other cases are processed indentically.
        CTR contains caller's return address.
        LR contains "unpack" return address to caller.
       
        If unpacked normally, 
        results are in 
        sign_res (r0),
        frac_a_hi & frac_a_lo (r3&r4), exp_a(r7), 
        frac_b_hi & frac_b_lo (r5&r6), exp_b(r8).
        #CR1=(a_hi<=>0) i.e. contains sign_a
        
        */
HIDDEN_FUNC(__spe_unpackdf2_mul_div)
        /* UNPACK SIGNS*/
        xor     temp1, a_hi, b_hi                               # create result sign
        li      temp2, 0
        /*cmpi    cr1, 0, a_hi, 0x0000                            #remember sign_a in CR1, DONT USE CR1 for other calculations !*/
        rlwinm  sign_res, temp1, 0, 0, 0                        # extract result sign
        mtcrf   0x03, temp2                                     # clear CR6 & CR7 that are used for signalling about A==ZERO & A==INF
        
        /* UNPACK FIRST ARGUMENT*/
        rlwinm. exp_a, a_hi, (32-exp_shift), 21, 31             # extract exponent, check against zero and max
        rlwinm  frac_a_hi, a_hi, 0, 12, 31                      # extract fraction
        cmpi    cr5, 0, exp_a, 2047
        
        /*now we have cr0=(exp<=>0), cr5=(exp<=>2047))*/
        
        ble-    cr0, L(a_zero_denorm_mul_div)
        bge-    cr5, L(a_nan_inf_mul_div) 
        oris    frac_a_hi, frac_a_hi, 0x0010                    # add leading 1.0 to fraction
        
L(frac_a_ready_mul_div):
        /*CONTINUE*/
        
        /*now CR6=(A==ZERO), CR7=(A==INF)*/
        
        /* UNPACK SECOND ARGUMENT*/
        rlwinm. exp_b, b_hi, (32-exp_shift), 21, 31             # extract exponent, check against zero and max
        rlwinm  frac_b_hi, b_hi, 0, 12, 31                      # extract fraction
        cmpi    cr5, 0, exp_b, 2047
        
        /*now we have cr1=(exp<=>0), cr5=(exp<=>2047))*/

        ble-    cr0, L(b_zero_denorm_mul_div)
        bge-    cr5, L(b_nan_inf_mul_div) 
        /*check for case (ZERO,INF)*(NORM) or (ZERO,INF)/(NORM) */
        beq-    cr6, L(return_a_mul_div)
        beq-    cr7, L(return_a_mul_div)
        oris    frac_b_hi, frac_b_hi, 0x0010                    # add leading 1.0 to fraction
        
L(frac_b_ready_mul_div):
        /*CONTINUE*/
        
        blr

L(a_zero_denorm_mul_div):
        or.     temp1, frac_a_hi, frac_a_lo
        li      temp2, 0x0020
        bne-    cr0, L(a_denorm_mul_div)
L(a_zero_mul_div):
        mtcrf   0x03, temp2                                     # set CR6 EQ bit, clear CR7 EQ bit to show that A==ZERO
        b       L(frac_a_ready_mul_div)                            # continue, if first argument is ZERO
        
L(a_denorm_mul_div):
        cntlzw  cnt_lz_hi, frac_a_hi
        cntlzw  cnt_lz_lo, frac_a_lo
        cmpi    cr5, 0, cnt_lz_hi, 32
        addi    cnt_lz_lo, cnt_lz_lo, +32
        isel    cnt_lz, cnt_lz_hi, cnt_lz_lo, cr5_lt            # cnt = (cnt_hi<32) ? cnt_hi : (cnt_lo+32)
        
        addi    lsh, cnt_lz, -(lz_arg_normal+frac_arg_shift)    # =cnt-11
        subfic  exp_a, cnt_lz, 1+(lz_arg_normal+frac_arg_shift) # =1-lsh=1-cnt+11
        cmpi    cr5, 0, lsh, 32
        bge-    cr5, L(a_big_shift_left_mul_div)
        
L(a_small_shift_left_mul_div):
        subfic  temp2, lsh, 32                                  # use "temp2" instead of "rsh" which is busy as "unpack_op"
        slw     frac_a_hi, frac_a_hi, lsh
        srw     temp1, frac_a_lo, temp2
        slw     frac_a_lo, frac_a_lo, lsh
        or      frac_a_hi, temp1, frac_a_hi
        b       L(frac_a_ready_mul_div)        

L(a_big_shift_left_mul_div):
        addi    lsh, lsh, -32
        slw     frac_a_hi, frac_a_lo, lsh
        li      frac_a_lo, 0
        b       L(frac_a_ready_mul_div)

L(a_nan_inf_mul_div):
        or.     temp1, frac_a_hi, frac_a_lo
        li      temp2, 0x0002
        bne-    cr0, L(return_a_mul_div)                           # return first argument, if it is NAN
L(a_inf_mul_div):
        mtcrf   0x03, temp2                                     # set CR7 EQ bit, clear CR6 EQ bit to show that A==INF
        b       L(frac_a_ready_mul_div)                            # continue if first argument is INF

L(b_zero_denorm_mul_div):
        or.     temp1, frac_b_hi, frac_b_lo
        cmpi    cr5, 0, unpack_op, 0x0000                       # r12==0 in case of MUL, and !=0 in case of DIV
        bne-    cr0, L(b_denorm_mul_div)
        bne-    cr5, L(b_zero_div)
L(b_zero_mul):
        /*check for A=INF i.e. INF*ZERO case */
        beq-    cr7, L(return_nan_mul_div)
        b       L(return_zero_mul_div)                             # return second argument, if it is ZERO
L(b_zero_div):
        /*check for A=ZERO i.e. ZERO/ZERO case */
        beq-    cr6, L(return_nan_mul_div)
        b       L(return_inf_mul_div)

L(b_denorm_mul_div):
        /*check for case (ZERO,INF)*(DENORM) or (ZERO,INF)/(DENORM) */
        beq-    cr6, L(return_a_mul_div)
        beq-    cr7, L(return_a_mul_div)
        
        cntlzw  cnt_lz_hi, frac_b_hi
        cntlzw  cnt_lz_lo, frac_b_lo
        cmpi    cr5, 0, cnt_lz_hi, 32
        addi    cnt_lz_lo, cnt_lz_lo, +32
        isel    cnt_lz, cnt_lz_hi, cnt_lz_lo, cr5_lt            # cnt = (cnt_hi<32) ? cnt_hi : (cnt_lo+32)
        
        addi    lsh, cnt_lz, -(lz_arg_normal+frac_arg_shift)    # =cnt-11
        subfic  exp_b, cnt_lz, 1+(lz_arg_normal+frac_arg_shift) # =1-lsh=1-cnt+11
        cmpi    cr5, 0, lsh, 32
        bge-    cr5, L(b_big_shift_left_mul_div)
        
L(b_small_shift_left_mul_div):
        subfic  temp2, lsh, 32                                  # use "temp2" instead of "rsh" which is busy as "unpack_op"
        slw     frac_b_hi, frac_b_hi, lsh
        srw     temp1, frac_b_lo, temp2
        slw     frac_b_lo, frac_b_lo, lsh
        or      frac_b_hi, temp1, frac_b_hi
        blr     #b       frac_b_ready_mul_div        

L(b_big_shift_left_mul_div):
        addi    lsh, lsh, -32
        slw     frac_b_hi, frac_b_lo, lsh
        li      frac_b_lo, 0
        blr     #b       frac_b_ready_mul_div


L(b_nan_inf_mul_div):
        or.     temp1, frac_b_hi, frac_b_lo
        cmpi    cr5, 0, unpack_op, 0x0000                       # r12==0 in case of MUL, and !=0 in case of DIV
        bne-    cr0, L(return_b_mul_div)                           # return second argument, if it is NAN
        bne-    cr5, L(b_inf_div)
L(b_inf_mul):
        /*check for A=ZERO i.e. ZERO*INF case */
        beq-    cr6, L(return_nan_mul_div)
        b       L(return_b_mul_div)                                # return second argument, if it is INF
L(b_inf_div):
        /*check for A=INF i.e. INF/INF case */
        beq-    cr7, L(return_nan_mul_div)
        b       L(return_zero_mul_div)

L(return_a_mul_div):
        or      res_hi, frac_a_hi, sign_res
        /*a_lo & res_lo is the same*/
        rlwimi  res_hi, exp_a, 20, 1, 11
        bctr
        
L(return_b_mul_div):
        or      res_hi, frac_b_hi, sign_res
        mr      res_lo, b_lo
        rlwimi  res_hi, exp_b, 20, 1, 11
        bctr

L(return_zero_mul_div):
        mr      res_hi, sign_res
        li      res_lo, 0
        bctr

HIDDEN_FUNC(__spe_return_inf_mul_div)
L(return_inf_mul_div):
        oris    res_hi, sign_res, 0x7ff0
        li      res_lo, 0
        bctr
FUNC_END(__spe_return_inf_mul_div)

L(return_nan_mul_div):
        oris    res_hi, sign_res, 0x7fff
        li      res_lo, -1
        ori     res_hi, res_hi, 0xffff
        bctr
FUNC_END(__spe_unpackdf2_mul_div)
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

/*************************************************************
*	fast elf implementation of (2**n)*x function
*	IEEE compatible, no exeption flags setting
*	arguments:
*		r3,r4	-- double x
*		r5	-- int n
*	result:
*		r3,r4	-- y = (2**n)*x
*	uses:
*		registers:	r3,r4,r5,r6,r7
*		condional:	cr0,cr1
*************************************************************/
	.align  2

HIDDEN_FUNC(__spe_muldiv_scale2)
#define	hx r3
#define lx r4
#define n r5
#undef k
#define k r6
#define tmp r7
#define guard r5
#define sticky r7

	cmpwi	cr1,	hx,	0		# sign of x in cr1
	rlwinm.	k,	hx,	12,21,31	# exp of x to k
	bne-	cr0,	L(scale2_norm)

	clrlwi	hx,	hx,	1
	or.	tmp,	hx,	lx
	beqlr-	cr0				# quit if x = +-0

	cmpwi	cr0,	hx,	0
	bne+	cr0,	L(scale2_denorm_small_shift)

	cntlzw	k,	lx			# k = number of leading 0s
	cmpwi	cr0,	k,	11
	blt+	cr0,	L(scale2_denorm_middle_shift)

L(scale2_denorm_big_shift):
	addi	tmp,	k,	-11
	slw	hx,	lx,	tmp
	li	lx,	0
	addi	k,	k,	+20
	neg	k,	k
	b	L(scale2_norm)

L(scale2_denorm_middle_shift):
	li	tmp,	11
	sub	tmp,	tmp,	k		# tmp = 11 - k
	srw	hx,	lx,	tmp
	addi	k,	k,	+20
	slw	lx,	lx,	k
	neg	k,	k
	b	L(scale2_norm)

L(scale2_denorm_small_shift):
	cntlzw	k,	hx			# tmp = number of leading 0s
	addi	k,	k,	-11		# discard zeroes in exp
	slw	hx,	hx,	k
	li	tmp,	32
	sub	tmp,	tmp,	k		# tmp = 32 - k
	srw	tmp,	lx,	tmp
	or	hx,	hx,	tmp
	slw	lx,	lx,	k
	addi	k,	k,	-1
	neg	k,	k

L(scale2_norm):
	add.	k,	k,	n		# k = exp + n
	clrlwi	hx,	hx,	12

	bgt+	cr0,	L(scale2_positive)

	neg	k,	k
	oris	hx,	hx,	0x0010		# restore leading one

	cmpwi	cr0,	k,	53
	bgt-	cr0,	L(scale2_underflow)

	rlwinm	guard,	r0, 19, 31, 31  # g bit in guard
	rlwinm  sticky, r0, 20, 31, 31  # x bit in sticky

L(scale2_shift_loop):
	or	sticky,	sticky,	guard
	clrlwi	guard,	lx,	31
	srwi	lx,	lx,	1
	rlwimi	lx,	hx,	31,0,0
	srwi	hx,	hx,	1

	addic.	k,	k,	-1
	bge+	cr0,	L(scale2_shift_loop)

	cmpwi	cr0,	guard,	0
	beq-	cr0,	L(scale2_return_signed)	# if guard

	rlwimi.	sticky,	lx,	31,0,0
	beq-	cr0,	L(scale2_return_signed)

	addic	lx,	lx,	+1
	addze	hx,	hx

	b	L(scale2_return_signed)

L(scale2_positive):
	cmpwi	cr0,	k,	0x7ff		# if k >= 0x7ff then overflow
	bge-	cr0,	L(scale2_overflow)

	slwi	k,	k,	20
	or	hx,	hx,	k		# apply new exp
	b	L(scale2_return_signed)

L(scale2_underflow):
	lis	hx,	0			# form signed zero
	li	lx,	0
	b	L(scale2_return_signed)

L(scale2_overflow):
	lis	hx,	0x7ff0			# form signed Inf
	li	lx,	0
L(scale2_return_signed):
	xoris	n,	hx,	0x8000
	isel	hx,	n,	hx,	lt1
	blr
FUNC_END(__spe_muldiv_scale2)
#endif
