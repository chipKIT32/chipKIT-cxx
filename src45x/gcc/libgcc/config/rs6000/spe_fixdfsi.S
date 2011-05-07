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
 *  fast implementation of D2SI         * 
 ****************************************/
        .align  2
FUNC_START(__fixdfsi)
        rlwinm. sign_a, a_hi, 0, 0, 0
        rlwinm  exp_a, a_hi, (32-20), 21, 31
        oris    a_hi, a_hi, 0x0010              # add leading 1.0

        cmpli   cr7, 0, exp_a, 1023             #if number is too small (<2^0), DENORM or ZERO
        cmpli   cr1, 0, exp_a, 2047             #if number is INF or NAN
        cmpli   cr6, 0, exp_a, (1023+31)        #if number is too big (>=2^31)
        cmpli   cr5, 0, exp_a, (1023+20)        #if frac is a ready integer
        rlwinm  frac_a_hi, a_hi, 0, 11, 31

        blt-    cr7, L(return_zero_d2si)
HIDDEN_FUNC(__d2ui_entry_to_d2si)
        bge-    cr1, L(nan_inf_d2si)
        bge-    cr6, L(return_max_d2si)
        bgt-    cr5, L(left_shift_d2si)
        beq-    cr5, L(left_shift_d2si)
        /*blt-    cr5, right_shift_d2si*/

L(right_shift_d2si):
        subfic  rsh, exp_a, (1023+20)
        srw     res, frac_a_hi, rsh
L(no_shift_d2si):        
        neg     temp, res
        isel    res, res, temp, cr0_eq        
        blr
/*
L(no_shift_d2si):        
        neg     temp, frac_a_hi                 #"frac_a_hi" and "res" is the same
        isel    res, frac_a_hi, temp, cr0_eq        
        blr
*/
L(left_shift_d2si):
        addi    lsh, exp_a, -(1023+20)
        subfic  rsh, exp_a, (32+1023+20)        #=32-lsh=32+(1023+20)-exp_a
        slw     res, frac_a_hi, lsh         
        srw     temp1, frac_a_lo, rsh         
        or      res, res, temp1
        neg     temp, res
        isel    res, res, temp, cr0_eq        
        blr

L(return_zero_d2si):
        li      res, 0
        blr

L(nan_inf_d2si):
        rlwinm  frac_a, frac_a_hi, 0, 12, 31
        or.     temp, a_lo, frac_a
        li      res, 0
        bnelr-  cr0                             #return ZERO if we have NAN
        cmpi    cr0, 0, sign_a, 0x0000          #continue & return +-MAX if we have INF
L(return_max_d2si):
        #big negative values return -0x8000_0000 = -1.0 * 2^31, big positive return +0x7fff_ffff
        li      res, -1
        rlwinm  res, res, 0, 1, 31
        isel    res, res, sign_a, cr0_eq        #can't use cr0_lt because sign_a == r0
        blr
        
FUNC_END(__d2ui_entry_to_d2si)
FUNC_END(__fixdfsi)
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
 *   wrapper for D2SI                   * 
 ****************************************/
        .align  2
FUNC_START(__fixdfsi)
        evmergelo       r11, r3, r4
        efdctsiz        r3, r11         #throws FINV(A=DENORM,INF,NAN; A>max_int, A<min_int), FINXS
        blr
        
FUNC_END(__fixdfsi)
#endif
