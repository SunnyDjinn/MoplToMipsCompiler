#ifndef __AST_H__
#define __AST_H__

#include <string.h>
#define MAX_ID_SIZE 50
#define EMPTY '\0'

/* 
 * AST node types
 */
#define NULLNODE 0
#define NUMBER 1
#define OPERATOR 2
#define SYMBOL 3
#define EXPR 4
#define OPERATION 5
#define READCALL 6
#define WRITECALL 7
#define RETURNSTMT 8
#define STARTNODE 9
#define LISTVARDEC 10
#define VARDEC 11
#define ARRSIZE 12
#define DECFUNC 13
#define LISTDECFUNC 14
#define LISTPARAM 15
#define AFFECT 16
#define LISTINSTR 17
#define IFSTMT 18
#define WHILESTMT 19
#define VAR 20
#define FUNCCALL 21


typedef struct ast {
	struct ast *parent;
	struct ast *leftMostChild;
	struct ast *leftMostSibling;
	struct ast *rightSibling;

	int intVal;
	char operator;
	char id[MAX_ID_SIZE];

	int type;

	int tableEntry;
}astNode;

astNode* makeNode(int nodeType, int value, char operator, char* id);
astNode* makeSiblings(astNode* x, astNode* y);
astNode* makeLeftSibling(astNode* x, astNode* y);
astNode* adoptChildren(astNode* x, astNode* y);
astNode* adoptLeftChildren(astNode* x, astNode* y);
void displayAstTree(astNode* root);
int freeAstTree(astNode* root);
void scourAndWriteASTTree(astNode* root, FILE* fp);
void refineAstTree(astNode* root);
int numberOfChildren(astNode* root);
int drawAstTree(astNode* root);


#endif
