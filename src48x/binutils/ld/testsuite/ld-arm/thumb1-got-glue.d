
.*:     file format .*


Disassembly of section \.text:

00008000 <__real_ok>:
    8000:	b508      	push	\{r3, lr\}
    8002:	bc08      	pop	\{r3\}
    8004:	bc01      	pop	\{r0\}
    8006:	4700      	bx	r0

00008008 <_start>:
    8008:	b508      	push	\{r3, lr\}
    800a:	4b04      	ldr	r3, \[pc, #16\].*
    800c:	447b      	add	r3, pc
    800e:	4a04      	ldr	r2, \[pc, #16\].*
    8010:	589b      	ldr	r3, \[r3, r2\]
    8012:	f000 f807 	bl	8024.*
    8016:	bc08      	pop	\{r3\}
    8018:	bc01      	pop	\{r0\}
    801a:	4700      	bx	r0
    801c:	00000ff0 	.*
    8020:	00000000 	.*
    8024:	4718      	bx	r3
    8026:	46c0      	nop			; .*

00008028 <ok>:
    8028:	e59fc000 	ldr	ip, \[pc\]	; .*
    802c:	e12fff1c 	bx	ip
    8030:	00008001 	.*

Disassembly of section \.got:

00009000 <_GLOBAL_OFFSET_TABLE_-0x4>:
    9000:	00008028 	.*

00009004 <_GLOBAL_OFFSET_TABLE_>:
.*
