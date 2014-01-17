#source: octeon2.s
#as: -march=octeon -mno-octeon-useun
#objdump: -dr -Mno-octeon-useun

.*:     file format .*

Disassembly of section .*:

00000000 <.*>:
.*:	68a40000 	ldl	a0,0\(a1\)
.*:	6ca40007 	ldr	a0,7\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	68240000 	ldl	a0,0\(at\)
.*:	6c240007 	ldr	a0,7\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	68240000 	ldl	a0,0\(at\)
.*:	6c240007 	ldr	a0,7\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	68240000 	ldl	a0,0\(at\)
.*:	6c240007 	ldr	a0,7\(at\)
.*:	88a40000 	lwl	a0,0\(a1\)
.*:	98a40003 	lwr	a0,3\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	88240000 	lwl	a0,0\(at\)
.*:	98240003 	lwr	a0,3\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	88240000 	lwl	a0,0\(at\)
.*:	98240003 	lwr	a0,3\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	88240000 	lwl	a0,0\(at\)
.*:	98240003 	lwr	a0,3\(at\)
.*:	b0a40000 	sdl	a0,0\(a1\)
.*:	b4a40007 	sdr	a0,7\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	b0240000 	sdl	a0,0\(at\)
.*:	b4240007 	sdr	a0,7\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	b0240000 	sdl	a0,0\(at\)
.*:	b4240007 	sdr	a0,7\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	b0240000 	sdl	a0,0\(at\)
.*:	b4240007 	sdr	a0,7\(at\)
.*:	a8a40000 	swl	a0,0\(a1\)
.*:	b8a40003 	swr	a0,3\(a1\)
.*:	3c011234 	lui	at,0x1234
.*:	34215678 	ori	at,at,0x5678
.*:	00250821 	addu	at,at,a1
.*:	a8240000 	swl	a0,0\(at\)
.*:	b8240003 	swr	a0,3\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	a8240000 	swl	a0,0\(at\)
.*:	b8240003 	swr	a0,3\(at\)
.*:	3c010000 	lui	at,0x0
.*: R_MIPS_HI16	symbol
.*:	24210000 	addiu	at,at,0
.*: R_MIPS_LO16	symbol
.*:	00250821 	addu	at,at,a1
.*:	a8240000 	swl	a0,0\(at\)
.*:	b8240003 	swr	a0,3\(at\)
