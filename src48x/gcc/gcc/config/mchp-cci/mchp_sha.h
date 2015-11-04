/* Subroutines used for Microchip PIC32 MCU support
   Copyright (C) 2014  
   Microchip Technology, Inc.
   Contributed by S. Bekal, swaroopkumar.bekal@microchip.com

This file is part of Microchip XC compiler tool-chain.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
alng with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef MCHP_SHA_H
#define MCHP_SHA_H

/* SHA-256 digest of xclm executables for Win, Linux and OS X */
#if !defined(MCHP_XCLM_SHA256_DIGEST)
# if defined(__MINGW32__)
#  define MCHP_XCLM_SHA256_DIGEST   "2e162eef9b84179f6b5a6d3bcc3e8bfe603079c7daa70251297de54b07009fa3"
# elif defined(__linux__)
#  define MCHP_XCLM_SHA256_DIGEST   "e8e20a7ad3f03445172e6e61d9d6f6b48d64f220034309f47343e805f33c0157"
# elif defined(__MACH__)
#  define MCHP_XCLM_SHA256_DIGEST   "085a5ce305bf4c993d5d35eeef59bd1529127f1146c1ff06290886f36e54156c"
# else 
#  error "Unknown host"
# endif
#endif

/* Function to calculate the SHA256 digest of given file
 * Parameters :
 *     - Absolute path to file
 *     - Buffer to hold SHA256 digest 
 * Return value :
 *     - 0 on success, non-zero on any error
*/
int mchp_sha256( const char *path, unsigned char output[32] );

/* Function to validate if the SHA256 digest of given file
 * mathces the suppiled value
 * Parameters :
 *     - Absolute path to file
 *     - Pre-calculated SHA256 digest of the file (null terminated)
 * Return value :
 *     - 0 on match, non-zero on mis-match 
*/
int mchp_sha256_validate( const char *path, unsigned const char *sha2);

#endif 
