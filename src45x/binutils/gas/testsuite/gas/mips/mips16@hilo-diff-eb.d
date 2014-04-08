#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS hilo-diff o32/EB
#as: -32 -EB
#source: hilo-diff.s

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> f000 6c00 	li	a0,0
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f7ef 4c1c 	addiu	a0,32764
[0-9a-f]+ <[^>]*> f000 6c00 	li	a0,0
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f7ef 4c1c 	addiu	a0,32764
	\.\.\.
[0-9a-f]+ <[^>]*> 6c00      	li	a0,0
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f7ef 4c1c 	addiu	a0,32764
[0-9a-f]+ <[^>]*> f000 6c01 	li	a0,1
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f010 4c00 	addiu	a0,-32768
[0-9a-f]+ <[^>]*> f000 6c01 	li	a0,1
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f010 4c00 	addiu	a0,-32768
	\.\.\.
[0-9a-f]+ <[^>]*> 6c01      	li	a0,1
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f010 4c00 	addiu	a0,-32768
[0-9a-f]+ <[^>]*> f000 6c12 	li	a0,18
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f7df 4c1c 	addiu	a0,-36
[0-9a-f]+ <[^>]*> f000 6c12 	li	a0,18
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> f7df 4c1c 	addiu	a0,-36
	\.\.\.
[0-9a-f]+ <[^>]*> 6c12      	li	a0,18
[0-9a-f]+ <[^>]*> f400 3480 	sll	a0,16
[0-9a-f]+ <[^>]*> 4cdc      	addiu	a0,-36
	\.\.\.
