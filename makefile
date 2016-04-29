#/bin/bash

CC=gcc -c -g -Wall  

LD=gcc -o

all: clean compiler

compiler: parser.tab.h parser.tab.o lex.yy.o ast.o symbolsTable.o codeGen.o ast.h symbolsTable.h
	$(LD) compiler parser.tab.o lex.yy.o ast.o symbolsTable.o codeGen.o -lfl

parser.tab.o: parser.tab.c
	$(CC) parser.tab.c

ast.o: ast.c ast.h
	$(CC) ast.c

codeGen.o: codeGen.c codeGen.h
	$(CC) codeGen.c

symbolsTable.o: symbolsTable.c symbolsTable.h
	$(CC) symbolsTable.c

parser.tab.h:
	bison -d parser.y

parser.tab.c:
	bison -d parser.y
	
lex.yy.o: lex.yy.c
	$(CC) lex.yy.c

lex.yy.c:
	flex scanner.flex


clean:
	rm parser.tab.c
	rm parser.tab.h
	rm lex.yy.c
	rm *.o
	rm compiler
