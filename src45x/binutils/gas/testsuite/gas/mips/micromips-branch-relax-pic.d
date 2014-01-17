#objdump: -dr --show-raw-insn
#name: Relax microMIPS branches (pic)
#as: -mips32r2 -32 -relax-branch -KPIC
#stderr: micromips-branch-relax-pic.l
#source: micromips-branch-relax.s

.*: +file format .*mips.*

Disassembly of section \.text:

[0-9a-f]+ <test>:
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	ff3c 0000 	lw	t9,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3339 0000 	addiu	t9,t9,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	45d9      	jalr	t9
[ 	]*[0-9a-f]+: R_MICROMIPS_JALR	test3
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	45c1      	jalr	at
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	b483 0006 	bne	v1,a0,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	9483 0006 	beq	v1,a0,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4043 0006 	bgez	v1,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4003 0006 	bltz	v1,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	40d4 0006 	bgtz	s4,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	4094 0006 	blez	s4,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	401e 0006 	bltz	s8,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	45c1      	jalr	at
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	405e 0006 	bgez	s8,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	45c1      	jalr	at
[ 0-9a-f]+:	00a4 1950 	addu	v1,a0,a1
[ 0-9a-f]+:	b483 fffe 	bne	v1,a0,[0-9a-f]+ <test\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	b403 0006 	bnez	v1,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	b401 0006 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	b401 fffe 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	b401 0006 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	b401 fffe 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4003 fffe 	bltz	v1,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	9401 0006 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	9401 fffe 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	9401 0006 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	9401 fffe 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4083 fffe 	blez	v1,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	b401 0006 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b50 	slt	at,a0,v1
[ 0-9a-f]+:	b401 fffe 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	b401 0006 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0064 0b90 	sltu	at,a0,v1
[ 0-9a-f]+:	b401 fffe 	bnez	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	40c3 fffe 	bgtz	v1,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	9401 0006 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b50 	slt	at,v1,a0
[ 0-9a-f]+:	9401 fffe 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	9401 0006 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	0083 0b90 	sltu	at,v1,a0
[ 0-9a-f]+:	9401 fffe 	beqz	at,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4043 fffe 	bgez	v1,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	4043 fffe 	bgez	v1,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	45c1      	jalr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	9403 0006 	beqz	v1,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1
[ 0-9a-f]+:	9403 fffe 	beqz	v1,[0-9a-f]+ <.*\+0x[0-9a-f]+>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
[ 0-9a-f]+:	9483 fffe 	beq	v1,a0,[0-9a-f]+ <.*>
[ 	]*[0-9a-f]+: R_MICROMIPS_PC16_S1	.*
[ 0-9a-f]+:	0c00      	nop
[ 0-9a-f]+:	fc3c 0000 	lw	at,0\(gp\)
[ 	]*[0-9a-f]+: R_MICROMIPS_GOT16	test3
[ 0-9a-f]+:	3021 0000 	addiu	at,at,0
[ 	]*[0-9a-f]+: R_MICROMIPS_LO16	test3
[ 0-9a-f]+:	4581      	jr	at
[ 0-9a-f]+:	00a4 1b10 	xor	v1,a0,a1

[0-9a-f]+ <.*>:
	\.\.\.

[0-9a-f]+ <test3>:
[ 0-9a-f]+:	05d8      	addu	v1,a0,a1
[ 0-9a-f]+:	0c00      	nop
#pass
