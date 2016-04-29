#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"


int labelCount = 0;

void codeGeneration(astNode* root, symTab* symtab) {

	fprintf(yyoutasm, "\t.data\n");
	fprintf(yyoutasm, "_CR_:\t.asciiz\t\"\\n\"\n");


	semanticAnalysisAndTargetCodeGeneration(root, symtab);
	
	if(seekDeclaration("main", symtab) == 0) {	// Checking for a "main" function
		semanticError("No function \'main\' found in file.", "Aborting.");
	}

	fprintf(yyoutasm, "\t### EXIT\n");
	fprintf(yyoutasm, "\tli\t$v0, 10\t\t# Load 10 into $v0 for exit call \n");
	fprintf(yyoutasm, "\tsyscall\t\t\t# System call\n");
}


void scourChildren(astNode* root, symTab* symtab) {
	if(root == NULL)
		return;

	astNode* child = root->leftMostChild;

	while(child != NULL) {
		semanticAnalysisAndTargetCodeGeneration(child, symtab);
		child = child->rightSibling;
	}
}


void semanticAnalysisAndTargetCodeGeneration(astNode* root, symTab* symtab) {
	if(root == NULL)
		return;

	char* id;
	if(root->leftMostChild != NULL && root->leftMostChild->type == SYMBOL)
		id = root->leftMostChild->id;

	switch(root->type) {
		case STARTNODE:
			if(root->leftMostChild == NULL || root->leftMostChild->type == NULLNODE) { // No globals to declare
				fprintf(yyoutasm, "\t.text\n");
				fprintf(yyoutasm, "\tj main\t\t\t# Directly jump to label main -> where we usually enter the program\n");	
			}
			scourChildren(root, symtab);
		break;

		case LISTPARAM:
			scourChildren(root, symtab);
		break;

		case LISTVARDEC:
			scourChildren(root, symtab);
		break;

		case LISTDECFUNC:
			scourChildren(root, symtab);
		break;

		case VARDEC:
			semanticVarDec(root, symtab, id);
			symbol* variable = seekExecutionLocal(id, symtab);
			if(variable == NULL)
				variable = seekExecutionGlobal(id, symtab);
			codeGenVardec(variable, root, symtab);
		break;

		case DECFUNC:
			semanticDecFunc(root, symtab, id);
			codeGenDecFunc(root, symtab, id);
			scourChildren(root, symtab);
		break;

		case LISTINSTR:
			scourChildren(root, symtab);
		break;

		case AFFECT:
			codeGenAffect(root, symtab);
			scourChildren(root, symtab);
		break;

		case VAR:
			semanticVar(root, symtab, id);
		break;

		case FUNCCALL:
			semanticFuncCall(root, symtab, id);
			scourChildren(root, symtab);
		break;

		case IFSTMT:
			codeGenIfStmt(root, symtab);
		break;

		case WHILESTMT:
			codeGenWhileStmt(root, symtab);
		break;

		case WRITECALL:
			codeGenWriteCall(root, symtab);
		break;

		case EXPR:
			scourChildren(root, symtab);
		break;

		case NULLNODE:
			if(root->parent->type == LISTINSTR && root->parent->parent->type == DECFUNC)
				leaveFunction(symtab);
		break;

		case RETURNSTMT:
			codeGenReturn(root, symtab);
		break;

		case OPERATOR:
			scourChildren(root, symtab);
		break;

		case OPERATION:
			scourChildren(root, symtab);
		break;


		case NUMBER:
		case SYMBOL:
		case READCALL:
		case ARRSIZE:
		default:
		break;
	}
}


void codeGenAffect(astNode* root, symTab* symtab) {
	astNode* child = root->leftMostChild;
	fprintf(yyoutasm, "\t## Affectation\n");
	if(child != NULL && child->rightSibling != NULL && child->rightSibling->type == EXPR) {
		short offset = offsetVariable(child);
		if(offset >= 0) {
			if(computeExpression(child->rightSibling, symtab) >= 0) { 	// Compute expression pushes result on the stack
				stackToVar(child->leftMostChild->id, offset, symtab);	// Pop stack to variable 
			}
		}
	}
	fprintf(yyoutasm, "\t## End affectation\n");
}


void codeGenVardec(symbol* variable, astNode* root, symTab* symtab) {
	if(variable == NULL)
		return;
	/* Global variables allocation */
	if(variable->scope == S_GLOBAL) {
		if(variable->type == T_INT) {
			// print word
			fprintf(yyoutasm, "%s:\t.word %d\n", variable->id+1, 0);	// 0 is the initialization
		} else if(variable->type == T_ARRAY) {
			// print words of size array
			fprintf(yyoutasm, "%s:\t.word 0", variable->id+1);
			for(int i = 0; i < variable->argNb-1; i++)
				fprintf(yyoutasm, ", 0");
			fprintf(yyoutasm, "\n");
		}
		if(root->rightSibling != NULL && root->rightSibling->type == NULLNODE) { // End of globals declaration
			fprintf(yyoutasm, "\t.text\n");
			fprintf(yyoutasm, "\tj main\t\t\t# Directly jump to label main -> where we usually enter the program\n");
			fprintf(yyoutasm, "\tnop\n");
		}
	} /* Local avriables allocation on stack */
	else if(variable->scope != S_ARG){ 
		if(variable->type == T_ARRAY)
			pushStack(variable->argNb, symtab);
		else
			pushStack(1, symtab);
	}
}


void codeGenDecFunc(astNode* root, symTab* symtab, char* id) {
	fprintf(yyoutasm, "%s:\n", id);	// Print label of the function in output file
	symtab->sp = 0;	// Reset stack allocation counter

	fprintf(yyoutasm, "\tmove\t$t4, $sp\t# Temporarily store $sp into $t4\n");

	pushStackReg("$fp", symtab);	// Save value of caller's fp
	pushStackReg("$ra", symtab);	// Save value of caller's ra
	pushStackReg("$t4", symtab);	// Save value of caller's sp

	fprintf(yyoutasm, "\tmove\t$fp, $sp\t# Set new value of $fp to $sp\n");
}


void codeGenFuncCall(astNode* root, symTab* symtab, char* id) {
	astNode* child = root->leftMostChild;
	short argNb = 0;
	pushStack(1, symtab); 	// Word reserved on the stack for return value 

	while(child->rightSibling != NULL) {
		if(child->rightSibling->type == EXPR) {	// Evaluate expressions of each parameter and push them onto the stack 
			computeExpression(child->rightSibling, symtab);
			argNb++;
		}
		child = child->rightSibling;
	}

	fprintf(yyoutasm, "\tjal %s\t\t# Jump to function %s and return address is next instruction\n", id, id);	// Inconditional jump to callee function
	fprintf(yyoutasm, "\tnop\n");
	popStack(argNb, symtab);	// Remove arguments. Top of the stack is now returned value
}

void codeGenReturn(astNode* root, symTab* symtab) {
	if(computeExpression(root->leftMostChild, symtab) >= 0) {
		popStackReg("$t0", symtab);
		fprintf(yyoutasm, "\tsw\t$t0, %d($sp)\t# Store $t0 into return value allocated space in $sp\n", (symtab->sp + symtab->current->argNb)*4);
	}

	fprintf(yyoutasm, "\tlw\t$fp, %d($sp)\t# restore the previous value of fp from $sp\n", (symtab->sp-1) * 4);	// Restore value of fp
	fprintf(yyoutasm, "\tlw\t$ra, %d($sp)\t# restore the previous value of ra from $sp\n", (symtab->sp-2) * 4);	// Restore value of ra
	fprintf(yyoutasm, "\tlw\t$sp, %d($sp)\t# restore the previous value of sp from $sp\n", (symtab->sp-3) * 4);	// Restore value of sp
	fprintf(yyoutasm, "\tjr $ra\t\t\t# Jump back to calling function\n");	// Back to caller 
	fprintf(yyoutasm, "\tnop\n");
}

void codeGenIfStmt(astNode* root, symTab* symtab) {
	astNode* child = root->leftMostChild;
	if(child == NULL)
		return;
	short ifLabelNb = labelCount++;

	fprintf(yyoutasm, "\t## If Statement\n");
	if(computeExpression(child, symtab) >= 0) {
		popStackReg("$t0", symtab);

		if(child->rightSibling->rightSibling == NULL || child->rightSibling->rightSibling->type == NULLNODE) { // No "else" stmt in IFSTMT
			fprintf(yyoutasm, "\tbeq\t$t0, $0, endif%d\n", ifLabelNb); // Jump to endif if expr false
			fprintf(yyoutasm, "\tnop\n");
			semanticAnalysisAndTargetCodeGeneration(child->rightSibling, symtab);

			fprintf(yyoutasm, "endif%d:\n", ifLabelNb);	// Label endif after if's instructions
		}
		else {	// There is an else stmt in IFSTMT
			fprintf(yyoutasm, "\tbeq\t$t0, $0, else%d\n", ifLabelNb); // Jump to else if expr false
			fprintf(yyoutasm, "\tnop\n");
			semanticAnalysisAndTargetCodeGeneration(child->rightSibling, symtab);


			fprintf(yyoutasm, "\tj endif%d\n", ifLabelNb);	// Inconditional jump to end of if (after the else)
			fprintf(yyoutasm, "\tnop\n");
			fprintf(yyoutasm, "else%d:\n", ifLabelNb);	// Label else after true if's instructions

			semanticAnalysisAndTargetCodeGeneration(child->rightSibling->rightSibling, symtab);

			fprintf(yyoutasm, "endif%d:\n", ifLabelNb);	// Label endif after all if's instructions
		}
		fprintf(yyoutasm, "\t## End If statement\n");
	}
}


void codeGenWhileStmt(astNode* root, symTab* symtab) {
	astNode* exprNode = root->leftMostChild;

	fprintf(yyoutasm, "\t## While Statement\n");
	short whileLabelNb = labelCount++;

	fprintf(yyoutasm, "while%d:\n", whileLabelNb);
	if(computeExpression(exprNode, symtab) >= 0) {
		popStackReg("$t0", symtab);

		fprintf(yyoutasm, "\tbeq\t$t0, $0, endWhile%d\n", whileLabelNb); // Jump to endif if expr false
		fprintf(yyoutasm, "\tnop\n");
		/* Compute the list of instructions - to be removed */
		semanticAnalysisAndTargetCodeGeneration(exprNode->rightSibling, symtab);

		fprintf(yyoutasm, "\tj while%d\n", whileLabelNb);	// Inconditional jump to start of while
		fprintf(yyoutasm, "\tnop\n");
		fprintf(yyoutasm, "endWhile%d:\n", whileLabelNb);

	}
	fprintf(yyoutasm, "\t## End While Statement\n");
}


void codeGenWriteCall(astNode* root, symTab* symtab) {
	fprintf(yyoutasm, "\t## Write call\n");
	computeExpression(root->leftMostChild, symtab);
	popStackReg("$t0", symtab);
	fprintf(yyoutasm, "\tmove\t$a0, $t0\t# Move $t0 into $a0\n");
	fprintf(yyoutasm, "\tli\t$v0, 1\t\t# Load 1 into $v0 for write call\n");
	fprintf(yyoutasm, "\tsyscall\t\t\t# System call\n");
	/* Carriage return */
	fprintf(yyoutasm, "\tla\t$a0, _CR_\t# Move carriage return into $a0\n");
	fprintf(yyoutasm, "\tli\t$v0, 4\t\t# Load 4 into $v0 for write call\n");
	fprintf(yyoutasm, "\tsyscall\t\t\t# System call\n");
	fprintf(yyoutasm, "\t## End write call\n");
}


/* Pushes local variable var node on top of the stack */
int varToStack(astNode* varNode, symTab* symtab) {
	char* id = varNode->leftMostChild->id;
	symbol* sym = seekExecutionLocal(id, symtab);
	if(sym == NULL) {
		sym = seekExecutionGlobal(id, symtab);
		if(sym == NULL) {
			printf("\n\nVAR DUMPED %s \n\n", id);
			return -1;
		}
		else {
			int offset = offsetVariable(varNode);
			if(offset < 0)
				return -1;
			fprintf(yyoutasm, "\tla\t$t0, %s\t# Load address of %s into $t0\n", id+1, id+1);
			fprintf(yyoutasm, "\tlw\t$t0, %d($t0)\t# Load value of %s[%d] into $t0\n", offset*4, id+1, offset);
			pushStackReg("$t0", symtab);
			return 0;
		}
	}
	int offset = offsetVariable(varNode);
	if(offset < 0)
		return -1;
	if(sym->scope == S_ARG)
		fprintf(yyoutasm, "\tlw\t$t0, %d($sp)\t# Load value of %s[%d] into $t0\n", (symtab->sp-(sym->address+offset+1))*4, id, offset);
	else
		fprintf(yyoutasm, "\tlw\t$t0, %d($sp)\t# Load value of %s[%d] into $t0\n", (symtab->sp-(sym->address+offset+1)-3)*4, id, offset);
	pushStackReg("$t0", symtab);
	return 0;
}

int stackToVar(char* id, int offset, symTab* symtab) {
	symbol* sym = seekExecutionLocal(id, symtab);
	if(sym == NULL) {
		sym = seekExecutionGlobal(id, symtab);
		if(sym == NULL)
			return -1;
		else {
			popStackReg("$t0", symtab);
			fprintf(yyoutasm, "\tla\t$t1, %s\t# Load address of %s into $t1\n", id+1, id+1);
			fprintf(yyoutasm, "\tsw\t$t0, %d($t1)\t# Store $t0 into %s[%d]\n", offset*4, id+1, offset);
			return 0;
		}
	}
	popStackReg("$t0", symtab);
	if(sym->scope == S_ARG)
		fprintf(yyoutasm, "\tsw\t$t0, %d($sp)\t# Store value of $t0 into %s[%d]\n", (symtab->sp-(sym->address+offset+1))*4, id, offset);
	else
		fprintf(yyoutasm, "\tsw\t$t0, %d($sp)\t# Store value of $t0 into %s[%d]\n", (symtab->sp-(sym->address+offset+1)-3)*4, id, offset);
	return 0;
}


int offsetVariable(astNode* varNode) {
	astNode* child = varNode->leftMostChild;
	if(child->rightSibling == NULL || child->rightSibling->type == NULLNODE) {
		return 0;
	}
	else if(child->rightSibling->type == EXPR || child->rightSibling->type == OPERATION) {
		return child->rightSibling->leftMostChild->intVal;
	}
	else
		return -1;
}

int computeExpression(astNode* exprNode, symTab* symtab) {

	if(exprNode->leftMostChild == NULL) 
		return -1;

	astNode* child = exprNode->leftMostChild;

	if(child->type == OPERATOR || child->rightSibling->type == OPERATOR)
		return computeOperation(exprNode, symtab);

	else if(child->type == VAR) {
		return(varToStack(child, symtab));

	} else if(child->type == READCALL) {
		fprintf(yyoutasm, "\t# Read call\n");
		fprintf(yyoutasm, "\tli\t$v0, 5\t\t# Load 5 into $v0 for read call\n");
		fprintf(yyoutasm, "\tsyscall\t\t\t# System call for read call\n");
		fprintf(yyoutasm, "\t# End read call\n");
		pushStackReg("$v0", symtab);
		return 0;

	} else if(child->type == FUNCCALL) {
		codeGenFuncCall(child, symtab, child->leftMostChild->id);
		return 0;
	}

	if(child->rightSibling == NULL)
		return -1;

	if(child->rightSibling->type == OPERATOR || child->type == OPERATOR) {
		return computeOperation(exprNode, symtab);
	}

	if(child->type == NUMBER) { // Loading and returning number
		/* Loads left child in $t1 */
		fprintf(yyoutasm, "\tli\t$t0, %d\t\t# Load immediate %d into $t0\n", child->intVal, child->intVal);
		pushStackReg("$t0", symtab);

		if(child->rightSibling == NULL || child->rightSibling->type == NULLNODE) {
			return 0;

		} else if(child->rightSibling->type == EXPR || child->rightSibling->type == OPERATION) {
			return(computeExpression(child->rightSibling, symtab));
		}
	} 

	return -1;
}

int computeOperation(astNode* exprNode, symTab* symtab) {
	astNode* lhs = exprNode->leftMostChild;

	if(lhs != NULL && lhs->type == OPERATOR)
		return(computeNegation(lhs, symtab));

	astNode* operator = lhs->rightSibling;
	astNode* rhs = operator->leftMostChild;
	short rhsReg = -1;
	short lhsReg = -1;

	/* Left Hand Side */
	if (lhs == NULL || lhs->type == NULLNODE) {
		lhsReg = -1;

	} else if(lhs->type == EXPR || lhs->type == OPERATION) {
		if(computeExpression(lhs, symtab) >= 0) {
			lhsReg = 0;
		}

	} else if(lhs->type == NUMBER) { // Loading number into temp
		fprintf(yyoutasm, "\tli\t$t0, %d\t\t# Load %d into $t0\n", lhs->intVal, lhs->intVal);
		lhsReg = 0;
		pushStackReg("$t0", symtab);

	} else if (lhs->type == VAR){
		varToStack(lhs, symtab);
		lhsReg = 0;
	}

	/* Right Hand Side */
	if (rhs == NULL || rhs->type == NULLNODE) {
		rhsReg = -1;
	}
	else if(rhs->type == EXPR || rhs->type == OPERATION) {
		if(computeExpression(rhs, symtab) >= 0) {
			rhsReg = 1;
		}

	} else if(rhs->type == NUMBER) { // Loading number into temp
		fprintf(yyoutasm, "\tli\t$t1, %d\n", rhs->intVal);
		rhsReg = 1;
		pushStackReg("$t1", symtab);

	} else if (rhs->type == VAR){
		varToStack(rhs, symtab);
		rhsReg = 1;
	}


	fprintf(yyoutasm, "\t# Operation\n");


	if(lhsReg < 0 && rhsReg < 0) {
		fprintf(yyoutasm, "\t# End operation\n");
		return 0;
	}
	else if(lhsReg < 0) {
		fprintf(yyoutasm, "\t# End operation\n");
		return 1;
	}
	else if(rhsReg < 0) {
		fprintf(yyoutasm, "\t# End operation\n");
		return 1;
	}
	else {
		popStackReg("$t1", symtab);	// RHS
		popStackReg("$t0", symtab);	// LHS
		switch(operator->operator) {
			case '+':
				fprintf(yyoutasm, "\tadd\t$t2, $t0, $t1\t# Add $t0 and $t1 and store in $t2\n");
			break;
			case '-':
				fprintf(yyoutasm, "\tsub\t$t2, $t0, $t1\t# Subtract $t0 and $t1 and store in $t2\n");
			break;
			case '*':
				fprintf(yyoutasm, "\tmult\t$t0, $t1\t# Multiply $t0 and $t1\n");
				fprintf(yyoutasm, "\tmflo\t$t2\t# Store result in $t2\n");
			break;
			case '/':
				fprintf(yyoutasm, "\tdiv\t$t0, $t1\t# Divide $t0 and $t1\n");
				fprintf(yyoutasm, "\tmflo\t$t2\t# Store result in $t2\n");
			break;
			case '<':
				fprintf(yyoutasm, "\tli\t$t2, -1\n");	// True
				fprintf(yyoutasm, "\tblt\t$t0, $t1, e%d\n", labelCount);	// If lhs < rhs, jump next stmt
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tli\t$t2, 0\n");	// False
				fprintf(yyoutasm, "e%d:\n", labelCount++);
			break;
			case '=':
				fprintf(yyoutasm, "\tli\t$t2, -1\n");	// True
				fprintf(yyoutasm, "\tbeq\t$t0, $t1, e%d\n", labelCount);	// If lhs = rhs, jump next stmt
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tli\t$t2, 0\n");	// False
				fprintf(yyoutasm, "e%d:\n", labelCount++);
			break;
			case '&':
				fprintf(yyoutasm, "\tli\t$t2, 0\n");	// False
				fprintf(yyoutasm, "\tbeq\t$t0, $0, e%d\n", labelCount);	// If lhs = 0 (false), we jump over the whole stmt and evaluate to false
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tbeq\t$t1, $0, e%d\n", labelCount);	// Otherwise, lhs is true and we evaluate rhs
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tli\t$t2, -1\n");	// True				
				fprintf(yyoutasm, "e%d:\n", labelCount++);
			break;
			case '|':
				fprintf(yyoutasm, "\tli\t$t2, -1\n");	// True
				fprintf(yyoutasm, "\tbne\t$t0, $0, e%d\n", labelCount);	// If lhs != 0 (true), we jump over the whole stmt and evaluate to true
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tbne\t$t1, $0, e%d\n", labelCount);	// Otherwise, lhs is false and we evaluate rhs
				fprintf(yyoutasm, "\tnop\n");
				fprintf(yyoutasm, "\tli\t$t2, 0\n");	// False				
				fprintf(yyoutasm, "e%d:\n", labelCount++);
			break;

			default:
			break;
		}
	}
	pushStackReg("$t2", symtab);				// Push on stack
	fprintf(yyoutasm, "\t# End operation\n");

	return 1;
}

int computeNegation(astNode* operator, symTab* symtab) {
	astNode* child = operator->leftMostChild;
	if(child == NULL || child->type == NULLNODE)
		return -1;
	else if(child->type == EXPR || OPERATION) {
		if(computeExpression(child, symtab) >= 0) {
			popStackReg("$t0", symtab);	// Pop previous result 

			fprintf(yyoutasm, "\tli\t$t1, 0\n");	// False
			fprintf(yyoutasm, "\tbne\t$t0, $0, e%d\n", labelCount);
			fprintf(yyoutasm, "\tnop\n");
			fprintf(yyoutasm, "\tli\t$t1, -1\n");	// False
			fprintf(yyoutasm, "e%d:\n", labelCount++);

			pushStackReg("$t1", symtab);	// Push result of negation onto the stack
			return 1;
		}	
	}
	return -1;
}


void pushStackReg(char* reg, symTab* symtab) {
	pushStack(1, symtab);
	fprintf(yyoutasm, "\tsw\t%s, ($sp)\t# Store %s on the stack\n", reg, reg);
}


void popStackReg(char* reg, symTab* symtab) {
	fprintf(yyoutasm, "\tlw\t%s, ($sp)\t# Load top of the stack in %s\n", reg, reg);
	popStack(1, symtab);
}

void pushStack(int nb, symTab* symtab) {
	fprintf(yyoutasm, "\tsubu\t$sp, $sp, %d\t# Allocate a new word on the stack, nbAlloc = %d\n", 4*nb, symtab->sp + nb);
	symtab->sp += nb;
}

void popStack(int nb, symTab* symtab) {
	fprintf(yyoutasm, "\taddu\t$sp, $sp, %d\t# Free a word from the stack, nbAlloc = %d\n", 4*nb, symtab->sp - nb);
	symtab->sp -= nb;
}