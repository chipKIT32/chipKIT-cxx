/* Test single-stepping over LL/SC sequence.  */
	.file   1 "mips-llsc.s"
	.data
	.align	8
test_data:
	.word	0xaaaaaaaa
	.text

	.globl	main
	.type	main, @function
	.ent	main
main:
	.loc 1 15
_pre_ll1:
	ll	$2, test_data
	sll	$2, $2, 1
	beqz	$2, _return	/* forward out */
	sc	$2, test_data
_post_sc1:
	li	$3, 1
	xori	$4, $2, 0x1
	bnez	$4, _fail

_pre_ll2:
	ll	$2, test_data
	beqz	$2, _post_sc2	/* end of sequence */
	srl	$2, $2, 1
	bltz	$2, _post_sc2	/* end of sequence */
	addu	$2, $2, $2
	beqz	$2, _post_sc1	/* backward out */
	sc	$2, test_data
_post_sc2:
	li	$3, 2
	xori	$4, $2, 0x1
	bnez	$4, _fail

	la	$2, test_data
_pre_ll3:
	ll	$2, ($2)
	beqz	$2, _pre_ll3	/* start of sequence */
	srl	$2, $2, 1
	bltz	$2, _pre_ll3	/* start of sequence */
	addu	$2, $2, $2
	beqz	$2, _fail	/* forward out */
	sc	$2, test_data
_post_sc3:
	li	$3, 3
	xori	$4, $2, 0x1
	bnez	$4, _fail

	la	$2, test_data
_pre_ll4:
	ll	$2, ($2)
	beqz	$2, _return	/* forward out */
	srl	$2, $2, 1
	bltz	$2, _return	/* repeat forward out */
	addiu	$2, $2, 3
	beqz	$2, _return	/* repeat forward out */
	addu	$2, $2, $2
	beqz	$2, _pre_ll4	/* start of sequence */
	sc	$2, test_data
_post_sc4:
	li	$3, 4
	xori	$4, $2, 0x1
	bnez	$4, _fail

	la	$2, test_data
_pre_ll5:
	ll	$2, ($2)
	bnez	$2, _i5		/* intra sequence */
	srl	$2, $2, 1
	b	_i5		/* repeat intra sequence */
	xori	$2, $2, 1
	b	main		/* backward out */
	addu	$2, $2, $2
	beqz	$2, _pre_ll5	/* start of sequence */
_i5:
	sc	$2, test_data
_post_sc5:
	li	$3, 5
	xori	$4, $2, 0x1
	bnez	$4, _fail

	move	$2, $0
	b	_return

_fail:
	move  $2, $3
_return:
	jr $31
  .end main
