#objdump: -dr --prefix-addresses --show-raw-insn -M reg-names=numeric
#name: uld2 -EL
#source: uld2.s
#as: -EL

# Further checks of uld macro (microMIPS).
# XXX: note: when 'move' is changed to use 'or' rather than daddu, the
# XXX: 'move' opcodes shown here (whose raw instruction fields are daddu)
# XXX: should be changed to be 'or' instructions and this comment should be
# XXX: removed.

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 4007 	ldl	\$4,7\(\$1\)
[0-9a-f]+ <[^>]*> 6081 5000 	ldr	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 4007 	ldl	\$4,7\(\$1\)
[0-9a-f]+ <[^>]*> 6081 5000 	ldr	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 4007 	ldl	\$5,7\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 5000 	ldr	\$5,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 4007 	ldl	\$5,7\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 5000 	ldr	\$5,0\(\$1\)
	\.\.\.
