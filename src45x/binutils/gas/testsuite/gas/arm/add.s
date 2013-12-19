@ test case of ADD{S} and SUB{S} instructions in ARM mode

	.text
	.align	2
	add sp, sp, r0, LSL #4
	add sp, sp, r0, LSR #3
	add sp, sp, r0, ASR #3
	add sp, sp, r0, ROR #3
	add sp, sp, r0, RRX
	adds sp, sp, r0, LSL #4
	adds sp, sp, r0, LSR #3
	adds sp, sp, r0, ASR #3
	adds sp, sp, r0, ROR #3
	adds sp, sp, r0, RRX
	sub sp, sp, r0, LSL #4
	sub sp, sp, r0, LSR #3
	sub sp, sp, r0, ASR #3
	sub sp, sp, r0, ROR #3
	sub sp, sp, r0, RRX
	subs sp, sp, r0, LSL #4
	subs sp, sp, r0, LSR #3
	subs sp, sp, r0, ASR #3
	subs sp, sp, r0, ROR #3
	subs sp, sp, r0, RRX

	add sp, sp, r0
	add sp, sp, r0, LSL #1
	add sp, sp, r0, LSL #2
	add sp, sp, r0, LSL #3
	adds sp, sp, r0
	adds sp, sp, r0, LSL #1
	adds sp, sp, r0, LSL #2
	adds sp, sp, r0, LSL #3
	sub sp, sp, r0
	sub sp, sp, r0, LSL #1
	sub sp, sp, r0, LSL #2
	sub sp, sp, r0, LSL #3
	subs sp, sp, r0
	subs sp, sp, r0, LSL #1
	subs sp, sp, r0, LSL #2
	subs sp, sp, r0, LSL #3

	add sp, sp, r4
	add sp, sp, r4, LSL #1
	add sp, sp, r4, LSL #2
	add sp, sp, r4, LSL #3
	adds sp, sp, r4
	adds sp, sp, r4, LSL #1
	adds sp, sp, r4, LSL #2
	adds sp, sp, r4, LSL #3
	sub sp, sp, r4
	sub sp, sp, r4, LSL #1
	sub sp, sp, r4, LSL #2
	sub sp, sp, r4, LSL #3
	subs sp, sp, r4
	subs sp, sp, r4, LSL #1
	subs sp, sp, r4, LSL #2
	subs sp, sp, r4, LSL #3

	add r4, sp, r0
	add r4, sp, r0, LSL #1
	add r4, sp, r0, LSL #2
	add r4, sp, r0, LSL #3
	add r4, sp, r0, LSL #4
	add r4, sp, r0, LSR #3
	add r4, sp, r0, ASR #3
	add r4, sp, r0, ROR #3
	add r4, sp, r0, RRX
	adds r4, sp, r0
	adds r4, sp, r0, LSL #1
	adds r4, sp, r0, LSL #2
	adds r4, sp, r0, LSL #3
	adds r4, sp, r0, LSL #4
	adds r4, sp, r0, LSR #3
	adds r4, sp, r0, ASR #3
	adds r4, sp, r0, ROR #3
	adds r4, sp, r0, RRX
	sub r4, sp, r0
	sub r4, sp, r0, LSL #1
	sub r4, sp, r0, LSL #2
	sub r4, sp, r0, LSL #3
	sub r4, sp, r0, LSL #4
	sub r4, sp, r0, LSR #3
	sub r4, sp, r0, ASR #3
	sub r4, sp, r0, ROR #3
	sub r4, sp, r0, RRX
	subs r4, sp, r0
	subs r4, sp, r0, LSL #1
	subs r4, sp, r0, LSL #2
	subs r4, sp, r0, LSL #3
	subs r4, sp, r0, LSL #4
	subs r4, sp, r0, LSR #3
	subs r4, sp, r0, ASR #3
	subs r4, sp, r0, ROR #3
	subs r4, sp, r0, RRX
