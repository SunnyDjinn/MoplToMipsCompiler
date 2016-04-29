%code top {
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include "parser.tab.h"
	#include "codeGen.h"

	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern FILE* yyoutasm;

	void yyerror(const char *);
	extern int line_num;
	extern int num_errors;
	extern char * yytext;
	char fileName[50];
	
}

%code requires{
	#include "ast.h"
}

%union {
  int ival;
  char symbol[MAX_ID_SIZE];
  astNode* node;
}

%code {
	astNode* astRoot = NULL;
}

%token IF
%token THEN
%token ELSE
%token WHILE
%token DO
%token READ
%token WRITE

%token AND
%token OR
%token NOT
%token RETURN

%token INT

%token MULT
%token PLUS
%token DIV
%token MINUS

%token SEMICOLON
%token COMMA

%token PAR_OP
%token PAR_CL
%token BRACK_OP
%token BRACK_CL
%token SBRACK_OP
%token SBRACK_CL

%token EQUAL
%token SMALLER


%token ID_VAR
%token ID_FUNC


%token  INTNUM


%type <node> Program OptVarDec ListVarDec ListVarDec2 VarDec OptArrSize ListDecFuncs DecFunc ListParam OptListVarDec I IAffect IBlock ListI IIf OptElse IWhile ICall IReturn IWrite IEmpty Expression Expression2 Operator Operator2 Negation Comparison Comparison2 ArithExp ArithExp2 Term Term2 Factor Var OptIndex FuncCall ListExpr ListExpr2


%right	EQUAL
%left	PLUS MINUS
%left	MULT DIV

%%

Program: 	OptVarDec ListDecFuncs {astRoot = adoptChildren(makeNode(STARTNODE, 0, EMPTY, NULL), makeSiblings($1, $2)); $$ = astRoot;}
		;

OptVarDec:	ListVarDec SEMICOLON {$$ = $1;}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

ListVarDec:	VarDec ListVarDec2 {$$ = adoptChildren(makeNode(LISTVARDEC, 0, EMPTY, NULL), makeSiblings($1, $2));}
		;

ListVarDec2:	COMMA VarDec ListVarDec2 {$$ = makeSiblings($2, $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

VarDec:		INT ID_VAR OptArrSize {$$ = adoptChildren(makeNode(VARDEC, 0, EMPTY, NULL), makeSiblings(makeNode(SYMBOL, 0, EMPTY, $<symbol>2), $3));}
		;

OptArrSize: 	SBRACK_OP INTNUM SBRACK_CL {$$ = makeNode(ARRSIZE, $<ival>2, EMPTY, NULL);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

ListDecFuncs:	DecFunc ListDecFuncs {$$ = adoptChildren(makeNode(LISTDECFUNC, 0, EMPTY, NULL), makeSiblings($1, $2));}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

DecFunc:	ID_FUNC ListParam OptVarDec IBlock {$$ = adoptChildren(makeNode(DECFUNC, 0, EMPTY, NULL), makeSiblings(makeNode(SYMBOL, 0, EMPTY, $<symbol>1), makeSiblings($2, makeSiblings($3, $4))));}
		;

ListParam:	PAR_OP OptListVarDec PAR_CL {$$ = adoptChildren(makeNode(LISTPARAM, 0, EMPTY, NULL), $2);}
		;

OptListVarDec:	ListVarDec {$$ = $1;}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

I:		IAffect {$$ = $1;}
		| IBlock {$$ = $1;}
		| IIf {$$ = $1;}
		| IWhile {$$ = $1;}
		| ICall {$$ = $1;}
		| IReturn {$$ = $1;}
		| IWrite {$$ = $1;}
		| IEmpty {$$ = $1;}
		;

IAffect:	Var EQUAL Expression SEMICOLON {$$ = adoptChildren(makeNode(AFFECT, 0, EMPTY, NULL), makeSiblings($1, $3));}
		;

IBlock:		BRACK_OP ListI BRACK_CL {$$ = adoptChildren(makeNode(LISTINSTR, 0, EMPTY, NULL), $2);}
		;

ListI:		I ListI {$$ = makeSiblings($1, $2);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

IIf:		IF Expression THEN IBlock OptElse {$$ = adoptChildren(makeNode(IFSTMT, 0, EMPTY, NULL), makeSiblings($2, makeSiblings($4, $5)));}
		;

OptElse:	ELSE IBlock {$$ = $2;}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

IWhile:		WHILE Expression DO IBlock {$$ = adoptChildren(makeNode(WHILESTMT, 0, EMPTY, NULL), makeSiblings($2, $4));}
		;

ICall:		FuncCall SEMICOLON {$$ = $1;}
		;

IReturn:	RETURN Expression SEMICOLON {$$ = adoptChildren(makeNode(RETURNSTMT, 0, EMPTY, NULL), $2);}
		;

IWrite:		WRITE PAR_OP Expression PAR_CL SEMICOLON {$$ = adoptChildren(makeNode(WRITECALL, 0, EMPTY, NULL), $3);}
		;

IEmpty:		SEMICOLON {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

Expression:	Operator Expression2 {$$ = adoptChildren(makeNode(EXPR, 0, EMPTY, NULL), makeSiblings($1, $2));}
		;

Expression2:	OR Operator Expression2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '|', NULL), $2), $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

Operator:	Negation Operator2 {$$ = adoptChildren(makeNode(EXPR, 0, EMPTY, NULL), makeSiblings($1, $2));}
		;

Operator2: 	AND Negation Operator2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '&', NULL), $2), $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

Negation:	NOT ArithExp Comparison2 {$$ = adoptChildren(makeNode(EXPR, 0, EMPTY, NULL), makeSiblings(adoptChildren(makeNode(EXPR, 0, EMPTY, NULL), adoptChildren(makeNode(OPERATOR, 0, '!', NULL), $2)), $3));}
		| Comparison {$$ = $1;}
		;

Comparison:	ArithExp Comparison2 {$$ = adoptChildren(makeNode(EXPR, 0, EMPTY, NULL), makeSiblings($1, $2));}
		;

Comparison2:	EQUAL ArithExp Comparison2 {$$ = adoptChildren(makeNode(OPERATOR, 0, '=', NULL), makeSiblings($2, $3));}
		| SMALLER ArithExp Comparison2 {$$ = adoptChildren(makeNode(OPERATOR, 0, '<', NULL), makeSiblings($2, $3));}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

ArithExp:	Term ArithExp2 {$$ = adoptChildren(makeNode(OPERATION, 0, EMPTY, NULL), makeLeftSibling($2, $1));}
		;

ArithExp2:	PLUS Term ArithExp2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '+', NULL), $2), $3);}
		| MINUS Term ArithExp2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '-', NULL), $2), $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

Term:		Factor Term2 {$$ = adoptChildren(makeNode(OPERATION, 0, EMPTY, NULL), makeLeftSibling($2, $1));}
		;

Term2:		MULT Factor Term2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '*', NULL), $2), $3);}
		| DIV Factor Term2 {$$ = makeSiblings(adoptChildren(makeNode(OPERATOR, 0, '/', NULL), $2), $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

Factor:		PAR_OP Expression PAR_CL {$$ = $2;}
		| INTNUM {$$ = makeNode(NUMBER, $<ival>1, EMPTY, NULL);}
		| FuncCall {$$ = $1;}
		| Var {$$ = $1;}
		| READ PAR_OP PAR_CL {$$ = makeNode(READCALL, 0, EMPTY, NULL);}
		;

Var:		ID_VAR OptIndex {$$ = adoptChildren(makeNode(VAR, 0, EMPTY, NULL), makeSiblings(makeNode(SYMBOL, 0, EMPTY, $<symbol>1), $2));}
		;

OptIndex:	SBRACK_OP Expression SBRACK_CL {$$ = $2;}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

FuncCall:	ID_FUNC PAR_OP ListExpr PAR_CL {$$ = adoptChildren(makeNode(FUNCCALL, 0, EMPTY, NULL), makeSiblings(makeNode(SYMBOL, 0, EMPTY, $<symbol>1), $3));}
		;

ListExpr:	Expression ListExpr2 {$$ = makeSiblings($1, $2);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;

ListExpr2:	COMMA Expression ListExpr2 {$$ = makeSiblings($2, $3);}
		| {$$ = makeNode(NULLNODE, 0, EMPTY, NULL);}
		;


%%

int main(int argc, char* argv[]) {
	if(argc < 3) {
		perror("compiler: Usage: ./compiler input.mopl outputfilename");
		exit(EXIT_FAILURE);
	}
	/* Input file opening */
	yyin = fopen(argv[1], "r");
	if(yyin == NULL){
		perror("Cannot open file");
		exit(1);
	}
	strcpy(fileName, argv[1]);
	
	/* Output file opening */
	char outputfile[MAX_ID_SIZE];
	strcpy(outputfile, argv[2]);
	strcat(outputfile, ".asm");
	yyoutasm = fopen(outputfile, "w");

	if(yyoutasm == NULL){
		perror("Cannot create file");
		exit(1);
	}

	/* Parsing */
	yyparse();

	refineAstTree(astRoot);
	drawAstTree(astRoot);
	symTab* symtab = makeSymTab();

	/* Semantic analysis and code generation */
	codeGeneration(astRoot, symtab);

	/* Free memory */
	freeSymTable(symtab->start, symtab);
	free(symtab);
	freeAstTree(astRoot);
	fclose(yyin);
	fclose(yyoutasm);
	yylex_destroy();

	/* End message */
	if(num_errors == 0)
		printf("Compiling done with success\n");
	else {
		printf("Compiling: %d errors\n", num_errors);

		yyoutasm = fopen(outputfile, "w");	/* Flushing output file if errors found */
		fclose(yyoutasm);
	return 1;

	}
	return 0;
}

void yyerror(const char *s) {
	//extern char * yytext;
	num_errors++;
	printf("%s:%d:%s at symbol \'%s\'\n", fileName, line_num, s, yytext);
	freeAstTree(astRoot);
	fclose(yyin);
	yylex_destroy();
	exit(1);
}
