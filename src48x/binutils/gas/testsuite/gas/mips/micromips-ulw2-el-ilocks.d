#objdump: -dr --prefix-addresses --show-raw-insn -M reg-names=numeric
#name: ulw2 -EL interlocked
#source: ulw2.s
#as: -EL -32

# Further checks of ulw macro (microMIPS).
# XXX: note: when 'move' is changed to use 'or' rather than addu/daddu, the
# XXX: 'move' opcodes shown here (whose raw instruction fields are addu/daddu)
# XXX: should be changed to be 'or' instructions and this comment should be
# XXX: removed.

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 0003 	lwl	\$4,3\(\$1\)
[0-9a-f]+ <[^>]*> 6081 1000 	lwr	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 6081 0003 	lwl	\$4,3\(\$1\)
[0-9a-f]+ <[^>]*> 6081 1000 	lwr	\$4,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0000 	li	\$1,0
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 0003 	lwl	\$5,3\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 1000 	lwr	\$5,0\(\$1\)
[0-9a-f]+ <[^>]*> 3020 0001 	li	\$1,1
[0-9a-f]+ <[^>]*> 00a1 0950 	addu	\$1,\$1,\$5
[0-9a-f]+ <[^>]*> 60a1 0003 	lwl	\$5,3\(\$1\)
[0-9a-f]+ <[^>]*> 60a1 1000 	lwr	\$5,0\(\$1\)
	\.\.\.
