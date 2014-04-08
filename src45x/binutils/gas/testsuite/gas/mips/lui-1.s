# Source code used to test error diagnostics with the LUI instruction.

	.text
foo:
	lui	$2, -1
	lui	$2, 65536
	lui	$2, $3
0:
	lui	$2, 0b
	lui	$2, 1f
1:
	lui	$2, foo
	lui	$2, bar
