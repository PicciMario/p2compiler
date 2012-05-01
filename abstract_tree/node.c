#include "node.h"

extern Lexval lexval;
int curPrintingLine = 0;

#define CHILDREN_RECURSION_LIMIT 100
int children[CHILDREN_RECURSION_LIMIT]; 

// Node functions *************************************************************

Pnode newnode(Typenode tnode){
	Pnode p;
	p = (Pnode)malloc(sizeof(Node));
	p->type = tnode;
	p->child = p->brother = NULL;
	p->line = line;
	return(p);
}

Pnode nontermnode(Nonterminal nonterm){
	Pnode p;
	p = newnode(T_NONTERMINAL);
	p->value.ival = nonterm;
	return(p);
}

Pnode keynode(Typenode keyword){
	return(newnode(keyword));
}

Pnode idnode(){
	Pnode p;
	p = newnode(T_ID);
	p->value.sval = lexval.sval;
	return(p);
}

Pnode intconstnode(){
	Pnode p;
	p = newnode(T_INTCONST);
	p->value.ival = lexval.ival;
	return(p);
}

Pnode strconstnode(){
	Pnode p;
	p = newnode(T_STRCONST);
	p->value.sval = lexval.sval;
	return(p);
}

Pnode boolconstnode(){
	Pnode p;
	p = newnode(T_BOOLCONST);
	p->value.ival = lexval.ival;
	return(p);
}

// Tree printing *************************************************************

const char* node_names[] = {
	"T_INTEGER",
	"T_STRING",
	"T_BOOLEAN",
	"T_INTCONST",
	"T_BOOLCONST",
	"T_STRCONST",
	"T_ID",
	"T_NONTERMINAL",
	"T_AND",
	"T_OR",
	"T_NOT",
	"T_EQ",
	"T_NEQ",
	"T_LE",
	"T_LT",
	"T_GE",
	"T_GT",
	"T_PLUS",
	"T_MINUS",
	"T_MULT",
	"T_DIVIDE"
};

const char* nonterminal_names[] = {
	"PROGRAM",
	"STAT_LIST",
	"STAT",
	"DEF_STAT",
	"ID_LIST",
	"TYPE",
	"ATOMIC_TYPE",
	"TABLE_TYPE",
	"ATTR_LIST",
	"ATTR_DECL",
	"ASSIGN_STAT",
	"EXPR",
	"BOOL_TERM",
	"COMP_TERM",
	"LOW_TERM",
	"FACTOR",
	"UNARY_OP",
	"JOIN_OP",
	"PROJECT_OP",
	"SELECT_OP",
	"EXISTS_OP",
	"ALL_OP",
	"EXTEND_OP",
	"UPDATE_OP",
	"RENAME_OP",
	"CONSTANT",
	"ATOMIC_CONST",
	"TABLE_CONST",
	"TUPLE_LIST",
	"ATOMIC_TYPE_LIST",
	"TUPLE_CONST",
	"IF_STAT",
	"WHILE_STAT",
	"READ_STAT",
	"SPECIFIER",
	"WRITE_STAT"
};


void printTree(Pnode rootnode, int indent){
	
	int i;

	/*
	if (curPrintingLine != rootnode->line){
		curPrintingLine = rootnode->line;
		printf("%04i ", curPrintingLine);
	}
	else{
		printf(" --  ");	
	}
	*/
	
	printf("%03d  ", rootnode->line);
	
	for (i = 0; i < indent; i++){
		if (children[i] == 1)
			printf("\u2502   ");
		else
			printf("    ");	
	}
	
	if (rootnode->brother == NULL)
		printf("\u2514\u2500> ");
	else
		printf("\u251c\u2500> ");

	printNode(rootnode);	
	printf("\n");

	if (rootnode->brother != NULL)
		children[indent] = 1;
	else
		children[indent] = 0;

	if (rootnode->child != NULL){
		printTree(rootnode->child, indent+1);	
	}

	if (rootnode->brother != NULL){
		printTree(rootnode->brother, indent);
	}	
	
	children[indent] = 0;
}

void printNode(Pnode node){
	if (node->type == T_NONTERMINAL){
		printf("%s", nonterminal_names[node->value.ival]);	
	}
	else
		if (node->type == T_ID)
			printf("ID (%s)", node->value.sval);
		else if (node->type == T_INTCONST)
			printf("INT (%d)", node->value.ival);
		else if (node->type == T_STRCONST)
			printf("STRING (%s)", node->value.sval);
		else if (node->type == T_BOOLCONST)
			printf("BOOL (%s)", node->value.ival == 1?"true":"false");		
		else if (node->type == T_EQ)
			printf("==");
		else if (node->type == T_NEQ)
			printf("!=");
		else if (node->type == T_LE)
			printf("<=");
		else if (node->type == T_LT)
			printf("<");
		else if (node->type == T_GT)
			printf(">");
		else if (node->type == T_GE)
			printf(">=");
		else if (node->type == T_AND)
			printf("AND");
		else if (node->type == T_OR)
			printf("OR");
		else if (node->type == T_NOT)
			printf("NOT");
		else if (node->type == T_PLUS)
			printf("+");
		else if (node->type == T_MINUS)
			printf("-");
		else if (node->type == T_MULT)
			printf("*");
		else if (node->type == T_DIVIDE)
			printf("/");

		else printf("%s", node_names[node->type]);

}
