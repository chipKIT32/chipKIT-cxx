/* { dg-do compile } */
/* { dg-options "-O2 -fdump-rtl-expand" } */

const char *
f (const char *p)
{
  while (1)
    {
      switch (*p)
        {
        case 9:
        case 10:
        case 13:
        case 32:
          break;
        default:
          return p;
        }
    }
}

/* { dg-final { scan-rtl-dump-times "jump_insn" 4 "expand" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump-times "REG_BR_PROB" 2 "expand" { target mips*-*-* } } } */
/* { dg-final { scan-rtl-dump-times "lt " 1 "expand"  { target mips*-*-* } } } */
/* { dg-final { cleanup-rtl-dump "expand" } } */
