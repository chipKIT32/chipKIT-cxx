#objdump: -dr --prefix-addresses --show-raw-insn
#name: Thumb2 Add{S} and Sub{S} instructions
#as: -march=armv7-a

# Test some Thumb2 instructions:

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <.*> 4485      	add	sp, r0
0+002 <.*> eb0d 0d40 	add.w	sp, sp, r0, lsl #1
0+006 <.*> eb0d 0d80 	add.w	sp, sp, r0, lsl #2
0+00a <.*> eb0d 0dc0 	add.w	sp, sp, r0, lsl #3
0+00e <.*> eb1d 0d00 	adds.w	sp, sp, r0
0+012 <.*> eb1d 0d40 	adds.w	sp, sp, r0, lsl #1
0+016 <.*> eb1d 0d80 	adds.w	sp, sp, r0, lsl #2
0+01a <.*> eb1d 0dc0 	adds.w	sp, sp, r0, lsl #3
0+01e <.*> ebad 0d00 	sub.w	sp, sp, r0
0+022 <.*> ebad 0d40 	sub.w	sp, sp, r0, lsl #1
0+026 <.*> ebad 0d80 	sub.w	sp, sp, r0, lsl #2
0+02a <.*> ebad 0dc0 	sub.w	sp, sp, r0, lsl #3
0+02e <.*> ebbd 0d00 	subs.w	sp, sp, r0
0+032 <.*> ebbd 0d40 	subs.w	sp, sp, r0, lsl #1
0+036 <.*> ebbd 0d80 	subs.w	sp, sp, r0, lsl #2
0+03a <.*> ebbd 0dc0 	subs.w	sp, sp, r0, lsl #3
0+03e <.*> 44a5      	add	sp, r4
0+040 <.*> eb0d 0d44 	add.w	sp, sp, r4, lsl #1
0+044 <.*> eb0d 0d84 	add.w	sp, sp, r4, lsl #2
0+048 <.*> eb0d 0dc4 	add.w	sp, sp, r4, lsl #3
0+04c <.*> eb1d 0d04 	adds.w	sp, sp, r4
0+050 <.*> eb1d 0d44 	adds.w	sp, sp, r4, lsl #1
0+054 <.*> eb1d 0d84 	adds.w	sp, sp, r4, lsl #2
0+058 <.*> eb1d 0dc4 	adds.w	sp, sp, r4, lsl #3
0+05c <.*> ebad 0d04 	sub.w	sp, sp, r4
0+060 <.*> ebad 0d44 	sub.w	sp, sp, r4, lsl #1
0+064 <.*> ebad 0d84 	sub.w	sp, sp, r4, lsl #2
0+068 <.*> ebad 0dc4 	sub.w	sp, sp, r4, lsl #3
0+06c <.*> ebbd 0d04 	subs.w	sp, sp, r4
0+070 <.*> ebbd 0d44 	subs.w	sp, sp, r4, lsl #1
0+074 <.*> ebbd 0d84 	subs.w	sp, sp, r4, lsl #2
0+078 <.*> ebbd 0dc4 	subs.w	sp, sp, r4, lsl #3
0+07c <.*> eb0d 0400 	add.w	r4, sp, r0
0+080 <.*> eb0d 0440 	add.w	r4, sp, r0, lsl #1
0+084 <.*> eb0d 0480 	add.w	r4, sp, r0, lsl #2
0+088 <.*> eb0d 04c0 	add.w	r4, sp, r0, lsl #3
0+08c <.*> eb0d 1400 	add.w	r4, sp, r0, lsl #4
0+090 <.*> eb0d 04d0 	add.w	r4, sp, r0, lsr #3
0+094 <.*> eb0d 04e0 	add.w	r4, sp, r0, asr #3
0+098 <.*> eb0d 04f0 	add.w	r4, sp, r0, ror #3
0+09c <.*> eb0d 0430 	add.w	r4, sp, r0, rrx
0+0a0 <.*> eb1d 0400 	adds.w	r4, sp, r0
0+0a4 <.*> eb1d 0440 	adds.w	r4, sp, r0, lsl #1
0+0a8 <.*> eb1d 0480 	adds.w	r4, sp, r0, lsl #2
0+0ac <.*> eb1d 04c0 	adds.w	r4, sp, r0, lsl #3
0+0b0 <.*> eb1d 1400 	adds.w	r4, sp, r0, lsl #4
0+0b4 <.*> eb1d 04d0 	adds.w	r4, sp, r0, lsr #3
0+0b8 <.*> eb1d 04e0 	adds.w	r4, sp, r0, asr #3
0+0bc <.*> eb1d 04f0 	adds.w	r4, sp, r0, ror #3
0+0c0 <.*> eb1d 0430 	adds.w	r4, sp, r0, rrx
0+0c4 <.*> ebad 0400 	sub.w	r4, sp, r0
0+0c8 <.*> ebad 0440 	sub.w	r4, sp, r0, lsl #1
0+0cc <.*> ebad 0480 	sub.w	r4, sp, r0, lsl #2
0+0d0 <.*> ebad 04c0 	sub.w	r4, sp, r0, lsl #3
0+0d4 <.*> ebad 1400 	sub.w	r4, sp, r0, lsl #4
0+0d8 <.*> ebad 04d0 	sub.w	r4, sp, r0, lsr #3
0+0dc <.*> ebad 04e0 	sub.w	r4, sp, r0, asr #3
0+0e0 <.*> ebad 04f0 	sub.w	r4, sp, r0, ror #3
0+0e4 <.*> ebad 0430 	sub.w	r4, sp, r0, rrx
0+0e8 <.*> ebbd 0400 	subs.w	r4, sp, r0
0+0ec <.*> ebbd 0440 	subs.w	r4, sp, r0, lsl #1
0+0f0 <.*> ebbd 0480 	subs.w	r4, sp, r0, lsl #2
0+0f4 <.*> ebbd 04c0 	subs.w	r4, sp, r0, lsl #3
0+0f8 <.*> ebbd 1400 	subs.w	r4, sp, r0, lsl #4
0+0fc <.*> ebbd 04d0 	subs.w	r4, sp, r0, lsr #3
0+100 <.*> ebbd 04e0 	subs.w	r4, sp, r0, asr #3
0+104 <.*> ebbd 04f0 	subs.w	r4, sp, r0, ror #3
0+108 <.*> ebbd 0430 	subs.w	r4, sp, r0, rrx
