# Test for mf{hi,lo} -> mult/div/mt{hi,lo} with 2 nops inbetween.
#
# mach:		mips1 mips2 mips3 mips4 vr4100 vr4111 vr4120 vr5000 vr5400 mips32 mips64 mips32r2 mips64r2
# as:		-mabi=eabi
# ld:		-N -Ttext=0x80010000
# output:	pass\\n

	.include "hilo-hazard.inc"
	.include "testutils.inc"

	setup

	.set noreorder
	.ent DIAG
DIAG:
	hilo
	pass
	.end DIAG
