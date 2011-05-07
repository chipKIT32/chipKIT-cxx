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
 *  fast implementation of D2SLL        * 
 ****************************************/
        .align  2
FUNC_START(__fixdfdi)
        rlwinm. sign_a, a_hi, 0, 0, 0
        rlwinm  exp_a, a_hi, (32-20), 21, 31
        oris    a_hi, a_hi, 0x0010              # add leading 1.0

        cmpli   cr7, 0, exp_a, 1023             #if number is too small (<2^0), DENORM or ZERO
        cmpli   cr6, 0, exp_a, (1023+63)        #if the number is too big (>=2^63, INF or NAN)
        cmpli   cr1, 0, exp_a, (1023+20)        #if frac is a small integer
        cmpli   cr5, 0, exp_a, (1023+52)        #if frac is a ready integer
        rlwinm  frac_a_hi, a_hi, 0, 11, 31

        blt-    cr7, L(return_zero_d2sll)
HIDDEN_FUNC(__d2ull_entry_to_d2sll)
        bge-    cr6, L(big_d2sll)                  #return_max_d2sll
        ble-    cr1, L(right_big_shift_d2sll)
        bgt-    cr5, L(left_shift_d2sll)
        beq-    cr5, L(no_shift_d2sll)
        /*blt-    cr5, right_small_shift_d2sll*/
        
L(right_small_shift_d2sll):
        subfic  rsh, exp_a, (1023+52)
        addi    lsh, exp_a, (32-1023-52)        #=32-rsh=32-(1023+52)+exp_a
        srw     res_lo, frac_a_lo, rsh
        slw     temp, frac_a_hi, lsh         
        srw     res_hi, frac_a_hi, rsh         
        or      res_lo, res_lo, temp
L(no_shift_d2sll):
        subfic  temp1, res_lo, 0
        subfze  temp2, res_hi
        isel    res_lo, res_lo, temp1, cr0_eq
        isel    res_hi, res_hi, temp2, cr0_eq
        blr

L(right_big_shift_d2sll):
        subfic  rsh, exp_a, (1023+20)
        srw     res_lo, frac_a_hi, rsh
        li      res_hi, 0

        neg     temp1, res_lo
        li      temp2, -1
        isel    res_lo, res_lo, temp1, cr0_eq
        isel    res_hi, res_hi, temp2, cr0_eq
        blr
/*
L(no_shift_d2sll):
        subfic  temp1, frac_a_lo, 0
        subfze  temp2, frac_a_hi
        isel    res_lo, frac_a_lo, temp1, cr0_eq
        isel    res_hi, frac_a_hi, temp2, cr0_eq
        blr
*/
L(left_shift_d2sll):
        addi    lsh, exp_a, -(1023+52)
        subfic  rsh, exp_a, (32+1023+52)        #=32-lsh=32+(1023+52)-exp_a
        slw     res_hi, frac_a_hi, lsh         
        srw     temp, frac_a_lo, rsh         
        slw     res_lo, frac_a_lo, lsh
        or      res_hi, res_hi, temp
        /*b       no_shift_d2sll*/
        subfic  temp1, res_lo, 0
        subfze  temp2, res_hi
        isel    res_lo, res_lo, temp1, cr0_eq
        isel    res_hi, res_hi, temp2, cr0_eq
        blr

L(return_zero_d2sll):
        li      res_hi, 0
        li      res_lo, 0
        blr

L(big_d2sll):
        cmpli   cr1, 0, exp_a, 2047             #if the number is NAN or INF
        rlwinm  frac_a, frac_a_hi, 0, 12, 31
        blt+    cr1, L(return_max_d2sll)
        or.     temp, a_lo, frac_a
        li      res_hi, 0
        li      res_lo, 0
        bnelr-  cr0                             #return ZERO if we have NAN
        cmpi    cr0, 0, sign_a, 0x0000          #continue & return +-MAX if we have INF
L(return_max_d2sll):
        #big negative values return -0x8000_0000_0000_0000 = -1.0 * 2^63, big bositive return +0x7fff_ffff_ffff_ffff
        li      res_lo, -1
        rlwinm  res_hi, res_lo, 0, 1, 31
        isel    res_lo, 0, res_lo, cr0_lt
        isel    res_hi, res_hi, sign_a, cr0_eq  #can't use cr0_lt because sign_a == r0
        blr

FUNC_END(__d2ull_entry_to_d2sll)        
FUNC_END(__fixdfdi)
