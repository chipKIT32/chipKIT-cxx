	.text
	.arch armv7-m
	.syntax unified
	.thumb
foo:
	swi 0x5a
	swi 0xa5
	svc 0x5a
	svc 0xa5
