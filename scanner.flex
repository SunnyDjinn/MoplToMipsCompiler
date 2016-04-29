
%{
/* This code goes at the "top" of the generated file. */
	#include <stdint.h>
	#include "parser.tab.h"
	#include "ast.h"
	int num_errors = 0;
	int line_num = 1;
	char currentSymbol[MAX_ID_SIZE];
	extern char* fileName;
%}

IF if
THEN then
ELSE else
WHILE while
DO do
READ read
WRITE write

AND "&"
OR "|"
NOT "!"
RETURN return

INT int

MULT "*"
PLUS "+"
DIV "/"
MINUS "-"

SEMICOLON ";"
COMMA ","

PAR_OP "("
PAR_CL ")"
BRACK_OP "{"
BRACK_CL "}"
SBRACK_OP "["
SBRACK_CL "]"

EQUAL "="
SMALLER "<"


ID_VAR \$([_[:alpha:]])+([_[:alnum:]])*
ID_FUNC ([_[:alpha:]])+([_[:alnum:]])*

COMMENT #.*

INTNUM ([[:digit:]])+([[:digit:]])*

%%

{IF} 		{return IF;}
{THEN} 		return THEN;
{ELSE} 		return ELSE;
{DO}		return DO;
{WHILE} 	return WHILE;
{READ} 		return READ;
{WRITE} 	return WRITE;


{AND} 		return AND;
{OR} 		return OR;
{NOT} 		return NOT;
{RETURN} 	return RETURN;
	
{INT} 		return INT;

{MULT} 		return MULT;
{PLUS} 		return PLUS;
{DIV} 		return DIV;
{MINUS} 	return MINUS;

{SEMICOLON}	return SEMICOLON;
{COMMA}		return COMMA;

{PAR_OP} 	return PAR_OP;
{PAR_CL} 	return PAR_CL;
{BRACK_OP} 	return BRACK_OP;
{BRACK_CL} 	return BRACK_CL;
{SBRACK_OP} 	return SBRACK_OP;
{SBRACK_CL} 	return SBRACK_CL;

{EQUAL} 	return EQUAL;
{SMALLER} 	return SMALLER;

{ID_FUNC} 	{strcpy(yylval.symbol, yytext); return ID_FUNC;}
{ID_VAR} 	{strcpy(yylval.symbol, yytext); return ID_VAR;}

{COMMENT}

{INTNUM} 	{yylval.ival = atoi(yytext); return INTNUM;}


" "
"\n"		++line_num;
"\t"
. 		{printf("%s:%d:syntax error at symbol \'%s\'\n", fileName, line_num, yytext); num_errors++;}

%%