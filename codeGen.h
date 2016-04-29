#ifndef __CODE_GEN_H__
#define __CODE_GEN_H__

#include <stdio.h>
#include <stdlib.h>
#include "symbolsTable.h"

FILE* yyoutasm;

void codeGeneration(astNode* root, symTab* symtab);
void semanticAnalysisAndTargetCodeGeneration(astNode* root, symTab* symtab);

void codeGenAffect(astNode* root, symTab* symtab);
void codeGenVardec(symbol* variable, astNode* root, symTab* symtab);
void codeGenDecFunc(astNode* root, symTab* symtab, char* id);
void codeGenFuncCall(astNode* root, symTab* symtab, char* id);
void codeGenReturn(astNode* root, symTab* symtab);
void codeGenIfStmt(astNode* node, symTab* symtab);
void codeGenWhileStmt(astNode* node, symTab* symtab);
void codeGenWriteCall(astNode* root, symTab* symtab);

int offsetVariable(astNode* varNode);
int computeExpression(astNode* exprNode, symTab* symtab);
int computeOperation(astNode* exprNode, symTab* symtab);
int computeNegation(astNode* lhs, symTab* symtab);

void pushStackReg(char* reg, symTab* symtab);
void popStackReg(char* reg, symTab* symtab);
void pushStack(int nb, symTab* symtab);
void popStack(int nb, symTab* symtab);

int varToStack(astNode* varNode, symTab* symtab);
int stackToVar(char* id, int offset, symTab* symtab);

#endif