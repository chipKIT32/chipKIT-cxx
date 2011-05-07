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
FUNC_START(__adddf3)
        /*FIND WHAT ARGUMENT IS BIGGER: |A| (FIRST) or |B| (SECOND)*/
        rlwinm  r11, a_hi, 0, 1, 31
        rlwinm  r12, b_hi, 0, 1, 31
        cmpl    cr0, 0, r11, r12
        cmpl    cr1, 0, a_lo, b_lo
        bgt+    cr0, L(a_bigger_add_sub) 
        blt-    cr0, L(b_bigger_add_sub) 
        bge+    cr1, L(a_bigger_add_sub) 
        /*blt-    cr1, b_bigger_add_sub*/

L(b_bigger_add_sub): 
        /* change "A" and "B" arguments: (r3,r4) <-> (r5,r6) to make |A|>|B| */
        mr      r7, a_hi
        mr      r8, a_lo
        mr      a_hi, b_hi
        mr      a_lo, b_lo
        mr      b_hi, r7
        mr      b_lo, r8
        
L(a_bigger_add_sub): 
        /* 
        NOW |A|>=|B| 
        Below are all possible cases for arguments:
        NAN_A >= NAN_B
        NAN_A > (INF_B, NORM_B, DENORM_B, ZERO_B)
        INF_A = INF_B
        INF_A > (NORM_B, DENORM_B, ZERO_B)
        NORM_A >= NORM_B
        NORM_A > (DENORM_B, ZERO_B)
        DENORM_A >= DENORM_B
        DENORM_A > ZERO_B
        ZERO_A = ZERO_B
        */
        
        /*
        Now argument fractions are shifted left by 8, and have format "1.(20+8).(24+8x)".
        this gives a possibility to work with exponent differencies = 52,53,54. 
        This works well for all cases except subtraction with exponent difference = 54.
        Here during equalization "B" is shifted right WITHOUT rounding.
        Result is obtained in "1or2.(20+8).(24+8x)" format (or smaller if we have subtraction). 
        Rounding scheme also works with this format.  
        For subtraction with exponent difference = 54 there is a special patch.
        */

        /* UNPACK FIRST ARGUMENT*/
        rlwinm  sign_a, a_hi, 0, 0, 0
        rlwinm. exp_a, a_hi, (32-exp_shift), 21, 31
        rlwinm  frac_a_hi, a_hi, frac_shift, (12-frac_shift), (31-frac_shift)
        cmpi    cr5, 0, exp_a, 2047
        rlwimi  frac_a_hi, a_lo, frac_shift, (32-frac_shift), 31
        rlwinm  frac_a_lo, a_lo, frac_shift, 0, (31-frac_shift)
        /*now we have cr0=(exp<=>0), cr5=(exp<=>2047)*/
        
        ble-    cr0, L(a_zero_denorm_add_sub)
        bge-    cr5, L(a_nan_inf_add_sub) 
        oris    frac_a_hi, frac_a_hi, 0x0010<<frac_shift                # add leading 1.0 to fraction
        
L(frac_a_ready_add_sub):
        /*CONTINUE*/
        
        /* UNPACK SECOND ARGUMENT*/
        rlwinm  sign_b, b_hi, 0, 0, 0
        rlwinm. exp_b, b_hi, (32-exp_shift), 21, 31
        rlwinm  frac_b_hi, b_hi, frac_shift, (12-frac_shift), (31-frac_shift)
        rlwimi  frac_b_hi, b_lo, frac_shift, (32-frac_shift), 31
        rlwinm  frac_b_lo, b_lo, frac_shift, 0, (31-frac_shift)
        /*now we have cr0=(exp<=>0)*/
        
        ble-    cr0, L(b_zero_denorm_add_sub)
        oris    frac_b_hi, frac_b_hi, 0x0010<<frac_shift                # add leading 1.0 to fraction
        
L(frac_b_ready_add_sub):
        /*CONTINUE*/

L(__do_add_sub):
L(equalize_add_sub):
        /*FIND DIFFERENCE IN "A" & "B" EXPONENTS*/
        /*check that exp_a-exp_b <= 54*/
        subf.   rsh, exp_b, exp_a
        cmpl    cr7, 0, sign_a, sign_b
        beq-    cr0, L(perform_add_sub)
        cmpi    cr1, 0, rsh, +54
        cmpi    cr5, 0, rsh, +32
        bgt-    cr1, L(return_a_add_sub)
        beq-    cr5, L(frac_b_32_shift_add_sub)
        bgt-    cr5, L(frac_b_big_shift_add_sub)


        /*NOTE: starting from this we do not need exp_b & sign_b, so use r8 & r10 as temporary variable*/

L(frac_b_small_shift_add_sub):
        subfic  lsh, rsh, +32
        li      temp2, +1
        slw.    temp1, frac_b_lo, lsh 
        srw     frac_b_lo, frac_b_lo, rsh
        slw     temp, frac_b_hi, lsh
        isel    temp2, 0, temp2, cr0_eq
        srw     frac_b_hi, frac_b_hi, rsh
        or      frac_b_lo, frac_b_lo, temp
        or      frac_b_lo, frac_b_lo, temp2     #put remainder (shifted bits of b_lo) to lsb of new b_lo
        b       L(perform_add_sub)

L(frac_b_32_shift_add_sub):
        cmpli   cr0, frac_b_lo, 0
        li      temp2, +1
        mr      frac_b_lo, frac_b_hi
        isel    temp2, 0, temp2, cr0_eq
        li      frac_b_hi, 0x0000
        or      frac_b_lo, frac_b_lo, temp2     #put remainder (b_lo) to lsb of new b_lo
        b       L(perform_add_sub)

L(frac_b_big_shift_add_sub):
        subfic  lsh, rsh, +64   # = 32 - new_rsh = 64 - old_rsh
        addi    rsh, rsh, -32
        slw     temp1, frac_b_hi, lsh 
        li      temp2, +1
        or.     temp1, temp1, frac_b_lo
        srw     frac_b_lo, frac_b_hi, rsh
        isel    temp2, 0, temp2, cr0_eq
        li      frac_b_hi, 0x0000
        or      frac_b_lo, frac_b_lo, temp2     #put remainder (whole b_lo + shifted bits of b_hi) to lsb of new b_lo
        /*b       perform_add_sub*/

L(perform_add_sub):
        /*merge add/sub in a single flow with isel */
        subfc   r12, frac_b_lo, frac_a_lo
        subfe   r11, frac_b_hi, frac_a_hi
        addc    frac_res_lo, frac_b_lo, frac_a_lo
        adde    frac_res_hi, frac_b_hi, frac_a_hi
        isel    frac_res_hi, frac_res_hi, r11, cr7_eq
        isel    frac_res_lo, frac_res_lo, r12, cr7_eq

L(__packf1_add_sub):
L(performed_add_sub):
        /*NOTE: starting from this we do not need "B" components, so use r5,r6,r8,r10 as temporary variables*/
        /* 
        return +ZERO regardless of argument signs ...
        "frac_res_hi" & "res_hi" is the same is already +ZERO
        "frac_res_lo" & "res_lo" is the same as is already zero
        */
        or.     temp, frac_res_hi, frac_res_lo
        #beqlr-  cr0                                    #moved down ...

        /*COUNT LEADING ZEROS in r5 !
        If (cnt == 3), this means that result still in "1.52.8" format and no fraction/exponent correction needed.
        If (cnt < 3), then it is either "2.52.8" or "3.52.8" format so we need to shift fraction right by (11-cnt), round it and increase exponent by (3-cnt).
        if (cnt > 3 & cnt < 11), then we need to shift fraction right by (11-cnt) without rounding and decrease exponent by (cnt-3).
        if (cnt == 11), then we need to decrease exponent by (cnt-3) without fraction changes.
        if (cnt > 11), then we need to shift fraction left by (cnt-11) and decrease exponent by (cnt-3).

        Rounding: if overflow occurred then frac=0x20_0000<<some_shift, i.e. it is ZERO -> no need to shift it back
        */
        /*
        cnt_lz = (frac_res_hi) ? cnt_lz_hi : (cnt_lz_lo+32)
        rsh = 8+(3-cnt_lz)+(1-exp_res)=12-exp_res-cntlz
        exp_res += (3-cnt_lz)
        
        if rsh == 0 -> no shift
        if rsh>32 -> big_right_shift & round
        if rsh>0 -> small_right_shift & round
        if rsh<-32 -> big_left_shift
        if rsh<0 -> small_left_shift
        lsh = -rsh
        */
        cntlzw  cnt_lz_hi, frac_res_hi
        cntlzw  cnt_lz_lo, frac_res_lo
        beqlr-  cr0                                     #moved from above ...
        cmpi    cr5, 0, cnt_lz_hi, 32
        addi    cnt_lz_lo, cnt_lz_lo, +32
        isel    cnt_lz, cnt_lz_hi, cnt_lz_lo, cr5_lt    # cnt = (cnt_hi<32) ? cnt_hi : (cnt_lo+32)
        subfic  temp1, cnt_lz, lz_normal                # exp_delta = 3-cnt
        subfic  rsh, cnt_lz, (lz_normal+frac_shift)     # = 8+exp_delta = 11-cnt
        addi    lsh, cnt_lz, -(lz_normal+frac_shift)    # = -(8+exp_delta) = cnt-11
        add.    exp_res, exp_res, temp1                 # exp_res += exp_delta += 3-cnt
        subfic  temp2, exp_res, 1
        isel    temp2, 0, temp2, cr0_gt
        subf.   lsh, temp2, lsh                         # lsh -= (exp_res>0) ? 0 : (1-exp_res)
        add     rsh, temp2, rsh                         # rsh += (exp_res>0) ? 0 : (1-exp_res)
        cmpi    cr5, 0, lsh, -32 
        cmpi    cr1, 0, lsh, +32
        blt-    cr5, L(res_big_shift_right_add_sub)
        beq-    cr5, L(res_32_shift_right_add_sub)
        blt+    cr0, L(res_small_shift_right_add_sub)
        bgt-    cr1, L(res_big_shift_left_add_sub)
        beq-    cr1, L(res_32_shift_left_add_sub)
        bgt+    cr0, L(res_small_shift_left_add_sub)
        #beq-    cr0, res_no_shift_add_sub

L(res_no_shift_add_sub):
        /*correct exponent*/
        cmpi    cr6, 0, exp_res, 0                      
        cmpi    cr7, 0, exp_res, 2047
        isel    exp_res, 0, exp_res, cr6_lt
        bge-    cr7, L(return_inf_add_sub)
        /*pack & return*/
        rlwimi  res_hi, sign_res, 0, 0, 0
        rlwimi  res_hi, exp_res, exp_shift, 1, 11
        blr

L(res_small_shift_left_add_sub):                           #works for lsh=0..31
        subfic  rsh, lsh, +32
        slw     frac_res_hi, frac_res_hi, lsh
        srw     temp, frac_res_lo, rsh
        slw     res_lo, frac_res_lo, lsh
        or      res_hi, temp, frac_res_hi
        b       L(res_no_shift_add_sub)
        
L(res_32_shift_left_add_sub):                              #works for lsh=32
        mr      res_hi, frac_res_lo
        li      res_lo, 0
        b       L(res_no_shift_add_sub)
                
L(res_big_shift_left_add_sub):                             #works for lsh>=32 (32 is included) !!!
        addi    lsh, lsh, -32
        slw     res_hi, frac_res_lo, lsh
        li      res_lo, 0
        b       L(res_no_shift_add_sub)

L(res_small_shift_right_add_sub):                          #works for rsh=0..31
        subfic  lsh, rsh, +32                           #can be calculated as rsh=0-old_lsh, new_lsh=32+old_lsh
        slw.    round_number, frac_res_lo, lsh
        srw     frac_res_lo, frac_res_lo, rsh
        slw     temp, frac_res_hi, lsh
        srw     res_hi, frac_res_hi, rsh
        or      res_lo, temp, frac_res_lo
L(res_small_round_add_sub):
        bge+    cr0, L(res_no_shift_add_sub)
        addic   temp2, res_lo, +1
        rlwimi. round_number, res_lo, 31, 0, 0
        addze   temp1, res_hi
        addi    temp3, exp_res, +1
        isel	res_lo, res_lo, temp2, cr0_eq
        isel	res_hi, res_hi, temp1, cr0_eq
        andis.  temp, res_hi, 0x0020                    # in case of overflow fraction = 0x20_0000_0000_0000, i.e. it is ZERO->no need to shift it back
        isel    exp_res, exp_res, temp3, cr0_eq
        b       L(res_no_shift_add_sub)
        
L(res_32_shift_right_add_sub):                             #works for rsh=32
        or.     round_number, frac_res_lo, frac_res_lo
        mr      res_lo, frac_res_hi
        li      res_hi, 0
L(res_32_round_add_sub):
        bge+    cr0, L(res_no_shift_add_sub)
        addic   temp2, res_lo, +1
        rlwimi. round_number, res_lo, 31, 0, 0
        addze   temp1, res_hi
        isel	res_lo, res_lo, temp2, cr0_eq
        isel	res_hi, res_hi, temp1, cr0_eq
        b       L(res_no_shift_add_sub)

L(res_big_shift_right_add_sub):                            #works for rsh>32 (32 is NOT included) !!!
        subfic  lsh, rsh, +64
        addi    rsh, rsh, -32
        slw.    round_num_hi, frac_res_hi, lsh
        mr      round_num_lo, frac_res_lo
        srw     res_lo, frac_res_hi, rsh
        li      res_hi, 0
L(res_big_round_add_sub):
        bge+    cr0, L(res_no_shift_add_sub)
        rlwimi  round_num_hi, res_lo, 31, 0, 0
        addi    temp2, res_lo, +1
        or.     round_number, round_num_hi, round_num_lo      
        isel	res_lo, res_lo, temp2, cr0_eq
        b       L(res_no_shift_add_sub)
/*
L(res_big_shift_right_add_sub):                            #works for rsh>=32 (32 is included) !!!
        subfic  lsh, rsh, +64
        addi    rsh, rsh, -32
        slw.    round_num_hi, frac_res_hi, lsh
        mr      round_num_lo, frac_res_lo
        srw     res_lo, frac_res_hi, rsh
        li      res_hi, 0
L(res_big_round_add_sub):
        bge+    cr0, L(res_no_shift_add_sub)
        rlwimi  round_num_hi, res_lo, 31, 0, 0
        addic   temp2, res_lo, +1
        or.     round_number, round_num_hi, round_num_lo      
        addze   temp1, res_hi
        isel	res_lo, res_lo, temp2, cr0_eq
        isel	res_hi, res_hi, temp1, cr0_eq
        b       L(res_no_shift_add_sub)
*/
        /*
        we have "2.52.8" i.e. "1.53.8" fraction, so 
        PERFORM "ROUND-TO-NEAREST" by 
        a) addition of 1<<(rsh-1)
        b) optional subtraction of 1 (the only purpose of this is to round 0.5->0.4999...->0.0 instead of 0.5->1.0)
        
        rounding algorithm:
        0.0x + (0.10...00 - 0.00...01) -> 00. (no round)
        0.10 + (0.10...00 - 0.00...01) -> 00. (no round)
        0.1x + (0.10...00 - 0.00...01) -> 01. (+1.0)
        1.0x + 0.10...00 -> 01.               (no round)
        1.1x + 0.10...00 -> 10.               (+1.0)
        So, 
        a). form rounding const= 0.10...00
        b). check least significant bit of frac: frac & 1.0
        c). if zero, then rounding_const-= 0.00...01 = 0.01...11 else subtract 0.00...00
        d). add rounding_contant to fraction, shift/truncate result
        
        another method is good when remainder occupies whole register
        a). if (signed_remainder>=0) (i.e. unsigned_remainder< 0x8000_0000) -> no round, goto code after ... (case 1 & 4 of the algorithm)
        b). pack least significant bit into msb of remainder with CR update
        c). if new remainder is zero ->  no round, goto code after ... (case 2 of the algorithm)
        d). add +1 to fraction, shift/truncate result
        */

L(a_zero_denorm_add_sub):
        or.     temp, frac_a_hi, frac_a_lo
        /*
        if A == ZERO, then B == ZERO also (abs(B)<=abs(A)), so return ZERO, 
        if at least one of arguments is +ZERO, return +ZERO, else return -ZERO
        "res_hi" & "frac_a_hi" is the same and is already +ZERO  
        "res_lo" & "frac_a_lo" is the same and is already zero  
        */
        bne-    cr0, L(a_denorm_add_sub)
L(a_zero_add_sub):
        and     res_hi, sign_a, b_hi        
        blr
L(a_denorm_add_sub):
        /* 
        DENORM is 0.something * 2^(-1022), 
        i.e. biased exp = 1 but not 0 as it is stored !
        */
        li      exp_a, 1                        # real exp_a value
        b       L(frac_a_ready_add_sub)            # treat DENORM as usual number

L(a_nan_inf_add_sub):
        or.     temp, frac_a_hi, frac_a_lo
        bne-    cr0, L(return_a_add_sub)           # return first argument if it is NAN

L(a_inf_add_sub):
        /*
        check for (+INF) + (-INF) case which results in NAN, 
        otherwise return INF (first argument) 
        */
        xoris   temp, sign_a, 0xfff0            # construct "-a" in {r0,0}, assumed that a==+-INF
        cmpli   cr1, 0, b_lo, 0x0000 
        cmpl    cr0, 0, b_hi, temp
        bne+    cr1, L(return_inf_add_sub)         #return_a_add_sub
        bne+    cr0, L(return_inf_add_sub)         #return_a_add_sub
        /*return generated (-NAN) as sum of 2 opposite INFs*/
L(return_nan_add_sub):        
        li      res_hi, -1
        li      res_lo, -1
        blr

L(b_zero_denorm_add_sub):
        or.     temp, frac_b_hi, frac_b_lo
        bne-    cr0, L(b_denorm_add_sub)

L(b_zero_add_sub):
        /*check for A = DENORM and correct its exponent back: 1->0*/        
        andis.  temp, frac_a_hi, 0x0010<<frac_shift     # check for leading 1.0 in frac_a
        isel    exp_a, 0, exp_a, cr0_eq
        #b       return_a_add_sub                # return first argument if second is ZERO
L(return_a_add_sub):
        rlwinm  frac_res_lo, frac_a_lo, (32-frac_shift), frac_shift, 31
        rlwimi  frac_res_lo, frac_a_hi, (32-frac_shift), 0, (frac_shift-1)
        rlwinm  frac_res_hi, frac_a_hi, (32-frac_shift), 12, 31
        rlwimi  res_hi, exp_a, exp_shift, 1, 11
        or      res_hi, sign_a, frac_a_hi
        blr

L(b_denorm_add_sub):
        /*
        DENORM is 0.something * 2^(-1022), 
        i.e. biased exp = 1 but not 0 as it is stored !
        */
        li      exp_b, 1                        # real exp_b value
        b       L(frac_b_ready_add_sub)            # treat DENORM as usual number

L(return_inf_add_sub):        
        /*sign_res & sign_a is the same!*/ 
        oris    res_hi, sign_res, 0x7ff0
        li      res_lo, 0x0000
        blr
FUNC_END(__adddf3)
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

/***************************************************************
*	fast implementation of addition IEEE compatible,
*       no exeption flags setting
***************************************************************/
	.align  2

FUNC_START(__adddf3)
	evmergelo	a,	r3,r4		# merge a
	evmergelo	b_,	r5,r6		# merge b

	efdadd		c,	a,	b_	# c = a+b -- addition in person

	mfspr		r0,	SPEFSCR
	andi.		r9, r0,	XPTN		# mask all exeptions

	evmergelohi	r3,	r4,	c	# split result
	beqlr+		cr0			# end exit if no exeptions
	b	__spe_add_exception@local
FUNC_END(__adddf3)

#endif
