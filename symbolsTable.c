#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbolsTable.h"

void semanticError(const char* s, const char* symId) {
	num_errors++;
	printf("%s:Semantic error: %s \'%s\'\n", fileName, s, symId);
}

void semanticVarDec(astNode* root, symTab* symtab, char* id) {
	astNode* child = root->leftMostChild;

	if(seekDeclaration(id, symtab) == 1) {
		semanticError("Variable already declared in same scope", id);
	} else { // Checking whether it is a parameter or just a simple variable
		if(root->parent->parent->type == LISTPARAM) { // parameter
			if(child->rightSibling->type == ARRSIZE) { // parameter and array
				addSymbol(symtab, makeSymbol(S_ARG, id, T_ARRAY, symtab->address, child->rightSibling->intVal));
			} else { // parameter and int
				addSymbol(symtab, makeSymbol(S_ARG, id, T_INT, symtab->address, 0));
			}
		} else { // not a parameter
			if(child->rightSibling->type == ARRSIZE) { // simple variable and array
				if(root->parent->parent->type == STARTNODE)
					addSymbol(symtab, makeSymbol(S_GLOBAL, id, T_ARRAY, symtab->address, child->rightSibling->intVal));
				else
					addSymbol(symtab, makeSymbol(S_LOCAL, id, T_ARRAY, symtab->address, child->rightSibling->intVal));
			} else { // simple variable and int
				if(root->parent->parent->type == STARTNODE)
					addSymbol(symtab, makeSymbol(S_GLOBAL, id, T_INT, symtab->address, 0));
				else
					addSymbol(symtab, makeSymbol(S_LOCAL, id, T_INT, symtab->address, 0));
			}
		}
	}
}

void semanticVar(astNode* root, symTab* symtab, char* id) {
	astNode* child = root->leftMostChild;
	symbol* declaredVar = seekExecutionLocal(id, symtab);

	if(declaredVar != NULL) { // Declared locally
		if(declaredVar->type == T_ARRAY) { // Need to check for an index
			if(child->rightSibling != NULL && child->rightSibling->type != NULLNODE) {}	// ok
			else {
				semanticError("Use of ARRAY type variable without indexing at", id);
			}
		} else if(declaredVar->type == T_INT) { // Need to check there is no index
			if(child->rightSibling != NULL && child->rightSibling->type != NULLNODE) {	// index found: not ok
				semanticError("Use of INT type variable with use of indexing at", id);
			}
		}
	} else {
		declaredVar = seekExecutionGlobal(id, symtab);
		if(declaredVar != NULL) { // Declared globally
			if(declaredVar->type == T_ARRAY) { // Need to check for an index
				if(child->rightSibling != NULL && child->rightSibling->type != NULLNODE) {}	// ok
				else {
					semanticError("Use of ARRAY type variable without indexing at", id);
				}
			} else if(declaredVar->type == T_INT) { // Need to check there is no index
				if(child->rightSibling != NULL && child->rightSibling->type != NULLNODE) {	// index found: not ok
					semanticError("Use of INT type variable with use of indexing at", id);
				}
			}
		} else { // Undeclared
			semanticError("Undeclared symbol", id);
		}
	}
}


void semanticDecFunc(astNode* root, symTab* symtab, char* id) {
	astNode* child = root->leftMostChild;

	if(seekDeclaration(id, symtab) == 1) {
		semanticError("Function already declared", id);
	} else { // Declaring function
		astNode* params = child->rightSibling;
		params = params->leftMostChild;
		short argsNb = 0;
		if(params != NULL && params->leftMostChild != NULL) {
			params = params->leftMostChild;
			argsNb++;
			while(params->rightSibling != NULL && params->rightSibling->type != NULLNODE) {
				argsNb++;
				params = params->rightSibling;
			}
		}
		addSymbol(symtab, makeSymbol(S_GLOBAL, id, T_FUNC, -1, 0));
		enterFunction(symtab);
		symbol* sym = seekExecutionGlobal(id, symtab);
		sym->argNb = argsNb;
		symtab->address = -argsNb;
		if(strcmp(id, "main") == 0 && argsNb != 0) // Verifying main has no arguments
			semanticError("Function requires no arguments at", "main");
	}
}


void semanticFuncCall(astNode* root, symTab* symtab, char* id) {
	astNode* child = root->leftMostChild;

	symbol* function = seekExecutionGlobal(id, symtab);
	if(function == NULL) {
		semanticError("Undeclared function", id);
	} else { // Verify number of arguments for call: has to be equal to function->argNb
		short argsNb = 0;
		astNode* sibling = child;
		while(sibling->rightSibling != NULL) {
			if(sibling->rightSibling->type != NULLNODE)
				argsNb++;
			sibling = sibling->rightSibling;
		}
		if(argsNb != function->argNb)
			semanticError("Invalid number of arguments when call of", id);
	}
}


int verifyMain(symTab* symtab) {
	short mainFound = seekDeclaration("main", symtab);
	if(mainFound == 0) {
		semanticError("No \'main\' function found in file", fileName);
		return 0;
	}
	return 1;
}

void drawSymTab(symTab* symtab) {
	symbol* curr = symtab->start;
	while(curr != NULL) {
		if(curr == symtab->start)
			printf("\tStart: ");
		if(curr == symtab->current)
			printf("\tCurrent: ");
		if(curr == symtab->top)
			printf("\tTop: ");
		printf("%s(%d)(%d)(%d), addr %d\t", curr->id, curr->type, curr->scope, curr->argNb, curr->address);
		curr = curr->next;
	}
	printf("\n");
}

symbol* makeSymbol(short scope, char* id, short type, int address, int argNb) {

	symbol* newSym = malloc(sizeof(symbol));

	newSym->next = NULL;
	newSym->scope = scope;
	newSym->id = id;
	newSym->type = type;
	newSym->address = address;
	newSym->argNb = argNb;

	return newSym;
}

symTab* makeSymTab() {
	symTab* symtab = malloc(sizeof(symTab));
	symtab->start = NULL;
	symtab->current = NULL;
	symtab->top = NULL;
	symtab->address = 0;
	symtab->sp = 0;
	return symtab;
}

/* 
 * Seeks a symbol in the table given an ID, starting at "current", meaning the symbol needs to be found in the current scope 
 * (can't declare twice the same id in the same scope)
 * Returns 0 if not found, 1 if found
 */
short seekDeclaration(char* id, symTab* symtab) {
	symbol* node = symtab->current;
	while(node != NULL) {
		if(strcmp(node->id, id) == 0)
			return 1;
		node = node->next;
	}
	return 0;
}

/* 
 * Seeks a symbol in the table given an ID, starting at "current", meaning the symbol can be found anywhere in the global of local scope
 * (it just needs to exist and be reachable to retrieve its value)
 * Returns a pointer to the symbol if found, NULL if not
 */
symbol* seekExecutionLocal(char* id, symTab* symtab) {
	symbol* node = symtab->current->next;
	while(node != NULL) {
		if(strcmp(node->id, id) == 0)
			return node;
		node = node->next;
	}
	return NULL;
}

/* 
 * Seeks a symbol in the table given an ID, starting at "start", meaning the symbol can be found anywhere in the global of local scope
 * (it just needs to exist and be reachable to retrieve its value)
 * Returns a pointer to the symbol if found, NULL if not
 */
symbol* seekExecutionGlobal(char* id, symTab* symtab) {
	symbol* node = symtab->start;
	symbol* end;
	if(node == symtab->current) /* Current context = global only */
		end = symtab->top->next;
	else
		end = symtab->current->next; /* Current context = global and local */
	while(node != end) {
		if(strcmp(node->id, id) == 0)
			return node;
		node = node->next;
	}
	return NULL;
}


symbol* addSymbol(symTab* symtab, symbol* sym) {

	if(sym->scope == S_UNDEF) {
		if(symtab->start == symtab->current && symtab->start != symtab->top)	// Current scope is global
			sym->scope = S_GLOBAL;
		else								// Current scope is local
			sym->scope = S_LOCAL;
	}

	if(sym->scope == S_GLOBAL) {
		sym->address = -1;
	}

	if(sym->scope != S_GLOBAL) {
		if(sym->type == T_INT || sym->type == T_FLOAT)
			symtab->address++;
		if(sym->type == T_ARRAY)
			symtab->address += sym->argNb;
	}


	if(symtab->start == NULL) { // Initialization symtab
		symtab->start = sym;
		symtab->current = sym;
		symtab->top = sym;
	} else {
		symtab->top->next = sym;
		symtab->top = symtab->top->next;
	}
	return sym;
}


void enterFunction(symTab* symtab) {
	symtab->current = symtab->top;
}

void leaveFunction(symTab* symtab) {

	freeSymTable(symtab->current->next, symtab);
	symtab->current->next = NULL;

	symtab->top = symtab->current;

	symtab->current = symtab->start;
	
}

void freeSymTable(symbol* node, symTab* symtab) {
	if(node == NULL)
		return;
	if(node->scope != S_GLOBAL && node->type == T_ARRAY)
		symtab->address -= node->argNb;
	else if(node->type == T_INT || node->type == T_FLOAT)
		symtab->address--;

	symbol* next = node->next;
	freeSymTable(next, symtab);	
	free(node);
}