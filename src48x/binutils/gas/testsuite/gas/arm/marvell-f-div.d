#objdump: -dr --prefix-addresses --show-raw-insn
#name: Marvell division encodings

.*:     file format .*arm.*

Disassembly of section .text:
0[0-9a-f]+ <[^>]+> ee321693 	mrc	6, 1, r1, cr2, cr3, \{4\}
0[0-9a-f]+ <[^>]+> be321693 	mrclt	6, 1, r1, cr2, cr3, \{4\}
0[0-9a-f]+ <[^>]+> ee321613 	mrc	6, 1, r1, cr2, cr3, \{0\}
0[0-9a-f]+ <[^>]+> ce321613 	mrcgt	6, 1, r1, cr2, cr3, \{0\}

