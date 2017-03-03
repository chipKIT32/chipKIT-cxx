/*-------------------------------------------------------------------------
 * MPLAB XC Compiler -  Public XLCM constants
 *
 * This software is developed by Microchip Technology Inc. and its
 * subsidiaries ("Microchip").
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 * 1.      Redistributions of source code must retain the above copyright
 *         notice, this list of conditions and the following disclaimer.
 * 2.      Redistributions in binary form must reproduce the above 
 *         copyright notice, this list of conditions and the following 
 *         disclaimer in the documentation and/or other materials provided 
 *         with the distribution.
 * 3.      Microchip's name may not be used to endorse or promote products
 *         derived from this software without specific prior written 
 *         permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY MICROCHIP "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL MICROCHIP BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING BUT NOT LIMITED TO
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWSOEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *-------------------------------------------------------------------------*/

/* General XCLM return values */
#define MCHP_XCLM_NORMAL_EXIT            0x0

/* Original options used for C translation units */
#define MCHP_XCLM_FREE_LICENSE           0x0
#define MCHP_XCLM_VALID_STANDARD_LICENSE 0x1
#define MCHP_XCLM_VALID_PRO_LICENSE      0x2

#define MCHP_XCLM_DONOTUSE               0x3

/* New options used for C++ translation units */
#define MCHP_XCLM_NO_CPP_LICENSE         0x4
#define MCHP_XCLM_VALID_CPP_FREE         0x5
#define MCHP_XCLM_VALID_CPP_FULL         0x6

/* diagnose problems */
#define MCHP_XCLM_OPTION_ERROR		0x10	/* something bad in supplied options - refer to stderr for details */
#define MCHP_XCLM_INTERNAL_ERROR	0x11	/* internal error - memory, filesystem, network, ? */
#define MCHP_XCLM_LICENSE_ERROR		0x12	/* no license available, or license expired */


/* -liccheck */
#define MCHP_XCLM_NO_VALID_LICENSE	 0x1
#define MCHP_XCLM_SUBSCRIP_RENEWED	 0x2
#define MCHP_XCLM_LICENSE_EXPIRED	 0x3
#define MCHP_XCLM_LICENSE_RENEWED	 0x4
#define MCHP_XCLM_DONGLE_LICENSE	 0x5
