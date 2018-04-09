# mips r6 special (non-micromips) tests
# mach: mips32r6 mips64r6
# as:   -mabi=eabi
# ld:   -N -Ttext=0x80010000
# output: *\\npass\\n

  .include "testutils.inc"
  .include "utils-r6.inc"

  setup

  .set noreorder

  .ent DIAG
DIAG:
  writemsg "[1] Test NAL"
  jal GetPC
  nop
  move $11, $6
  nal
  nop
  addiu $11, 12
  beqc $11, $31, L1
  nop
  fail

L1:
  pass

  .end DIAG
