#source: octeon2.s
#as: -march=octeon -mocteon-useun
#objdump: -dr

.*:     file format .*

Disassembly of section .*:

00000000 <.*>:
.*:	68a40000 	uld	a0,0\(a1\)
.*:	3c041234 	lui	a0,0x1234
.*:	00852021 	addu	a0,a0,a1
.*:	68845678 	uld	a0,22136\(a0\)
.*:	3c040000 	lui	a0,0x0
.*: R_MIPS_HI16	symbol
.*:	68840000 	uld	a0,0\(a0\)
.*: R_MIPS_LO16	symbol
.*:	3c040000 	lui	a0,0x0
.*: R_MIPS_HI16	symbol
.*:	00852021 	addu	a0,a0,a1
.*:	68840000 	uld	a0,0\(a0\)
.*: R_MIPS_LO16	symbol
.*:	88a40000 	ulw	a0,0\(a1\)
.*:	3c041234 	lui	a0,0x1234
.*:	00852021 	addu	a0,a0,a1
.*:	88845678 	ulw	a0,22136\(a0\)
.*:	3c040000 	lui	a0,0x0
.*: R_MIPS_HI16	symbol
.*:	88840000 	ulw	a0,0\(a0\)
.*: R_MIPS_LO16	symbol
.*:	3c040000 	lui	a0,0x0
.*: R_MIPS_HI16	symbol
.*:	00852021 	addu	a0,a0,a1
.*:	88840000 	ulw	a0,0\(a0\)
.*: R_MIPS_LO16	symbol
.*:	b0a40000 	usd	a0,0\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	00250821 	addu	at,at,a1
.*:	b0245678 	usd	a0,22136\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	b0240000 	usd	a0,0\(at\)
.*: R_MIPS_LO16	symbol
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	00250821 	addu	at,at,a1
.*:	b0240000 	usd	a0,0\(at\)
.*: R_MIPS_LO16	symbol
.*:	a8a40000 	usw	a0,0\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	00250821 	addu	at,at,a1
.*:	a8245678 	usw	a0,22136\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	a8240000 	usw	a0,0\(at\)
.*: R_MIPS_LO16	symbol
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	00250821 	addu	at,at,a1
.*:	a8240000 	usw	a0,0\(at\)
.*: R_MIPS_LO16	symbol
