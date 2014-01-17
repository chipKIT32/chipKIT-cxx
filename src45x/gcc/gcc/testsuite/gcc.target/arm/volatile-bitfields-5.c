/* { dg-require-effective-target arm_eabi } */
/* { dg-do run } */
/* { dg-options "-O2 -fno-strict-aliasing" } */

#include <stdlib.h>

#pragma pack(1)	// no space between structure members
volatile typedef struct {
  unsigned char byte;
  /* Packed field members get converted to bitfields internally.
     On most other these will either be split into smaller accesses,
     or a single large aligned access.  With -fstrict-volatile-bitfields
     store_bitfield was assuming a single aligned user-specified-size access
     covered the whole field.  */
  unsigned short halfword;
} S;
#pragma pack() // resume default structure packing

void __attribute__((noinline)) foo(S *p)
{
  p->halfword++; /* { dg-message "note: When a volatile" } */
  /* { dg-warning "mis-aligned access" "" { target *-*-* } 21 } */
}

int main()
{
/* Make sure the halfword is actually aligned in practice.  */
  short buf[3];
  buf[0] = 0x5a5a;
  buf[1] = 0x42ff;
  buf[2] = 0x5a5a;

  foo((S *)(((char *)buf) + 1));
  if (buf[0] != 0x5a5a || buf[1] != 0x4300 || buf[2] != 0x5a5a)
    abort();
  return 0;
}
