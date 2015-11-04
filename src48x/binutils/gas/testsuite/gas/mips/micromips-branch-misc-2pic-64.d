#objdump: -dr --prefix-addresses --show-raw-insn
#name: MIPS branch-misc-2pic-64
#source: branch-misc-2.s
#as: -64 -call_shared

# Test the backward branches to global symbols in current file (microMIPS).

.*: +file format .*mips.*

Disassembly of section \.text:
	\.\.\.
	\.\.\.
	\.\.\.
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+003c <x>
			3c: R_MICROMIPS_PC16_S1	g1\+0xf+fffc
			3c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			3c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+0044 <x\+0x8>
			44: R_MICROMIPS_PC16_S1	g2\+0xf+fffc
			44: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			44: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+004c <x\+0x10>
			4c: R_MICROMIPS_PC16_S1	g3\+0xf+fffc
			4c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			4c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+0054 <x\+0x18>
			54: R_MICROMIPS_PC16_S1	g4\+0xf+fffc
			54: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			54: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+005c <x\+0x20>
			5c: R_MICROMIPS_PC16_S1	g5\+0xf+fffc
			5c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			5c: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
[0-9a-f]+ <[^>]*> 4060 fffe 	bal	0+0064 <x\+0x28>
			64: R_MICROMIPS_PC16_S1	g6\+0xf+fffc
			64: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			64: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0000 0000 	nop
	\.\.\.
	\.\.\.
	\.\.\.
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+00a8 <g6>
			a8: R_MICROMIPS_PC16_S1	x1\+0xf+fffc
			a8: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			a8: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+00ae <g6\+0x6>
			ae: R_MICROMIPS_PC16_S1	x2\+0xf+fffc
			ae: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			ae: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0c00      	nop
[0-9a-f]+ <[^>]*> 9400 fffe 	b	0+00b4 <g6\+0xc>
			b4: R_MICROMIPS_PC16_S1	\.Ldata\+0xf+fffc
			b4: R_MIPS_NONE	\*ABS\*\+0xf+fffc
			b4: R_MIPS_NONE	\*ABS\*\+0xf+fffc
[0-9a-f]+ <[^>]*> 0c00      	nop
	\.\.\.
[0-9a-f]+ <[^>]*> 0c00      	nop
