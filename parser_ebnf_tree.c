#include "parser_ebnf_tree.h"

extern Lexval lexval;
extern int line;

Pnode root = NULL;

int lookahead;
int errors;

// Node functions *************************************************************

Pnode newnode(Typenode tnode){
	Pnode p;
	p = (Pnode)malloc(sizeof(Node));
	p->type = tnode;
	p->child = p->brother = NULL;
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

// ****************************************************************************

int main() {
	printf("\nCompiling, please wait..\n");
	line = 1;
	errors = 0;
	parse();
	
	printTree(root, 0);

	if (errors == 0){
		printf("Compilation successful.\n\n");
		return 0;
	}	
	else{
		printf("Compilation unsuccessful, %i errors.\n\n", errors);
		return 1;
	}	
}

void printTree(Pnode rootnode, int indent){
	
	int i;

	for (i = 0; i < indent; i++){
		printf("--");	
	}	
	printf(" ");
	printNode(rootnode);	
	printf("\n");

	if (rootnode->child != NULL){
		printTree(rootnode->child, indent+1);	
	}

	if (rootnode->brother != NULL){
		printTree(rootnode->brother, indent);
	}	
}

void printNode(Pnode node){
	if (node->type == T_NONTERMINAL)
		printf("%s", nonterminal_names[node->value.ival]);
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

void next(){
	int result = yylex();
	if (result == ERROR){
		printf("Errore - sintattico alla linea %d - %s\n", line, yytext);
		errors++;
	}
	else {
		lookahead = result;
	}
}

void match2(int symbol, const char* func){
	if (lookahead == symbol){
		next();
	}
	else {
		printf("Errore - linea %d, funzione %s: mi aspettavo %d, invece ho %d (yytext: %s)\n", line, func, symbol, lookahead, yytext);
		errors++;
	}
}

void print_error(){
	printf("Errore - linea %d (yytext: %s)\n", line, yytext);
	errors++;
}

// Parsing functions **********************************************************

void parse(){
	next();
	root = nontermnode(NPROGRAM);
	root->child = parse_program();
}

Pnode parse_program(){
	match2(PROGRAM, __func__);

	Pnode p;

	p = nontermnode(NSTAT_LIST);
	p->child = parse_stat_list();

	match2(END, __func__);
	
	return(p);
}

Pnode parse_stat_list(){
	
	Pnode head, p;

	head = p = nontermnode(NSTAT);
	p->child = parse_stat();

	while (lookahead == ';') {
		match2(';', __func__);

		p->brother = nontermnode(NSTAT);
		p = p->brother;
		p->child = parse_stat();
	}

	return(head);
}

Pnode parse_stat(){
	
	Pnode p;
	p = nontermnode(NDEF_STAT);

	switch (lookahead){
		case INTEGER:
		case STRING:
		case BOOLEAN:
		case TABLE:
			p->child = parse_def_stat();
			break;
		case IDNAME:
			p->child = parse_assign_stat();
			break;
		case IF:
			p->child = parse_if_stat();
			break;
		case WHILE:
			p->child = parse_while_stat();
			break;
		case READ:
			p->child = parse_read_stat();
			break;
		case WRITE:
			p->child = parse_write_stat();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}

	return(p);
}

Pnode parse_def_stat(){

	Pnode head, p;
	head = p = nontermnode(NTYPE);

	p->child = parse_type();
	
	p->brother = nontermnode(NID_LIST);
	p = p->brother;	
	p->child = parse_id_list();

	return(head);
}

Pnode parse_id_list() {
	
	Pnode p, head;

	if (lookahead == IDNAME){
		head = p = idnode();
		next();

		while(lookahead == ',') {

			match2(',', __func__);

			if (lookahead == IDNAME){
				p->brother = idnode();
				p = p->brother;		
				next();		
			}
			else{
				fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
				errors++;
				next();				
			}
		}
	}
	else {
		fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
		errors++;
		next();
	}

	return(head);
}

Pnode parse_type() {

	Pnode p;

	switch (lookahead){
		case INTEGER:
		case STRING:
		case BOOLEAN:
			p = nontermnode(NATOMIC_TYPE);
			p->child = parse_atomic_type();
			break;
		case TABLE:
			p = nontermnode(NTABLE_TYPE);
			p->child = parse_table_type();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}
	
	return(p);
	
}

Pnode parse_atomic_type() {

	Pnode p;

	switch (lookahead){
		case INTEGER:
			p = keynode(T_INTEGER);
			next();			
			break;
		case STRING:
			p = keynode(T_STRING);
			next();
			break;
		case BOOLEAN:
			p = keynode(T_BOOLEAN);
			next();
			break;
		default:
			print_error();
			next();
	}

	return(p);
}

Pnode parse_table_type() {

	Pnode p;
	p = nontermnode(NATTR_LIST);

	match2(TABLE, __func__);
	match2('(', __func__);
	p->child = parse_attr_list();
	match2(')', __func__);

	return(p);
}

Pnode parse_attr_list() {

	Pnode p, head;
	
	p = head = nontermnode(NATTR_DECL);
	p->child = parse_attr_decl();

	while (lookahead == ','){
		match2(',', __func__);
		
		p->brother = nontermnode(NATTR_DECL);
		p = p->brother;
		p->child = parse_attr_decl();
	}
	
	return(p);

}

Pnode parse_attr_decl() {

	Pnode head, p;

	head = p = idnode();
	next();

	match2(':', __func__);

	p->brother = nontermnode(NATOMIC_TYPE);	
	p = p->brother;	
	p->child = parse_atomic_type();

	return(head);
}

Pnode parse_assign_stat() {

	Pnode head, p;	

	head = p = idnode();
	next();

	match2('=', __func__);

	p->brother = nontermnode(NEXPR);
	p = p->brother;	
	p->child = parse_expr();

	return(head);
}

Pnode parse_expr() {

	Pnode p, head;
	
	p = head = nontermnode(NBOOL_TERM);
	p->child = parse_bool_term();
	
	while(lookahead == AND || lookahead == OR){
				
		if (lookahead == AND){
			p->brother = keynode(T_AND);						
		}
		else{
			p->brother = keynode(T_OR);	
		}
					
		next();
		p = p->brother;

		p->brother = nontermnode(NBOOL_TERM);
		p = p->brother;
		p->child = parse_bool_term();
	}

	return(head);
}

Pnode parse_bool_term(){
	
	Pnode head, p;
	head = p = nontermnode(NCOMP_TERM);
	p->child = parse_comp_term();

	if (lookahead == EQ || lookahead == NEQ || lookahead == GT || lookahead == GE || lookahead == LT || lookahead == LE){
		switch (lookahead){
			case EQ:
				p->brother = keynode(T_EQ);
				break;
			case NEQ:
				p->brother = keynode(T_NEQ);
				break;
			case GT:
				p->brother = keynode(T_GT);
				break;
			case GE:
				p->brother = keynode(T_GE);
				break;
			case LT:
				p->brother = keynode(T_LT);
				break;
			case LE:	
				p->brother = keynode(T_LE);
				break;	
		}
	
		p = p->brother;
		next();

		p->brother = nontermnode(NCOMP_TERM);
		p = p->brother;			
		p->child = parse_comp_term();

	}

	return(head);
}

Pnode parse_comp_term(){

	Pnode p, head;
	p = head = nontermnode(NLOW_TERM);
	p->child = parse_low_term();
	
	while (lookahead == '+' || lookahead == '-') {
		
		if (lookahead == '+')
			p->brother = keynode(T_PLUS);
		else
			p->brother = keynode(T_MINUS);
		
		p = p->brother;	
		next();

		p->brother = nontermnode(NLOW_TERM);
		p = p->brother;		
		p->child = parse_low_term();
	}

	return(head);
}

Pnode parse_low_term(){

	Pnode p, head;
	p = head = nontermnode(NFACTOR);
	p->child = parse_factor();
	
	while(lookahead == '*' || lookahead == '/' || lookahead == JOIN){
		if (lookahead == '*'){
			p->brother = keynode(T_MULT);			
			next();
			p = p->brother;
		}
		else if (lookahead == '/'){
			p->brother = keynode(T_DIVIDE);
			next();
			p = p->brother;
		}
		else{
			p->brother = nontermnode(NJOIN_OP);
			p = p->brother;
			p->child = parse_join_op();
		}
		
		p->brother = nontermnode(NFACTOR);
		p = p->brother;		
		p->child = parse_factor();
	}
	
	return(head);
}

Pnode parse_factor(){

	Pnode p, head;

	if (lookahead == '('){
		next();
		p = head = nontermnode(NEXPR);
		p->child = parse_expr();
		match2(')', __func__);
	}
	else if (lookahead == IDNAME){
		p = head = idnode();
		next();
	}
	else if (lookahead == INTCONST || lookahead == STRCONST || lookahead == BOOLCONST || lookahead == '{'){
		p = head = nontermnode(NCONSTANT);
		p->child = parse_constant();
	}
	else {
		p = head = nontermnode(NUNARY_OP);
		p->child = parse_unary_op();
		
		p->brother = nontermnode(NFACTOR);		
		p = p->brother;		
		p->child = parse_factor();
	}

	return(head);
}

Pnode parse_unary_op(){

	Pnode p;

	switch (lookahead){
		case '-':
			p = keynode(T_MINUS);
			break;
		case NOT:
			p = keynode(T_NOT);
			break;
		case PROJECT:
			p = nontermnode(NPROJECT_OP);
			p->child = parse_project_op();
			break;
		case SELECT:
			p = nontermnode(NSELECT_OP);
			p->child = parse_select_op();
			break;
		case EXISTS:
			p = nontermnode(NEXISTS_OP);
			p->child = parse_exists_op();
			break;
		case ALL:
			p = nontermnode(NALL_OP);
			p->child = parse_all_op();
			break;
		case UPDATE:
			p = nontermnode(NUPDATE_OP);
			p->child = parse_update_op();
			break;
		case EXTEND:
			p = nontermnode(NEXTEND_OP);
			p->child = parse_extend_op();
			break;
		case RENAME:
			p = nontermnode(NRENAME_OP);
			p->child = parse_rename_op();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}

	return(p);
}

Pnode parse_join_op(){

	Pnode p;

	match2(JOIN, __func__);
	match2('[', __func__);
	
	p = nontermnode(NEXPR);
	p->child = parse_expr();

	match2(']', __func__);

	return(p);
}

Pnode parse_project_op(){

	Pnode p;

	match2(PROJECT, __func__);
	match2('[', __func__);

	p = nontermnode(NID_LIST);
	p->child = parse_id_list();

	match2(']', __func__);

	return(p);
}

Pnode parse_select_op(){

	Pnode p;

	match2(SELECT, __func__);
	match2('[', __func__);

	p = nontermnode(NEXPR);
	p->child = parse_expr();

	match2(']', __func__);

	return(p);
}

Pnode parse_exists_op(){

	Pnode p;

	match2(EXISTS, __func__);
	match2('[', __func__);

	p = nontermnode(NEXPR);
	p->child = parse_expr();

	match2(']', __func__);

	return(p);
}

Pnode parse_all_op(){

	Pnode p;

	match2(ALL, __func__);
	match2('[', __func__);
	
	p = nontermnode(NEXPR);
	p->child = parse_expr();

	match2(']', __func__);

	return(p);
}

Pnode parse_extend_op(){

	Pnode p, head;

	match2(EXTEND, __func__);
	match2('[', __func__);

	head = p = idnode();
	next();

	match2('=', __func__);

	p->brother = nontermnode(NEXPR);
	p = p->brother;
	p->child = parse_expr();

	match2(']', __func__);

	return(head);
}

Pnode parse_update_op(){

	Pnode head, p;

	match2(UPDATE, __func__);
	match2('[', __func__);

	head = p = idnode();
	next();

	match2('=', __func__);

	p->brother = nontermnode(NEXPR);
	p = p->brother;	
	p->child = parse_expr();

	match2(']', __func__);

	return(head);
}

Pnode parse_rename_op(){
	
	Pnode p;

	match2(JOIN, __func__);
	match2('[', __func__);
	
	p = nontermnode(NID_LIST);
	p->child = parse_id_list();

	match2(']', __func__);

	return(p);
}

Pnode parse_constant(){
	
	Pnode p;

	if (lookahead == INTCONST || lookahead == STRCONST || lookahead == BOOLCONST){
		p = nontermnode(NATOMIC_CONST);		
		p->child = parse_atomic_const();
	}
	else if(lookahead == '{'){
		p = nontermnode(NTABLE_CONST);
		p->child = parse_table_const();
	}
	else 
		print_error();

	return(p);
}

Pnode parse_atomic_const(){

	Pnode p;
	
	if (lookahead == INTCONST){
		p = intconstnode();
		next();
	}
	else if (lookahead == BOOLCONST){
		p = boolconstnode();
		next();
	}
	else if (lookahead == STRCONST){
		p = strconstnode();
		next();
	}
	else
		print_error();

	return(p);
}

Pnode parse_table_const(){
	
	Pnode p, head;

	match2('{', __func__);
	if (lookahead == '('){		
		
		p = head = nontermnode(NTUPLE_CONST);
		p->child = parse_tuple_const();

		while (lookahead == ','){
			next();			
			p->brother = nontermnode(NTUPLE_CONST);
			p = p->brother;
			p->child = parse_tuple_const();
		}
	}
	match2('}', __func__);

	return(head);
}

Pnode parse_tuple_const(){

	Pnode p, head;

	match2('(', __func__);

	p = head = nontermnode(NATOMIC_CONST);
	p->child = parse_atomic_const();

	while (lookahead == ','){
		next();
		p->brother = nontermnode(NATOMIC_CONST);
		p = p->brother;
		p->child = parse_atomic_const();
	}	
	match2(')', __func__);

	return(head);
}

Pnode parse_if_stat(){

	Pnode p, head;

	match2(IF, __func__);
	
	p = head = nontermnode(NEXPR);
	p->child = parse_expr();

	match2(THEN, __func__);

	p->brother = nontermnode(NSTAT_LIST);
	p = p->brother;
	p->child = parse_stat_list();

	if (lookahead == ELSE){
		next();
		
		p->brother = nontermnode(NSTAT_LIST);
		p = p->brother;		
		p->child = parse_stat_list();
	}

	match2(END, __func__);

	return(head);
}

Pnode parse_while_stat(){

	Pnode p, head;

	match2(WHILE, __func__);

	head = p = nontermnode(NEXPR);
	p->child = parse_expr();
	
	match2(DO, __func__);

	p->brother = nontermnode(NSTAT_LIST);
	p = p->brother;
	p->child = parse_stat_list();

	match2(END, __func__);

	return(head);
}

Pnode parse_read_stat(){
	
	Pnode p, head;

	match2(READ, __func__);

	p = nontermnode(NSPECIFIER);
	p->child = parse_specifier();

	p->brother = idnode();
	next();

	return(head);
}

Pnode parse_specifier(){
	
	Pnode p;

	if (lookahead == '['){
		next();

		p = nontermnode(NEXPR);
		p->child = parse_expr();

		match2(']', __func__);
	}

	return(p);
}

Pnode parse_write_stat(){
	
	Pnode p, head;

	match2(WRITE, __func__);

	p = head = nontermnode(NSPECIFIER);
	p->child = parse_specifier();
	
	p->brother = nontermnode(NEXPR);
	p = p->brother;
	p->child = parse_expr();

	return(head);
}








