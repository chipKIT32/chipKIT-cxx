# Source code used to test %hi/%lo operators with constants worked out
# as a difference of local symbols, both previously defined and forward
# references.

	.text
foo:
	li	$4, %hi(1f - 0f)
	sll	$4, 16
	addiu	$4, %lo(1f - 0f)
0:
	li	$4, %hi(1f - 0b)
	sll	$4, 16
	addiu	$4, %lo(1f - 0b)
2:
	.insn
	.skip	0x7ffc - (2b - 0b)
1:
	li	$4, %hi(1b - 0b)
	sll	$4, 16
	addiu	$4, %lo(1b - 0b)

	li	$4, %hi(1f - 0f)
	sll	$4, 16
	addiu	$4, %lo(1f - 0f)
0:
	li	$4, %hi(1f - 0b)
	sll	$4, 16
	addiu	$4, %lo(1f - 0b)
2:
	.insn
	.skip	0x8000 - (2b - 0b)
1:
	li	$4, %hi(1b - 0b)
	sll	$4, 16
	addiu	$4, %lo(1b - 0b)

	li	$4, %hi(1f - 0f)
	sll	$4, 16
	addiu	$4, %lo(1f - 0f)
0:
	li	$4, %hi(1f - 0b)
	sll	$4, 16
	addiu	$4, %lo(1f - 0b)
2:
	.insn
	.skip	0x11ffdc - (2b - 0b)
1:
	li	$4, %hi(1b - 0b)
	sll	$4, 16
	addiu	$4, %lo(1b - 0b)

# Force some (non-delay-slot) zero bytes, to make 'objdump' print ...
	.align	4, 0
	.space	16
