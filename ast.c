#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int nodeNumber = 0;
char label[MAX_ID_SIZE];
/*
 * Creates and return a node according to its type. Only the parameters related to the type are required. Dummy values 
 * are to be entered in the undefined fields (won't be used)
 */
astNode* makeNode(int nodeType, int value, char operator, char* id) {
	astNode* node = malloc(sizeof(astNode));
	
	if(node == NULL) {
		perror("Memory allocation error");
		exit(EXIT_FAILURE);
	}
	node->parent = NULL;
	node->leftMostSibling = node;
	node->rightSibling = NULL;
	node->leftMostChild = NULL;

	node->type = nodeType;

	switch(nodeType) {

		case NUMBER:
			node->intVal = value;
		break;

		case OPERATOR:
			node->operator = operator;
		break;

		case SYMBOL:
			strcpy(node->id, id);
			/* Will have to define the table entry somewhere when implemented */
		break;

		case ARRSIZE:
			node->intVal = value;
		break;

		case NULLNODE:
		case EXPR:
		case OPERATION:
		case READCALL:
		case WRITECALL:
		case RETURNSTMT:
		case STARTNODE:
		case LISTVARDEC:
		case VARDEC:
		case DECFUNC:
		case LISTDECFUNC:
		case LISTPARAM:
		case AFFECT:
		case LISTINSTR:
		case IFSTMT:
		case WHILESTMT:
		case VAR:
		case FUNCCALL:
		default:
		break;
	}
	return node;
}

/* Joins two lists of siblings, makes y become siblings of x and y's parent become x's parent
 * Returns rightmost sibling of new siblings 
 */
astNode* makeSiblings(astNode* x, astNode* y) {
	if(x == NULL || y == NULL)
		return NULL;
	astNode* xsib = x;

	/* Get rightmost sibling */
	while(xsib->rightSibling != NULL) {
		xsib = xsib->rightSibling;
	}

	/* Link as siblings x -> y*/
	astNode* ysib = y->leftMostSibling;
	xsib->rightSibling = ysib;

	/* All ys get x->leftmostsibling as leftmostsibling and x->parent as parent */
	ysib->leftMostSibling = xsib->leftMostSibling;
	ysib->parent = xsib->parent;

	while(ysib->rightSibling != NULL) {
		ysib = ysib->rightSibling;
		ysib->leftMostSibling = xsib->leftMostSibling;
		ysib->parent = xsib->parent;
	}

	return ysib;
}

/* Joins two lists of siblings, makes y become the leftmost sibling of x and y's parent become x's parent
 * Returns leftmost sibling of new siblings 
 */
astNode* makeLeftSibling(astNode* x, astNode* y) {
	if(x == NULL || y == NULL)
		return NULL;
	astNode* xsib = x->leftMostSibling;
	astNode* ysib = y;

	/* Get y's rightmost sibling */
	while(ysib->rightSibling != NULL) {
		ysib = ysib->rightSibling;
	}

	/* Link as siblings y -> x*/
	ysib->rightSibling = xsib;

	/* All xs get y->leftmostsibling as leftmostsibling */
	xsib->leftMostSibling = ysib->leftMostSibling;

	while(xsib->rightSibling != NULL) {
		xsib = xsib->rightSibling;
		xsib->leftMostSibling = ysib->leftMostSibling;
	}

	/* All ys get x->parent as parent */
	ysib = y;
	ysib->parent = x->parent;

	while(ysib->rightSibling != NULL) {
		ysib = ysib->rightSibling;
		ysib->parent = x->parent;
	}

	return ysib->leftMostSibling;
}

/*
 * Makes x adopt y and its siblings as children and returns x
 */
astNode* adoptChildren(astNode* x, astNode* y) {
	if(x == NULL || y == NULL)
		return NULL;

	if(x->leftMostChild != NULL) {
		makeSiblings(x->leftMostChild, y);
	}
	else {

		if(y->parent != NULL)
			y->parent->leftMostChild = NULL;

		astNode* ysib = y->leftMostSibling;
		x->leftMostChild = ysib;
		while(ysib != NULL) {
			ysib->parent = x;
			ysib = ysib->rightSibling;
		}
	}
	return x;
}


/*
 * Makes x adopt y and its siblings as children and returns x
 */
astNode* adoptLeftChildren(astNode* x, astNode* y) {
	if(x == NULL || y == NULL)
		return NULL;

	if(x->leftMostChild != NULL) {
		makeLeftSibling(x->leftMostChild, y);
	}
	else {

		if(y->parent != NULL)
			y->parent->leftMostChild = NULL;

		astNode* ysib = y->leftMostSibling;
		x->leftMostChild = ysib;
		while(ysib != NULL) {
			ysib->parent = x;
			ysib = ysib->rightSibling;
		}
	}
	return x;
}


void displayAstTree(astNode* root) {
	if(root == NULL)
		return;
	printf("* %d\n", root->type);
	astNode* node = root->leftMostChild;
	while(node != NULL) {
		drawAstTree(node);
		node = node->rightSibling;
	}
}


void updateLabel(astNode* node) {
	char value[MAX_ID_SIZE];
	switch(node->type) {
		case NULLNODE:
			strcpy(label, "NULL node");
		break;

		case NUMBER:
			strcpy(label, "Number: ");
			sprintf(value, "%d", node->intVal);
			strcat(label, value);
		break;

		case OPERATOR:
			strcpy(label, "Operator: ");
			sprintf(value, "%c", node->operator);
			strcat(label, value);
		break;

		case SYMBOL:
			strcpy(label, "Symbol: ");
			sprintf(value, "%s", node->id);
			strcat(label, value);
		break;

		case EXPR:
			strcpy(label, "Expression");
		break;

		case OPERATION:
			strcpy(label, "Operation");
		break;

		case READCALL:
			strcpy(label, "Read call");
		break;

		case WRITECALL:
			strcpy(label, "Write call");
		break;

		case RETURNSTMT:
			strcpy(label, "Return");
		break;

		case STARTNODE:
			strcpy(label, "Start node");
		break;

		case LISTVARDEC:
			strcpy(label, "List variables declarations");
		break;

		case VARDEC:
			strcpy(label, "Variable declaration");
		break;

		case ARRSIZE:
			strcpy(label, "Array size: ");
			sprintf(value, "%d", node->intVal);
			strcat(label, value);
		break;

		case DECFUNC:
			strcpy(label, "Function declaration");
		break;

		case LISTDECFUNC:
			strcpy(label, "List functions declarations");
		break;

		case LISTPARAM:
			strcpy(label, "List parameters");
		break;

		case AFFECT:
			strcpy(label, "Affectation");
		break;

		case LISTINSTR:
			strcpy(label, "List instructions");
		break;

		case IFSTMT:
			strcpy(label, "If statement");
		break;

		case WHILESTMT:
			strcpy(label, "While statement");
		break;

		case VAR:
			strcpy(label, "Variable");
		break;

		case FUNCCALL:
			strcpy(label, "Function call");
		break;

		default:
			strcpy(label, "UNKNOWN");
		break;
	}
	return;
}

/*
 * Frees allocated memory for the tree
 */
int freeAstTree(astNode* root) {
	if(root == NULL)
		return 0;

	astNode* child = root->leftMostChild;
	astNode* leftChild;
	while(child != NULL) {
		leftChild = child;
		child = child->rightSibling;
		freeAstTree(leftChild);
	}

	free(root);
	return 1;
}


void scourAndWriteASTTree(astNode* root, FILE* fp) {
	if(root == NULL)
		return;

	int currentNodeNumber = nodeNumber;

	astNode* node = root->leftMostChild;
	while(node != NULL) {
		updateLabel(node);
		fprintf(fp, "%d -> %d [label=\"%s\"];\n", currentNodeNumber, ++nodeNumber, label);
		scourAndWriteASTTree(node, fp);
		node = node->rightSibling;
	}
}


void refineAstTree(astNode* root) {
	if(root == NULL)
		return;

	astNode* node = root->leftMostChild;
	while(node != NULL) {
		
		if(numberOfChildren(root) == 1 && ((root->type == EXPR && root->leftMostChild->type == EXPR) 
				|| (root->type == OPERATION && root->leftMostChild->type == OPERATION) 
				|| (root->type == EXPR && root->leftMostChild->type == OPERATION))) {
			astNode* child = root->leftMostChild;
			astNode* grandchild = child->leftMostChild;
			astNode* leftSib;
			root->leftMostChild = NULL;
			grandchild->parent = NULL;
			while(child != NULL) {
				leftSib = child;
				child = child->rightSibling;
				free(leftSib);
			}
			adoptChildren(root, grandchild);
			node = grandchild;
		}
		refineAstTree(node);
		node = node->rightSibling;
	}

}

/* Return the number of not NULLNODE children */
int numberOfChildren(astNode* root) {
	if(root->leftMostChild == NULL)
		return 0;
	astNode* child = root->leftMostChild;
	int siblingsNb = 0;
	if(child->type != NULLNODE) 
		siblingsNb = 1;

	while(child->rightSibling != NULL) {
		child = child->rightSibling;
		if(child->type != NULLNODE)
			siblingsNb++;
	}
	return siblingsNb;
}


/*
 * Draw the AST Tree in dot language using graphviz
 * Returns 1 on success, 0 on failure
 * */
int drawAstTree(astNode* root){
	FILE *fp;

	if(root == NULL)
		return 0;

	fp = fopen(".tmp", "w");
	if(fp == NULL) {
		perror("Cannot open file. Abort");
		return 0;
	}

	fprintf(fp, "digraph fsm{\n");
	fprintf(fp, "graph [margin=\"0,0\"];\n");
	fprintf(fp, "node [shape = circle];\n");
	fprintf(fp, "rankdir=TB;\n");

	/* Do loops and scouring here */
	scourAndWriteASTTree(root, fp);
  
	fprintf(fp, "}\n");
	if(fp != stdout)
		fclose(fp);

	if(system("dot .tmp | xdot - &") != 0) {
		fprintf(stderr, "impossible de dessiner l'automate");
		return 0;
	}
	return 1;
}

