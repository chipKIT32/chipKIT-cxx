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
 *  fast implementation of F2D          * 
 ****************************************/
        .align  2
FUNC_START(__extendsfdf2)
        rlwinm. exp_res, a, (32-23), 24, 31     #check for SP ZERO,DENORM
        rlwinm  frac_a, a, 0, 9, 31             
        rlwinm  sign_res, a, 0, 0, 0
        rlwinm  res_lo, a, (32-3), 0, 2
        cmpli   cr5, 0, frac_a, 0x0000          #check fraction for == 0
        rlwinm  frac_res_hi, a, (32-3), 12, 31  #SP frac -> DP frac_hi/frac_lo
        beq-    cr0, L(zero_denorm_f2d)

        cmpli   cr1, 0, exp_res, 0xff           #check for SP INF,NAN
        addi    exp_res, exp_res, (-127+1023)   #change bias
        li      temp, 0x07ff                    #exponent for DP INF,NAN 
        or      res_hi, frac_res_hi, sign_res
        isel    exp_res, exp_res, temp, cr1_lt
        rlwimi  res_hi, exp_res, 20, 1, 11
        blr

L(zero_denorm_f2d):
        cntlzw  cnt_lz, frac_a
        beq+    cr5, L(zero_f2d)
L(denorm_f2d):        
        addi    lsh, cnt_lz, -8
        subfic  rsh, cnt_lz, 40                 # =32-lsh=32-cnt_lz+8
        subfic  exp_res, cnt_lz, (1+1023-127+8) # =1+(1023-127)-lsh=(1+1023-127+8)-cnt_lz
        slw     frac_res_hi, frac_res_hi, lsh
        srw     temp, frac_res_lo, rsh
        rlwimi  sign_res, exp_res, 20, 1, 11
        or      frac_res_hi, frac_res_hi, temp
        slw     res_lo, frac_res_lo, lsh
        rlwimi  res_hi, sign_res, 0, 0, 11
        blr
L(zero_f2d):        
        li      res_lo, 0
        mr      res_hi, sign_res
        blr
        
FUNC_END(__extendsfdf2)
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
 *  fast implementation of F2D          * 
 ****************************************/
        .align  2
        
FUNC_START(__extendsfdf2)
    efdcfs  temp,   r3
    mfspr   r0, SPEFSCR
    andi.   r0, r0, FINV
    mr      temp1,  r3
    evmergelohi r3, r4, temp
    mr r4, temp         
    beqlr+  cr0
    #### exception ####    
    rlwinm. ea, temp1,  (32-23),    24, 31
    rlwinm  fa, temp1,  0,  9,  31
    rlwinm  sa, temp1,  0,  0,  0
    beq+    cr0,    L(f2d_denorm)
    
    or.     fa, fa, fa
    beq+    cr0,    L(f2d_return_inf)
    lis	    r3,	0x7fff
	li	    r4,	0
    blr
    
L(f2d_return_inf):
    lis     r3, 0x7FF0
    li      r4, 0x0
    or      r3, r3, sa
    blr
    
L(f2d_denorm):
    rlwinm  fla, temp1, (32-3), 0, 2
    rlwinm  fha, temp1, (32-3), 12, 31      #SP frac -> DP frac_hi/frac_lo
       
    cntlzw  cnt_lz, fa 
    addi    lsh, cnt_lz, -8
    subfic  rsh, cnt_lz, 40             # =32-lsh=32-cnt_lz+8
    subfic  ea, cnt_lz, (1+1023-127+8)  # =1+(1023-127)-lsh=(1+1023-127+8)-cnt_lz
    slw     fha, fha, lsh
    srw     temp, fla, rsh
    rlwimi  sa, ea, 20, 1, 11           # It seems that all is correct
    or      fha, fha, temp
    slw     r4, fla, lsh
    rlwimi  r3, sa, 0, 0, 11
    blr

FUNC_END(__extendsfdf2)
#endif
