#objdump: -dr --prefix-addresses --show-raw-insn
#name: ARM Add{S} and Sub{S} instructions
#as: -march=armv7-a

# Test some ARM instructions:

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <.*> e08dd200 	add	sp, sp, r0, lsl #4
0+004 <.*> e08dd1a0 	add	sp, sp, r0, lsr #3
0+008 <.*> e08dd1c0 	add	sp, sp, r0, asr #3
0+00c <.*> e08dd1e0 	add	sp, sp, r0, ror #3
0+010 <.*> e08dd060 	add	sp, sp, r0, rrx
0+014 <.*> e09dd200 	adds	sp, sp, r0, lsl #4
0+018 <.*> e09dd1a0 	adds	sp, sp, r0, lsr #3
0+01c <.*> e09dd1c0 	adds	sp, sp, r0, asr #3
0+020 <.*> e09dd1e0 	adds	sp, sp, r0, ror #3
0+024 <.*> e09dd060 	adds	sp, sp, r0, rrx
0+028 <.*> e04dd200 	sub	sp, sp, r0, lsl #4
0+02c <.*> e04dd1a0 	sub	sp, sp, r0, lsr #3
0+030 <.*> e04dd1c0 	sub	sp, sp, r0, asr #3
0+034 <.*> e04dd1e0 	sub	sp, sp, r0, ror #3
0+038 <.*> e04dd060 	sub	sp, sp, r0, rrx
0+03c <.*> e05dd200 	subs	sp, sp, r0, lsl #4
0+040 <.*> e05dd1a0 	subs	sp, sp, r0, lsr #3
0+044 <.*> e05dd1c0 	subs	sp, sp, r0, asr #3
0+048 <.*> e05dd1e0 	subs	sp, sp, r0, ror #3
0+04c <.*> e05dd060 	subs	sp, sp, r0, rrx
0+050 <.*> e08dd000 	add	sp, sp, r0
0+054 <.*> e08dd080 	add	sp, sp, r0, lsl #1
0+058 <.*> e08dd100 	add	sp, sp, r0, lsl #2
0+05c <.*> e08dd180 	add	sp, sp, r0, lsl #3
0+060 <.*> e09dd000 	adds	sp, sp, r0
0+064 <.*> e09dd080 	adds	sp, sp, r0, lsl #1
0+068 <.*> e09dd100 	adds	sp, sp, r0, lsl #2
0+06c <.*> e09dd180 	adds	sp, sp, r0, lsl #3
0+070 <.*> e04dd000 	sub	sp, sp, r0
0+074 <.*> e04dd080 	sub	sp, sp, r0, lsl #1
0+078 <.*> e04dd100 	sub	sp, sp, r0, lsl #2
0+07c <.*> e04dd180 	sub	sp, sp, r0, lsl #3
0+080 <.*> e05dd000 	subs	sp, sp, r0
0+084 <.*> e05dd080 	subs	sp, sp, r0, lsl #1
0+088 <.*> e05dd100 	subs	sp, sp, r0, lsl #2
0+08c <.*> e05dd180 	subs	sp, sp, r0, lsl #3
0+090 <.*> e08dd004 	add	sp, sp, r4
0+094 <.*> e08dd084 	add	sp, sp, r4, lsl #1
0+098 <.*> e08dd104 	add	sp, sp, r4, lsl #2
0+09c <.*> e08dd184 	add	sp, sp, r4, lsl #3
0+0a0 <.*> e09dd004 	adds	sp, sp, r4
0+0a4 <.*> e09dd084 	adds	sp, sp, r4, lsl #1
0+0a8 <.*> e09dd104 	adds	sp, sp, r4, lsl #2
0+0ac <.*> e09dd184 	adds	sp, sp, r4, lsl #3
0+0b0 <.*> e04dd004 	sub	sp, sp, r4
0+0b4 <.*> e04dd084 	sub	sp, sp, r4, lsl #1
0+0b8 <.*> e04dd104 	sub	sp, sp, r4, lsl #2
0+0bc <.*> e04dd184 	sub	sp, sp, r4, lsl #3
0+0c0 <.*> e05dd004 	subs	sp, sp, r4
0+0c4 <.*> e05dd084 	subs	sp, sp, r4, lsl #1
0+0c8 <.*> e05dd104 	subs	sp, sp, r4, lsl #2
0+0cc <.*> e05dd184 	subs	sp, sp, r4, lsl #3
0+0d0 <.*> e08d4000 	add	r4, sp, r0
0+0d4 <.*> e08d4080 	add	r4, sp, r0, lsl #1
0+0d8 <.*> e08d4100 	add	r4, sp, r0, lsl #2
0+0dc <.*> e08d4180 	add	r4, sp, r0, lsl #3
0+0e0 <.*> e08d4200 	add	r4, sp, r0, lsl #4
0+0e4 <.*> e08d41a0 	add	r4, sp, r0, lsr #3
0+0e8 <.*> e08d41c0 	add	r4, sp, r0, asr #3
0+0ec <.*> e08d41e0 	add	r4, sp, r0, ror #3
0+0f0 <.*> e08d4060 	add	r4, sp, r0, rrx
0+0f4 <.*> e09d4000 	adds	r4, sp, r0
0+0f8 <.*> e09d4080 	adds	r4, sp, r0, lsl #1
0+0fc <.*> e09d4100 	adds	r4, sp, r0, lsl #2
0+100 <.*> e09d4180 	adds	r4, sp, r0, lsl #3
0+104 <.*> e09d4200 	adds	r4, sp, r0, lsl #4
0+108 <.*> e09d41a0 	adds	r4, sp, r0, lsr #3
0+10c <.*> e09d41c0 	adds	r4, sp, r0, asr #3
0+110 <.*> e09d41e0 	adds	r4, sp, r0, ror #3
0+114 <.*> e09d4060 	adds	r4, sp, r0, rrx
0+118 <.*> e04d4000 	sub	r4, sp, r0
0+11c <.*> e04d4080 	sub	r4, sp, r0, lsl #1
0+120 <.*> e04d4100 	sub	r4, sp, r0, lsl #2
0+124 <.*> e04d4180 	sub	r4, sp, r0, lsl #3
0+128 <.*> e04d4200 	sub	r4, sp, r0, lsl #4
0+12c <.*> e04d41a0 	sub	r4, sp, r0, lsr #3
0+130 <.*> e04d41c0 	sub	r4, sp, r0, asr #3
0+134 <.*> e04d41e0 	sub	r4, sp, r0, ror #3
0+138 <.*> e04d4060 	sub	r4, sp, r0, rrx
0+13c <.*> e05d4000 	subs	r4, sp, r0
0+140 <.*> e05d4080 	subs	r4, sp, r0, lsl #1
0+144 <.*> e05d4100 	subs	r4, sp, r0, lsl #2
0+148 <.*> e05d4180 	subs	r4, sp, r0, lsl #3
0+14c <.*> e05d4200 	subs	r4, sp, r0, lsl #4
0+150 <.*> e05d41a0 	subs	r4, sp, r0, lsr #3
0+154 <.*> e05d41c0 	subs	r4, sp, r0, asr #3
0+158 <.*> e05d41e0 	subs	r4, sp, r0, ror #3
0+15c <.*> e05d4060 	subs	r4, sp, r0, rrx
