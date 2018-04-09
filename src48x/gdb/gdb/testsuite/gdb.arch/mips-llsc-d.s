/* Test single-stepping over LLD/SCD sequence.  */
	.file   1 "mips-llsc-d.s"
	.data
	.align	8
test_data:
	.word	0xaaaaaaaa
	.word	0xbbbbbbbb
	.text

	.globl	main
	.type	main, @function
	.ent	main
main:
	nop
	.loc 1 15
_pre_ll1:
	lld	$2, test_data
	sll	$2, $2, 1
	beqz	$2, _return	/* forward out */
	scd	$2, test_data
_post_sc1:
	li	$3, 1
	xori	$4, $2, 0x1
	bnez	$4, _fail

	move	$2, $0
	b	_return

_fail:
	move  	$2, $3
_return:
	jr	$31
  .end main
