	.text
	.set	micromips
	.set	noreorder
test:
	j	test3
	addu	$3, $4, $5
	jal	test3
	addu	$3, $4, $5
	b	test3
	addu	$3, $4, $5
	bal	test3
	addu	$3, $4, $5
	beq	$3, $4, test3
	addu	$3, $4, $5
	bne	$3, $4, test3
	addu	$3, $4, $5
	bltz	$3, test3
	addu	$3, $4, $5
	bgez	$3, test3
	addu	$3, $4, $5
	blez	$20, test3
	addu	$3, $4, $5
	bgtz	$20, test3
	addu	$3, $4, $5
	bgezal	$30, test3
	addu	$3, $4, $5
	bltzal	$30, test3
	addu	$3, $4, $5
	beql	$3, $4, test3
	addu	$3, $4, $5
	beqz	$3, test3
	xor	$3, $4, $5
	bge	$3, $4, test3
	xor	$3, $4, $5
	bgel	$3, $4, test3
	xor	$3, $4, $5
	bgeu	$3, $4, test3
	xor	$3, $4, $5
	bgeul	$3, $4, test3
	xor	$3, $4, $5
	bgezl	$3, test3
	xor	$3, $4, $5
	bgt	$3, $4, test3
	xor	$3, $4, $5
	bgtl	$3, $4, test3
	xor	$3, $4, $5
	bgtu	$3, $4, test3
	xor	$3, $4, $5
	bgtul	$3, $4, test3
	xor	$3, $4, $5
	bgtzl	$3, test3
	xor	$3, $4, $5
	ble	$3, $4, test3
	xor	$3, $4, $5
	blel	$3, $4, test3
	xor	$3, $4, $5
	bleu	$3, $4, test3
	xor	$3, $4, $5
	bleul	$3, $4, test3
	xor	$3, $4, $5
	blezl	$3, test3
	xor	$3, $4, $5
	blt	$3, $4, test3
	xor	$3, $4, $5
	bltl	$3, $4, test3
	xor	$3, $4, $5
	bltu	$3, $4, test3
	xor	$3, $4, $5
	bltul	$3, $4, test3
	xor	$3, $4, $5
	bltzl	$3, test3
	xor	$3, $4, $5
	bltzall	$3, test3
	xor	$3, $4, $5
	bnez	$3, test3
	xor	$3, $4, $5
	bnezl	$3, test3
	xor	$3, $4, $5
	bnel	$3, $4, test3
	xor	$3, $4, $5

	.skip	32767<<1
test3:
	addu	$3, $4, $5
