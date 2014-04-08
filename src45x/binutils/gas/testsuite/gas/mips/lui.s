# Source code used to test the LUI instruction with some expressions.

	.text
foo:
0:
	lui	$2, 0
	lui	$2, 65535
1:
	lui	$2, 1b - 0b
bar:
	lui	$2, 2f - 1b
2:
	lui	$2, bar - foo
	lui	$2, baz - bar
baz:
	sll	$0, $0, 0

# Force some (non-delay-slot) zero bytes, to make 'objdump' print ...
	.align	4, 0
	.space	16
