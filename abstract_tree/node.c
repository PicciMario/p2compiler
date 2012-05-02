#include "node.h"
#include "abs_tree.h"

extern Lexval lexval;

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

Pnode pseudotermnode(Typenode type, int value){
	Pnode p;
	p = newnode(type);
	p->value.ival = value;
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

int curPrintingLine = 0;

#define CHILDREN_RECURSION_LIMIT 100
int children[CHILDREN_RECURSION_LIMIT]; 

const char* node_names[] = {
	//"T_INTEGER",
	//"T_STRING",
	//"T_BOOLEAN",
	"T_ATOMIC_TYPE",
	
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
	//"ATOMIC_TYPE",
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

	printf("%s", printNode(rootnode));	
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

char* printNode(Pnode node){
	char *nodeDescr = malloc(100);
	if (node->type == T_NONTERMINAL){
		sprintf(nodeDescr, "%s", nonterminal_names[node->value.ival]);	
	}
	else
		if (node->type == T_ID)
			sprintf(nodeDescr, "ID (%s)", node->value.sval);
		else if (node->type == T_INTCONST)
			sprintf(nodeDescr, "INT (%d)", node->value.ival);
		else if (node->type == T_STRCONST)
			sprintf(nodeDescr, "STRING (%s)", node->value.sval);
		else if (node->type == T_BOOLCONST)
			sprintf(nodeDescr, "BOOL (%s)", node->value.ival == 1?"true":"false");		
		else if (node->type == T_EQ)
			sprintf(nodeDescr, "==");
		else if (node->type == T_NEQ)
			sprintf(nodeDescr, "!=");
		else if (node->type == T_LE)
			sprintf(nodeDescr, "<=");
		else if (node->type == T_LT)
			sprintf(nodeDescr, "<");
		else if (node->type == T_GT)
			sprintf(nodeDescr, ">");
		else if (node->type == T_GE)
			sprintf(nodeDescr, ">=");
		else if (node->type == T_AND)
			sprintf(nodeDescr, "AND");
		else if (node->type == T_OR)
			sprintf(nodeDescr, "OR");
		else if (node->type == T_NOT)
			sprintf(nodeDescr, "NOT");
		else if (node->type == T_PLUS)
			sprintf(nodeDescr, "+");
		else if (node->type == T_MINUS)
			sprintf(nodeDescr, "-");
		else if (node->type == T_MULT)
			sprintf(nodeDescr, "*");
		else if (node->type == T_DIVIDE)
			sprintf(nodeDescr, "/");
		else if (node->type == T_ATOMIC_TYPE){
			if (node->value.ival == INTEGER)
				sprintf(nodeDescr, "ATOMIC TYPE\\nINTEGER");
			else if (node->value.ival == STRING)
				sprintf(nodeDescr, "ATOMIC TYPE\\nSTRING");
			else if (node->value.ival == BOOLEAN)
				sprintf(nodeDescr, "ATOMIC TYPE\\nBOOLEAN");
			else 
				sprintf(nodeDescr, "ATOMIC TYPE\\n????");
		}

		else sprintf(nodeDescr, "%s", node_names[node->type]);
	
	return nodeDescr;

}

int nodeSeq = 0;

void printGraphvizTree(Pnode rootnode, int parent, int brother, FILE *file){

	if (parent == 0){
		file = fopen("graph.gv", "w");
		fprintf(file, "digraph \"\" {\n");
	}
	
	int thisNode = nodeSeq = nodeSeq + 1;
	
	if (rootnode->type == T_NONTERMINAL){
		fprintf(file, "node [shape=box]\n");
	}
	else{
		fprintf(file, "node [shape=ellipse]\n");
	}
	
	fprintf(file, "n%04d [label = \"%s\"]\n", thisNode, printNode(rootnode));
	
	if (brother == 1){
		fprintf(file, "{rank=same; n%04d; n%04d;}\n", thisNode, parent);
	}	
	
	if (parent != 0){
		fprintf(file, "n%04d -> n%04d\n", parent, thisNode);
	}
	
	if (rootnode->child != NULL){
		printGraphvizTree(rootnode->child, thisNode, 0, file);
	}

	if (rootnode->brother != NULL){
		printGraphvizTree(rootnode->brother, thisNode, 1, file);
	}	
	
	if (parent == 0){
		fprintf(file, "}");
		fclose(file);
	}
}
