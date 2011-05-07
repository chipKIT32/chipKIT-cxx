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
#*  Functions:    __extendsfdf2(_d_ftod), __truncdfsf2(_d_dtof),
#*
#*  Description:  implements floating point double precision
#*                software emulation for data type convrsion operations
#*                from float and to float
#*                for GNU (libgcc) & ABI runtime libraries 
#*                IEEE-compliant, no exception flags setting
#*
#*  Notes: 
#*
#******************************************************************************

#include <ppc-asm.h>


/* Local labels stripped out by the linker.  */
#undef L
#define L(x) .L##x

#define a_hi r3
#define a_lo r4
                
#define res_hi a_hi
#define res_lo a_lo
                
#define frac_a_hi a_hi
#define frac_a_lo a_lo
#define exp_a r7
#define sign_a r0
                
#define frac_res_hi res_hi
#define frac_res_lo res_lo
#define exp_res exp_a
#define sign_res sign_a

#define a r3
#define res r3
#define frac_a r5
#define frac_res r5
        
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
                
#define res_hi a_hi
#define res_lo a_lo
                
#define frac_a_hi a_hi
#define frac_a_lo a_lo
#define exp_a r7
#define sign_a r0
                
#define frac_res_hi res_hi
#define frac_res_lo res_lo
#define exp_res exp_a
#define sign_res sign_a
        
#define a r3
#define res r3
#define frac_a r5
#define frac_res r5
        

        #VARIABLES FOR FRACTION SHIFT
#define cnt_lz_hi r9
#define cnt_lz_lo r10
#define cnt_lz r9
#define exp_delta r10
        
#define lsh r11
#define rsh r12
                
        #VARIABLES FOR FRACTION ROUND
#define round_number r10

#define temp r6
#define temp1 r8
#define temp2 r9
#define temp3 r10
#define temp4 r11
#define temp5 r12

/****************************************
 *  fast implementation of D2F          * 
 ****************************************/
        .align  2
FUNC_START(__truncdfsf2)
        rlwinm  exp_res, a_hi, (32-20), 21, 31
        rlwinm  frac_res, a_lo, 3, 29, 31
        cmpli   cr1, 0, exp_res, 1023-127-23    #check for SP IN RANGE (DP NORM -> SP DENORM)
        cmpli   cr5, 0, exp_res, 2047           #check for DP NAN,INF
        cmpli   cr6, 0, exp_res, 1023-127       #check for SP LESS THAN (DP NORM -> SP NORM)
        cmpli   cr7, 0, exp_res, 1023+128       #check for SP GREATER THAN (DP NORM -> SP NORM)
        rlwinm. a_lo, a_lo, 3, 0, 28            #if we need rounding (remainder >= 0x8000_0000)
        rlwimi  frac_res, a_hi, 3, 9, 28
        rlwinm  res, a_hi, 0, 0, 0              #store sign in the result register (ready result for +-ZERO)

        bltlr-  cr1                             #DP ZEROs and DP DENORMs are converted to SP ZERO
        bge-	cr5, L(sp_nan_inf_d2f) 		#DP NAN/INF  to SP NAN/INF conversion
        ble-    cr6, L(sp_round_denorm_d2f)	#DP NORM to SP DENORM conversion
        bge-    cr7, L(sp_overflow_d2f)		#DP NORM tp SP INF conversion
        bge-    cr0, L(sp_norm_d2f)
        /*blt-    cr0, sp_round_norm_d2f*/

L(sp_round_norm_d2f):
        /*
        GNU documentation says that we need to truncate, 
        but GNU tests & ABI documentation says that we need to round
        */
        rlwimi. a_lo, frac_res, 31, 0, 0
        oris    frac_res, frac_res, 1 << (23-16)
        addi    temp2, exp_res, +1
        addi    temp1, frac_res, +1
        beq-    cr0, L(sp_norm_d2f)
        andis.  temp, temp1, 2 << (23-16)
        /* if rounding occurred then frac=0x20_0000, i.e. it is ZERO -> no need to shift it back !!!  */
        mr      frac_res, temp1
        isel    exp_res, exp_res, temp2, cr0_eq
        
L(sp_norm_d2f):        
        li      temp, 0x00ff                            #exp for SP INF (on overflow)
        addi    exp_res, exp_res, (-1023+127)           #change bias
        or      res, frac_res, res
        isel    exp_res, exp_res, temp, cr7_lt
        rlwimi  res, exp_res, 23, 1, 8
        blr

L(sp_round_denorm_d2f):
        oris    frac_res, frac_res, 1 << (23-16)
        addi    lsh, exp_res, 32-(1023-127+1)           # =32-rsh = 32-1+old_bias_exp-1023+127
        subfic  rsh, exp_res, (1023-127+1)              # =1-new_bias_exp = 1-old_bias_exp+1023-127
        li      temp1, 1

        slw.    temp, a_lo, lsh                         
        srw     a_lo, a_lo, rsh
        slw     temp2, frac_res, lsh
        isel    temp1, 0, temp1, cr0_eq
        or.     a_lo, a_lo, temp2                       #if we need rounding (remainder >= 0x8000_0000)
        srw     frac_res, frac_res, rsh
        or      round_number, a_lo, temp1
        bge-    cr0, L(sp_denorm_d2f)

        rlwimi. round_number, frac_res, 31, 0, 1
        addi    temp2, frac_res, +1
        isel    frac_res, frac_res, temp2, cr0_eq
        /*
        if DENORM->NORM during rounding then 
        leading "1.0" occupies 'exp" and shows that 
        exp == 1, i.e. that we have NORM !!!
        */

L(sp_denorm_d2f):
        rlwimi  res, frac_res, 0, 8, 31
        blr

L(sp_overflow_d2f):
	oris	res, res, 0x7f80 			#return (signed) SP INF
	blr
		
L(sp_nan_inf_d2f):
	lis   	temp, 0x0040
        oris    res, res, 0x7f80
        isel	temp, 0, temp, cr0_eq
        rlwimi	res, frac_res, 0, 9, 31	   		#changed exp, but preserved sign and fraction
        or		res, temp, res
        blr        

FUNC_END(__truncdfsf2)
#else
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
#define ha r3
#define la r4
                
#define fha ha
#define fla la
#define ea r7
#define sa r10

#define fa r5
                        
        #VARIABLES FOR FRACTION SHIFT
#define cnt_lz_hi r9
#define cnt_lz_lo r10
#define cnt_lz r9
        
#define lsh r11
#define rsh r12
                
#define temp r6
#define temp1 r8
#define temp2 r9
#define temp3 r10
#define temp4 r11
#define temp5 r12
        
#define SPEFSCR 512
    
#define FINXS 0x0020
#define XPTN 0x0f00
#define XF 0x1000
#define FINV 0x0800
#define FDBZ 0x0400
#define FUNF 0x0200
#define FOVF 0x0100
#define OVUN 0x0300

/****************************************
 *  fast implementation of D2F          * 
 ****************************************/
        .align  2

FUNC_START(__truncdfsf2)
    mfspr   r0, SPEFSCR
    andi.   temp2,   r0, XPTN
    beqlr+  cr0
   
    # invalid exception
  	evmergelohi     ha, la, temp
    rlwinm  sa, ha, 0,  0,  0
    andi.	temp,   r0,	FOVF
	bne-	cr0,	L(d2f_return_inf)
    
    rlwinm  ea, ha, (32-20),    21, 31
    andi.	temp,   r0,	FUNF
	bne-	cr0,	L(d2f_underflow)
    cmpwi	cr6,	ea, 0x0001
    bgt-    cr6,    L(d2f_naninf)
L(d2f_return_zero):
    mr      r3, sa
    blr
    
L(d2f_naninf):
    clrlwi	temp,	ha,	12
    or.     temp, temp, la
    li      r0, 0x0000
    beq+    cr0,    L(d2f_return_inf)
    lis     r3, 0x7fff
    blr       

L(d2f_underflow):                          # result is denormalized or zero
    cmpli   cr1, 0, ea, 1023-127-23     # if ea<1023-150 => res=0

    blt+    cr1, L(d2f_return_zero)
    rlwinm  temp,   la, 4,  28, 31      # remember about inexact bit  
    rlwimi  temp,   ha, 4,  8,  27      # now in temp 24 msb of fraction (with guard bit)
    oris    temp,   temp,   1 << ((23-16) + 1)
    clrlwi  temp,   temp,   7           # temp - normalized with minimal exp (x2 actually)
    clrlwi  temp2,  la, 4

    subfic  rsh,    ea, 1023 - 126 + 1
    eqv     temp1,  temp1,  temp1
    slw     temp1,  temp1,  rsh
    andc    r3,     temp,   temp1
    or.     temp2,  temp2,  r3          # cr0eq not set => inexact bit (FX true)

    subfic  r5, rsh,    33
    rlwnm  r5, temp,   r5, 31, 31      # FG
    srw     r3,     temp,   rsh
    beq-    cr0,    L(d2f_return)
    or.     r5,     r5,     r5
    beq-    cr0,    L(d2f_return)
    addi    r3, r3, 1

L(d2f_return):    
    or      r3,     r3,     sa
    blr

L(d2f_return_inf):
    oris    r3, sa,   0x7f80
    xor     r4, r4, r4
    blr

FUNC_END(__truncdfsf2)
#endif
