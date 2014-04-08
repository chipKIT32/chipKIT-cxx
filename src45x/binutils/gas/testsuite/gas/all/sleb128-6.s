.text
.globl foo
foo:
.L1:
.byte 0
.byte 0
.byte 0
.L2:

.data
bar:
.sleb128 1<<63+(.L2 - .L1)
.byte 42
.sleb128 1<<63+(.L1 - .L2)
.byte 42
.sleb128 (.L2 - .L1)+1<<63
.byte 42
.sleb128 (.L1 - .L2)+1<<63
.byte 42
