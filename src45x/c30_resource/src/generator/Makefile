
OBJS = lex.yy.o grammar.tab.o main.o resource.o
READER_OBJS = reader.o resource.o

CC=gcc
INCLUDE= -I. -I../c30 -I../xc32
CFLAGS= -g $(INCLUDE) $(PIC32)

rg:	$(OBJS) reader
	gcc -o $@ $(OBJS)

reader: $(READER_OBJS)

grammar.tab.c: grammar.y grammar.h
	bison -v -t -d `basename $<`

lex.yy.c: grammar.l grammar.tab.c
	flex -f `basename $<`

clean: 
	rm lex.yy.o grammar.tab.o main.o resource.o reader.o	
