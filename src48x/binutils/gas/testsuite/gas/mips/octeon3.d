#as: -march=octeon
#objdump: -dr -M gpr-names=numeric

.*:     file format .*

Disassembly of section \.text:

00000000 <start>:
.*:	70a62028 	baddu	\$4,\$5,\$6
.*:	c880fffe 	bbit0	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	c894fffc 	bbit0	\$4,0x14,0 <start>
.*:	00000000 	nop
.*:	c89ffffa 	bbit0	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	d880fff8 	bbit032	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	d892fff6 	bbit032	\$4,0x12,0 <start>
.*:	00000000 	nop
.*:	d89ffff4 	bbit032	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	d880fff2 	bbit032	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	d894fff0 	bbit032	\$4,0x14,0 <start>
.*:	00000000 	nop
.*:	d89fffee 	bbit032	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	e880ffec 	bbit1	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	e894ffea 	bbit1	\$4,0x14,0 <start>
.*:	00000000 	nop
.*:	e89fffe8 	bbit1	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	f880ffe6 	bbit132	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	f892ffe4 	bbit132	\$4,0x12,0 <start>
.*:	00000000 	nop
.*:	f89fffe2 	bbit132	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	f880ffe0 	bbit132	\$4,0x0,0 <start>
.*:	00000000 	nop
.*:	f894ffde 	bbit132	\$4,0x14,0 <start>
.*:	00000000 	nop
.*:	f89fffdc 	bbit132	\$4,0x1f,0 <start>
.*:	00000000 	nop
.*:	70a4f832 	cins	\$4,\$5,0x0,0x1f
.*:	70a4f2b2 	cins	\$4,\$5,0xa,0x1e
.*:	70a4eff2 	cins	\$4,\$5,0x1f,0x1d
.*:	70a4a033 	cins32	\$4,\$5,0x0,0x14
.*:	70a454b3 	cins32	\$4,\$5,0x12,0xa
.*:	70a407f3 	cins32	\$4,\$5,0x1f,0x0
.*:	70a45033 	cins32	\$4,\$5,0x0,0xa
.*:	70a45533 	cins32	\$4,\$5,0x14,0xa
.*:	70a62003 	dmul	\$4,\$5,\$6
.*:	70a0202d 	dpop	\$4,\$5
.*:	70a4f83a 	exts	\$4,\$5,0x0,0x1f
.*:	70a4f2ba 	exts	\$4,\$5,0xa,0x1e
.*:	70a4effa 	exts	\$4,\$5,0x1f,0x1d
.*:	70a4a03b 	exts32	\$4,\$5,0x0,0x14
.*:	70a454bb 	exts32	\$4,\$5,0x12,0xa
.*:	70a407fb 	exts32	\$4,\$5,0x1f,0x0
.*:	70a4503b 	exts32	\$4,\$5,0x0,0xa
.*:	70a4553b 	exts32	\$4,\$5,0x14,0xa
.*:	70800008 	mtm0	\$4
.*:	7080000c 	mtm1	\$4
.*:	7080000d 	mtm2	\$4
.*:	70800009 	mtp0	\$4
.*:	7080000a 	mtp1	\$4
.*:	7080000b 	mtp2	\$4
.*:	70a0202c 	pop	\$4,\$5
.*:	70a6202a 	seq	\$4,\$5,\$6
.*:	70a4802e 	seqi	\$4,\$5,-512
.*:	70a4ffee 	seqi	\$4,\$5,-1
.*:	70a4002e 	seqi	\$4,\$5,0
.*:	70a4006e 	seqi	\$4,\$5,1
.*:	70a47fee 	seqi	\$4,\$5,511
.*:	70a6202b 	sne	\$4,\$5,\$6
.*:	70a4802f 	snei	\$4,\$5,-512
.*:	70a4ffef 	snei	\$4,\$5,-1
.*:	70a4002f 	snei	\$4,\$5,0
.*:	70a4006f 	snei	\$4,\$5,1
.*:	70a47fef 	snei	\$4,\$5,511
.*:	0000004f 	syncio
.*:	0000008f 	synciobdma
.*:	000000cf 	syncioall
.*:	0000018f 	syncs
.*:	0000010f 	syncw
.*:	0000014f 	syncws
.*:	70a62011 	v3mulu	\$4,\$5,\$6
.*:	70a62010 	vmm0	\$4,\$5,\$6
.*:	70a6200f 	vmulu	\$4,\$5,\$6
.*:	48240000 	dmfc2	\$4,0x0
.*:	482403e8 	dmfc2	\$4,0x3e8
.*:	4824ffff 	dmfc2	\$4,0xffff
.*:	48a40000 	dmtc2	\$4,0x0
.*:	48a403e8 	dmtc2	\$4,0x3e8
.*:	48a4ffff 	dmtc2	\$4,0xffff
.*\.\.\.
