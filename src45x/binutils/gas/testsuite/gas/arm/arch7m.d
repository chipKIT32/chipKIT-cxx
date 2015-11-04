# name: SVC instructions on ARMv7-M
# objdump: -dr --prefix-addresses --show-raw-insn

.*: +file format .*arm.*

Disassembly of section \.text:
0+000 <[^>]+> df5a      	(swi|svc)	90.*
0+002 <[^>]+> dfa5      	(swi|svc)	165.*
0+004 <[^>]+> df5a      	(swi|svc)	90.*
0+006 <[^>]+> dfa5      	(swi|svc)	165.*
