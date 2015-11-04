/*
 *  A new mode for C30
 */

/* 
    P24PSV represents a small (__psv__) pointer
    P24PROG represetns a large (__prog__) pointer
*/

TARGET_POINTER_MODE(P24PROG, 24, 4);  // __prog__ pointer
TARGET_POINTER_MODE(P24PSV,  24, 4);  // __psv__ pointer
TARGET_POINTER_MODE(P16APSV, 16, 2);  // const pointer in auto_psv model 
TARGET_POINTER_MODE(P16PMP,  16, 2);  // __pmp__ pointer
TARGET_POINTER_MODE(P32EXT,  32, 4);  // __external__ pointer
TARGET_POINTER_MODE(P32EDS,  32, 4);  // __eds__ pointer
TARGET_POINTER_MODE(P32PEDS, 32, 4);  // paged __eds__ pointer
TARGET_POINTER_MODE(P32DF,   32, 4);  // __packed data flash pointer

