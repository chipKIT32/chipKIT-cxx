#objdump: -dr --show-raw-insn
#name: Relax microMIPS branches
#as: -mips32r2 -32 -relax-branch
#stderr: micromips-branch-relax.l
#source: micromips-branch-relax.s

.*: +file format .*mips.*

Disassembly of section \.text:

[0-9a-f]+ <test>:
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	f400 0000 	jal	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	f400 0000 	jal	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	b483 0003 	bne	v1,a0,26 <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	9483 0003 	beq	v1,a0,32 <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4043 0003 	bgez	v1,3e <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4003 0003 	bltz	v1,4a <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	40d4 0003 	bgtz	s4,56 <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4094 0003 	blez	s4,62 <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	401e 0003 	bltz	s8,6e <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	f400 0000 	jal	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	405e 0003 	bgez	s8,7c <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	f400 0000 	jal	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	b483 fffe 	bne	v1,a0,80 <test\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	b403 0003 	bnez	v1,96 <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	b401 0003 	bnez	at,a8 <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	b401 fffe 	bnez	at,b0 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	b401 0003 	bnez	at,cc <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	b401 fffe 	bnez	at,d4 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4003 fffe 	bltz	v1,e2 <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	9401 0003 	beqz	at,fe <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	9401 fffe 	beqz	at,106 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	9401 0003 	beqz	at,122 <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	9401 fffe 	beqz	at,12a <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4083 fffe 	blez	v1,138 <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	b401 0003 	bnez	at,154 <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	b401 fffe 	bnez	at,15c <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	b401 0003 	bnez	at,178 <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	b401 fffe 	bnez	at,180 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	40c3 fffe 	bgtz	v1,18e <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	9401 0003 	beqz	at,1aa <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	9401 fffe 	beqz	at,1b2 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	9401 0003 	beqz	at,1ce <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	9401 fffe 	beqz	at,1d6 <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4043 fffe 	bgez	v1,1e4 <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4043 fffe 	bgez	v1,1f2 <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	f400 0000 	jal	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	9403 0003 	beqz	v1,20a <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	9403 fffe 	beqz	v1,20e <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	9483 fffe 	beq	v1,a0,21c <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	d400 0000 	j	0 <test>
[ 	]*[0-9a-f]+: R_MICROMIPS_26_S1	test3
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
	\.\.\.

[0-9a-f]+ <test3>:
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	0c00      	nop
#pass
