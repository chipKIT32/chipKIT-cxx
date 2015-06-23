#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS jal-svr4pic
#as: -32 -KPIC
#source: jal-svr4pic.s

# Test the jal macro with -KPIC (microMIPS).

.*: +file format .*mips.*

Disassembly of section \.text:
[0-9a-f]+ <[^>]*> 41bc 0000 	lui	gp,0x0
[ 	]*0: R_MICROMIPS_HI16	_gp_disp
[0-9a-f]+ <[^>]*> 339c 0000 	addiu	gp,gp,0
[ 	]*4: R_MICROMIPS_LO16	_gp_disp
[0-9a-f]+ <[^>]*> 033c e150 	addu	gp,gp,t9
[0-9a-f]+ <[^>]*> fb9d 0000 	sw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> 45d9      	jalr	t9
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> ff9d 0000 	lw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> 0099 0f3c 	jalr	a0,t9
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> ff9d 0000 	lw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> ff3c 0000 	lw	t9,0\(gp\)
[ 	]*26: R_MICROMIPS_GOT16	text_label
[0-9a-f]+ <[^>]*> 3339 0000 	addiu	t9,t9,0
[ 	]*2a: R_MICROMIPS_LO16	text_label
[0-9a-f]+ <[^>]*> 45d9      	jalr	t9
[ 	]*2e: R_MICROMIPS_JALR	text_label
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> ff9d 0000 	lw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> ff3c 0000 	lw	t9,0\(gp\)
[ 	]*38: R_MICROMIPS_CALL16	weak_text_label
[0-9a-f]+ <[^>]*> 45d9      	jalr	t9
[ 	]*3c: R_MICROMIPS_JALR	weak_text_label
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> ff9d 0000 	lw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> ff3c 0000 	lw	t9,0\(gp\)
[ 	]*46: R_MICROMIPS_CALL16	external_text_label
[0-9a-f]+ <[^>]*> 45d9      	jalr	t9
[ 	]*4a: R_MICROMIPS_JALR	external_text_label
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> ff9d 0000 	lw	gp,0\(sp\)
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+0054 <text_label\+0x54>
[ 	]*54: R_MICROMIPS_PC16_S1	text_label
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
