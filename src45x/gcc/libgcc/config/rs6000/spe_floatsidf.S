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
 *  fast implementation of SI2D         * 
 ****************************************/
        .align  2
FUNC_START(__floatsidf)
        andis.  sign_res, a_hi, 0x8000
        neg     temp, a
        cntlzw  cnt_lz, a
        cntlzw  temp1, temp
        isel    frac_res, a, temp, cr0_eq
        isel    cnt_lz, cnt_lz, temp1, cr0_eq

        cmpli   cr1, 0, frac_res, 0x0000
        cmpl    cr5, 0, frac_res, sign_res
        cmpli   cr6, 0, cnt_lz, +11
        subfic  exp_res, cnt_lz, 1023+31

        /*0x0000_0000 & 0x8000_0000 are processed by CR1 & CR5*/
        beq-    cr1, L(zero_si2d)
        beq-    cr5, L(min_si2d)
        blt-    cr6, L(right_shift_si2d)
        #TBD: can merge no_shift_... and left_shift_...
        beq-    cr6, L(no_shift_si2d)
        /*bgt-    cr6, left_shift_si2d*/

L(left_shift_si2d):
        addi    lsh, cnt_lz, -11
        rlwimi  sign_res, exp_res, 20, 1, 11
        slw     res_hi, frac_res, lsh
        li      res_lo, 0
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr

L(no_shift_si2d):
        rlwimi  frac_res, exp_res, 20, 1, 11
        li      res_lo, 0
        or      res_hi, frac_res, sign_res
        blr
        
L(right_shift_si2d):
        subfic  rsh, cnt_lz, +11
        addi    lsh, cnt_lz, 21                 # =32-rsh=32-11+cnt_lz
        rlwimi  sign_res, exp_res, 20, 1, 11
        srw     res_hi, frac_res, rsh
        slw     res_lo, frac_res, lsh
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr

L(zero_si2d):
        li      res_lo, 0
        mr      res_hi, sign_res
        blr
        
L(min_si2d):        
        #-0x80000000=-1.0 * 2^31=0xC1E0_0000_0000_0000
        li      res_lo, 0
        lis     res_hi, 0xc1e0
        blr
        
FUNC_END(__floatsidf)
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
 *   wrapper for SI2D                   * 
 ****************************************/
        .align  2
FUNC_START(__floatsidf)
        efdcfsi         r4, r3          #throws no exceptions
        evmergelohi     r3, r4, r4
        blr
        
FUNC_END(__floatsidf)
#endif
