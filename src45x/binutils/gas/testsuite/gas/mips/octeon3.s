start:
	baddu	$4,$5,$6
	bbit0	$4,0,start
	bbit0	$4,20,start
	bbit0	$4,31,start
	bbit0	$4,32,start
	bbit0	$4,50,start
	bbit0	$4,63,start
	bbit032	$4,0,start
	bbit032	$4,20,start
	bbit032	$4,31,start
	bbit1	$4,0,start
	bbit1	$4,20,start
	bbit1	$4,31,start
	bbit1	$4,32,start
	bbit1	$4,50,start
	bbit1	$4,63,start
	bbit132	$4,0,start
	bbit132	$4,20,start
	bbit132	$4,31,start
	cins	$4,$5,0,31
	cins	$4,$5,10,30
	cins	$4,$5,31,29
	cins	$4,$5,32,20
	cins	$4,$5,50,10
	cins	$4,$5,63,0
	cins32	$4,$5,0,10
	cins32	$4,$5,20,10
	dmul	$4,$5,$6
	dpop	$4,$5
	exts	$4,$5,0,31
	exts	$4,$5,10,30
	exts	$4,$5,31,29
	exts	$4,$5,32,20
	exts	$4,$5,50,10
	exts	$4,$5,63,0
	exts32	$4,$5,0,10
	exts32	$4,$5,20,10
	mtm0	$4
	mtm1	$4
	mtm2	$4
	mtp0	$4
	mtp1	$4
	mtp2	$4
	pop	$4,$5
	seq	$4,$5,$6
	seqi	$4,$5,-512
	seqi	$4,$5,-1
	seqi	$4,$5,0
	seqi	$4,$5,1
	seqi	$4,$5,511
	sne	$4,$5,$6
	snei	$4,$5,-512
	snei	$4,$5,-1
	snei	$4,$5,0
	snei	$4,$5,1
	snei	$4,$5,511
	syncio
	synciobdma
	syncioall
	syncs
	syncw
	syncws
	v3mulu	$4,$5,$6
	vmm0	$4,$5,$6
	vmulu	$4,$5,$6
	dmfc2	$4,0
	dmfc2	$4,1000
	dmfc2	$4,65535
	dmtc2	$4,0
	dmtc2	$4,1000
	dmtc2	$4,65535

# Force at least 8 (non-delay-slot) zero bytes, to make 'objdump' print ...
	.space	8
