#ifndef _C30_FLAG_DEFS_H
#define _C30_FLAG_DEFS_H

/*
 *  Copyright 2009 Microchip Technology Inc
 */

/*
 *  This file is shared with the compiler sources and resource input file 
 *  Comments and pre-processor statements only!
 */

/*
** Family-specific flags
*/
#define P30F     1<<0
#define P30FSMPS 1<<1
#define P33F     1<<2
#define P24F     1<<3
#define P24H     1<<4
#define P24FK    1<<5
#define P33E     1<<6
#define P24E     1<<7
#define P24FV    P24FK | HAS_5VOLTS

#define FAMILY_MASK (0xFF)
#define CODEGUARD_MASK (0x00FFFF00)

/*
** Flags that identify record types
*/
#define IS_CODEGUARD_ID 0x10000000
#define IS_VECTOR_ID    0x20000000
#define IS_DEVICE_ID    0x40000000
#define IS_MEM_ID       0x80000000

/*
** Flags that are specific to record types
*/

/* IS_DEVICE_ID flags */
#define HAS_DSP    1<<8
#define HAS_EEDATA 1<<9
#define HAS_DMA    1<<10
#define HAS_CODEGUARD 1<<11
#define HAS_PMP    1<<12
#define HAS_EDS    1<<13
#define HAS_PMPV2  1<<14
#define HAS_ECORE  1<<15
#define HAS_DMAV2  1<<16
#define HAS_AUXFLASH 1<<17
#define HAS_5VOLTS 1 << 18
#define HAS_GIE    1 << 19

/* IS_CODEGUARD_ID flags */
#define FLASH         1<<8
#define RAM           1<<9
#define EEPROM        1<<10
#define NONE          1<<11
#define SMALL         1<<12
#define MEDIUM        1<<13
#define LARGE         1<<14
#define BOOT          1<<15
#define SECURE        1<<16
#define GENERAL       1<<17
#define CODE_PROTECT  1<<18
#define WRITE_PROTECT 1<<19
#define STANDARD      1<<20
#define HIGH          1<<21
#define ON            1<<22
#define OFF           1<<23
#define ENHANCED      1<<24
#define CONFIG        1<<25

#ifndef PATH_SEPARATOR_STR
#define PATH_SEPARATOR_STR "|"
#endif
#ifndef DIR_SEPARATOR_STR
#define DIR_SEPARATOR_STR "/"
#endif

/*
 * assembly include paths for each device family
 *   Multiple paths may be separated by colons
 *   '/' is converted to the correct directory separator for the OS
 */
#define MPLABC30_PIC30F_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC30F" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC33E_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC33E" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC33F_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC33F" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC33E_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC33E" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC24H_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24H" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC24E_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24E" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC24F_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24F" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
#define MPLABC30_PIC24E_INC_PATH ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24E" DIR_SEPARATOR_STR \
                                 "inc" \
                                 PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "inc"
/*
 * compiler include paths for each device family
 *   Multiple paths may be separated by colons
 *   '/' is converted to the correct directory separator for the OS
 */
#define MPLABC30_COMMON_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "include" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "generic" DIR_SEPARATOR_STR \
                                     "h"
#define MPLABC30_LEGACY_COMMON_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "include" DIR_SEPARATOR_STR "lega-c" \
                                     PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "generic" DIR_SEPARATOR_STR \
                                     "h"
#define MPLABC30_PIC24F_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "PIC24F" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_24F"
#define MPLABC30_PIC24H_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "PIC24H" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_30F_24H_33F"
#define MPLABC30_PIC24E_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "PIC24E" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_30F_24H_33F"
#define MPLABC30_PIC30F_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "dsPIC30F" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_30F_24H_33F"
#define MPLABC30_PIC33F_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "dsPIC33F" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_30F_24H_33F"
#define MPLABC30_PIC33E_INCLUDE_PATH ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "dsPIC33E" DIR_SEPARATOR_STR \
                                     "h" PATH_SEPARATOR_STR \
                                     ".." DIR_SEPARATOR_STR \
                                     "support" DIR_SEPARATOR_STR \
                                     "peripheral_30F_24H_33F"
/*
 * linker library paths for each device family
 *   Multiple paths may be separated by colons
 *   '/' is converted to the correct directory separator for the OS
 */
#define MPLABC30_COMMON_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "gld"
#define MPLABC30_ERRATA_COMMON_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "errata-lib" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "generic" DIR_SEPARATOR_STR \
                                 "gld"
#define MPLABC30_PIC24F_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "PIC24F" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24F" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_24F"
#define MPLABC30_PIC24H_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "PIC24H" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24H" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_30F_24H_33F"
#define MPLABC30_PIC24E_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "PIC24E" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "PIC24E" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_30F_24H_33F"
#define MPLABC30_PIC30F_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "dsPIC30F" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC30F" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_30F_24H_33F"
#define MPLABC30_PIC33F_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "dsPIC33F" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC33F" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_30F_24H_33F"
#define MPLABC30_PIC33E_LIB_PATH ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "dsPIC33E" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "support" DIR_SEPARATOR_STR \
                                 "dsPIC33E" DIR_SEPARATOR_STR \
                                 "gld" PATH_SEPARATOR_STR \
                                 ".." DIR_SEPARATOR_STR \
                                 "lib" DIR_SEPARATOR_STR \
                                 "peripheral_30F_24H_33F"


#endif
