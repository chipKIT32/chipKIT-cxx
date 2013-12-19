	.set	micromips
	.text
foo:
	.insn				# Keep the ISA bit set on foo.
bar:
	.fill	0			# Keep the ISA bit clear on bar.
	.rept	count
	ori	$2, $3, (. - bar) >> 2
	.endr
	addu	$2, $3, $4
	j	ext
