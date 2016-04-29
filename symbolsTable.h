#ifndef __SYMBOLS_TABLE_H__
#define __SYMBOLS_TABLE_H__

#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

#define T_INT 1
#define T_FLOAT 2
#define T_ARRAY 3
#define T_FUNC 4

#define S_UNDEF -1
#define S_GLOBAL 0
#define S_LOCAL 1
#define S_ARG 2

extern int num_errors;
extern char fileName[50];


typedef struct sym{
	struct sym* next;

	short scope;
	char* id;

	short type;
	int address;

	short argNb;

}symbol;

typedef struct symTab{
	symbol* start;
	symbol* current;
	symbol* top;

	int address;
	int sp;
}symTab;

void semanticError(const char* s, const char* symId);
void semanticVarDec(astNode* root, symTab* symtab, char* id);
void semanticVar(astNode* root, symTab* symtab, char* id);
void semanticDecFunc(astNode* root, symTab* symtab, char* id);
void semanticFuncCall(astNode* root, symTab* symtab, char* id);

symbol* makeSymbol(short scope, char* id, short type, int address, int argNb);
symTab* makeSymTab();
int verifyMain(symTab* symtab);
void drawSymTab(symTab* symtab);
short seekDeclaration(char* id, symTab* symtab);
symbol* seekExecutionLocal(char* id, symTab* symtab);
symbol* seekExecutionGlobal(char* id, symTab* symtab);
symbol* addSymbol(symTab* symtab, symbol* symbol);
void enterFunction(symTab* symtab);
void leaveFunction(symTab* symtab);
void freeSymTable(symbol* node, symTab* symtab);




#endif