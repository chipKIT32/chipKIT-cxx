#objdump: -dr --prefix-addresses --show-raw-insn -M reg-names=numeric
#name: uld2 -EB
#source: uld2.s
#as: -EB

# Further checks of uld macro (microMIPS).
# XXX: note: when 'move' is changed to use 'or' rather than daddu, the
# XXX: 'move' opcodes shown here (whose raw instruction fields are daddu)
# XXX: should be changed to be 'or' instructions and this comment should be
# XXX: removed.

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 4000 	ldl	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 6081 5007 	ldr	\$4,7\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 4000 	ldl	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 6081 5007 	ldr	\$4,7\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 4000 	ldl	\$5,0\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 5007 	ldr	\$5,7\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 4000 	ldl	\$5,0\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 5007 	ldr	\$5,7\(\$1\)
	\.\.\.
