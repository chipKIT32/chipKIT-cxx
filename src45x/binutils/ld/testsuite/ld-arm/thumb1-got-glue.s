	.cpu arm7tdmi
	.text
	.align 2
        .thumb_func
	.global ok
        .type   ok, %function
ok:
        push    {r3, lr}
	pop	{r3}
	pop	{r0}
	bx	r0

	.align 2
	.thumb_func
	.global _start
	.type _start, %function
_start:
	push {r3, lr}
	ldr r3, .L0
.LPIC0:
	add r3, pc
	ldr r2, .L0+4
	ldr r3, [r3, r2]
	bl .L1
	pop {r3}
	pop {r0}
	bx r0
	
.L0:
	.word   _GLOBAL_OFFSET_TABLE_-(.LPIC0+4)
	.word	ok(GOT)

.L1:
	bx r3
