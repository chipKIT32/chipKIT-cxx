#objdump: -dr --prefix-addresses --show-raw-insn
#name: PUSH and POP

# Test the `PUSH' and `POP' instructions

.*: +file format .*arm.*

Disassembly of section .text:
0+000 <.*> e52d0004 	push	{r0}		; \(str r0, \[sp, #-4\]!\)
0+004 <.*> e49d0004 	pop	{r0}		; \(ldr r0, \[sp\], #4\)
