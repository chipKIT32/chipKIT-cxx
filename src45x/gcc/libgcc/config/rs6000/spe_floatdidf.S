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
 *  fast implementation of SLL2D        * 
 ****************************************/
        .align  2
FUNC_START(__floatdidf)
        subfic  temp1, a_lo, 0
        andis.  sign_res, a_hi, 0x8000
        subfze  temp2, a_hi 
        isel    frac_res_lo, a_lo, temp1, cr0_eq
        isel    frac_res_hi, a_hi, temp2, cr0_eq

        cntlzw  cnt_lz_lo, frac_res_lo
        cmpli   cr1, 0, frac_res_hi, 0x0000
        cntlzw  cnt_lz_hi, frac_res_hi
        or.     temp, frac_res_hi, frac_res_lo
        isel    cnt_lz_lo, 0, cnt_lz_lo, cr1_gt
        add     cnt_lz, cnt_lz_hi, cnt_lz_lo
        beq-    cr0, L(zero_sll2d)
        
        cmpli   cr1, 0, frac_res_lo, 0
        cmpl    cr5, 0, frac_res_hi, sign_res
        cmpli   cr6, 0, cnt_lz, +11
        cmpli   cr7, 0, cnt_lz, +43
        crand   cr5_eq, cr5_eq, cr1_eq
        subfic  exp_res, cnt_lz, 1023+63
        
        beq-    cr5, L(min_sll2d)
        blt-    cr6, L(right_shift_sll2d)
        beq-    cr6, L(no_shift_sll2d)
        bge-    cr7, L(left_big_shift_sll2d)       #merge left_big_... & left_32_...
        /*
        bgt-    cr7, L(left_big_shift_sll2d)
        beq-    cr7, L(left_32_shift_sll2d)
        */
        /*blt-    cr7, left_small_shift_sll2d*/

L(left_small_shift_sll2d):
        addi    lsh, cnt_lz, -11
        subfic  rsh, cnt_lz, +(11+32)           # =32-lsh=32-cnt_lz+11
        rlwimi  sign_res, exp_res, 20, 1, 11
        slw     res_hi, frac_res_hi, lsh
        srw     temp, frac_res_lo, rsh
        slw     res_lo, frac_res_lo, lsh
        or      res_hi, res_hi, temp
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr
/*
L(left_32_shift_sll2d):
        rlwimi  sign_res, exp_res, 20, 1, 11
        mr      res_hi, frac_res_lo
        li      res_lo, 0
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr
*/
L(left_big_shift_sll2d):
        addi    lsh, cnt_lz, -(11+32)
        rlwimi  sign_res, exp_res, 20, 1, 11
        slw     res_hi, frac_res_lo, lsh
        li      res_lo, 0
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr
/*        
L(no_shift_sll2d):        
        or      res_hi, frac_res_hi, sign_res   #frac_res_hi and res_hi is the same
        rlwimi  res_hi, exp_res, 20, 1, 11
        blr
*/
L(right_shift_sll2d):
        addi    lsh, cnt_lz, +21                #=32-rsh=32-11+cnt_lz
        subfic  rsh, cnt_lz, +11
        slw.    round_number, frac_res_lo, lsh
        srw     frac_res_lo, frac_res_lo, rsh
        slw     temp, frac_res_hi, lsh
        srw     res_hi, frac_res_hi, rsh
        or      res_lo, frac_res_lo, temp
        bge+    cr0, L(no_round_sll2d)
L(round_sll2d):
        rlwimi. round_number, frac_res_lo, 31, 0, 0
        addic   temp2, res_lo, +1
        addi    temp4, exp_res, +1
        addze   temp1, res_hi
        beq-    cr0, L(no_round_sll2d)
        
        andis.  temp, temp1, 2 << (20-16)
        /* if rounding occurred then frac=0x20_0000, i.e. it is ZERO -> no need to shift it back !!! */
        mr      res_hi, temp1
        isel    exp_res, exp_res, temp4, cr0_eq
        mr      res_lo, temp2
        /*b       no_round_sll2d*/
L(no_round_sll2d):
L(no_shift_sll2d):        
        or      res_hi, res_hi, sign_res
        rlwimi  res_hi, exp_res, 20, 1, 11
        blr
        
L(zero_sll2d):
        li      res_lo, 0
        mr      res_hi, sign_res
        blr

L(min_sll2d):
        #-0x80000000_00000000=-1.0*2^63=0xC3E0_0000_0000_0000
        li      res_lo, 0
        lis     res_hi, 0xc3e0
        blr

FUNC_END(__floatdidf)
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
 *   wrapper for SLL2D                  * 
 ****************************************/
        .align  2
FUNC_START(__floatdidf)
        subfic          r6, r4, 0
        rlwinm.         r7, r3, 0, 0, 0 #remember sign in CR0 and r7
        subfze          r5, r3
        lis             r10, (0x3ff+32)<<(52-32-16)
        isel            r4, r6, r4, cr0_lt
        isel            r3, r5, r3, cr0_lt
        li              r9, 0
        efdcfui         r11, r3         #=sll_hi
        evmergelo       r10, r10, r9    #=2^32
        efdcfui         r12, r4         #=sll_lo

        efdmul          r9, r11, r10    #=sll_hi * (2^32)
        efdadd          r4, r12, r9     #=(sll_hi * (2^32)) + sll_lo
        evmergelohi     r3, r4, r4
        rlwimi          r3, r7, 0, 0, 0 #set sign back
        blr

FUNC_END(__floatdidf)
#endif
