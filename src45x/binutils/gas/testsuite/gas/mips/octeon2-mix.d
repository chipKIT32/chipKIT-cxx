#source: octeon2.s
#as: -march=octeon -mno-octeon-useun
#objdump: -dr -Mocteon-useun

.*:     file format .*

Disassembly of section .*:

00000000 <.*>:
.*:	68a40000 	uld	a0,0\(a1\)
.*:	6ca40007 	nop
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	68240000 	uld	a0,0\(at\)
.*:	6c240007 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	68240000 	uld	a0,0\(at\)
.*:	6c240007 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	68240000 	uld	a0,0\(at\)
.*:	6c240007 	nop
.*:	88a40000 	ulw	a0,0\(a1\)
.*:	98a40003 	nop
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	88240000 	ulw	a0,0\(at\)
.*:	98240003 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	88240000 	ulw	a0,0\(at\)
.*:	98240003 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	88240000 	ulw	a0,0\(at\)
.*:	98240003 	nop
.*:	b0a40000 	usd	a0,0\(a1\)
.*:	b4a40007 	nop
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	b0240000 	usd	a0,0\(at\)
.*:	b4240007 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	b0240000 	usd	a0,0\(at\)
.*:	b4240007 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	b0240000 	usd	a0,0\(at\)
.*:	b4240007 	nop
.*:	a8a40000 	usw	a0,0\(a1\)
.*:	b8a40003 	nop
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	a8240000 	usw	a0,0\(at\)
.*:	b8240003 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	a8240000 	usw	a0,0\(at\)
.*:	b8240003 	nop
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	a8240000 	usw	a0,0\(at\)
.*:	b8240003 	nop
