
#ifndef _XC32_FLAG_DEFS_H
#define _XC32_FLAG_DEFS_H

/*
 *  This file is shared with the compiler sources and resource input file 
 *  Comments and pre-processor statements only!
 */

/*
** Family-specific flags
*/
#define P32MX              (1<<0)
#define P32MZ              (1<<1)
#define P32SK              (1<<2)

#define FAMILY_MASK        (0xFF)

/*
** Flags that identify record types
 */
#define IS_DEVICE_ID       (1<<28)

/* IS_DEVICE_ID flags */
#define HAS_MIPS32R2       (1<<8)
#define HAS_MIPS16         (1<<9)
#define HAS_MICROMIPS      (1<<10)
#define HAS_DSPR2          (1<<11)
#define HAS_MCU            (1<<12)
#define HAS_FLTPT          (1<<13)
#define HAS_EBI            (1<<14)
#define HAS_SQI            (1<<15)
#define HAS_L1CACHE        (1<<16)
#define HAS_VECTOROFFSETS  (1<<17)

#define HAS_DEVICEID_MAX (1<< 27) /* Maximum value */

#ifndef PATH_SEPARATOR_STR
#define PATH_SEPARATOR_STR "|"
#endif
#ifndef DIR_SEPARATOR_STR
#define DIR_SEPARATOR_STR "/"
#endif

#endif

