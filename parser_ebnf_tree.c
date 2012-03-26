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
	if (errors == 0){
		printf("Compilation successful.\n\n");
		return 0;
	}	
	else{
		printf("Compilation unsuccessful, %i errors.\n\n", errors);
		return 1;
	}	
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
	
	//TODO: da sistemare
	while(lookahead == AND || lookahead == OR){
		next();

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

	//TODO: da sistemare
	if (lookahead == EQ || lookahead == NEQ || lookahead == GT || lookahead == GE || lookahead == LT || lookahead == LE){
		switch (lookahead){
			case EQ:
			case NEQ:
			case GT:
			case GE:
			case LT:
			case LE:	
				;	
		}

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
	
	//TODO: da sistemare
	while (lookahead == '+' || lookahead == '-') {
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
	
	//TODO: da sistemare
	while(lookahead == '*' || lookahead == '/' || lookahead == JOIN){
		if (lookahead == '*' || lookahead == '/'){
			next();
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

void parse_unary_op(){
	switch (lookahead){
		case '-':
			next();
			break;
		case NOT:
			next();
			break;
		case PROJECT:
			parse_project_op();
			break;
		case SELECT:
			parse_select_op();
			break;
		case EXISTS:
			parse_exists_op();
			break;
		case ALL:
			parse_all_op();
			break;
		case UPDATE:
			parse_update_op();
			break;
		case EXTEND:
			parse_extend_op();
			break;
		case RENAME:
			parse_rename_op();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}
}

void parse_join_op(){
	match2(JOIN, __func__);
	match2('[', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_project_op(){
	match2(PROJECT, __func__);
	match2('[', __func__);
	parse_id_list();
	match2(']', __func__);
}

void parse_select_op(){
	match2(SELECT, __func__);
	match2('[', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_exists_op(){
	match2(EXISTS, __func__);
	match2('[', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_all_op(){
	match2(ALL, __func__);
	match2('[', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_extend_op(){
	match2(EXTEND, __func__);
	match2('[', __func__);
	parse_atomic_type();
	match2(IDNAME, __func__);
	match2('=', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_update_op(){
	match2(UPDATE, __func__);
	match2('[', __func__);
	match2(IDNAME, __func__);
	match2('=', __func__);
	parse_expr();
	match2(']', __func__);
}

void parse_rename_op(){
	match2(JOIN, __func__);
	match2('[', __func__);
	parse_id_list();
	match2(']', __func__);
}

void parse_constant(){
	if (lookahead == INTCONST || lookahead == STRCONST || lookahead == BOOLCONST){
		parse_atomic_const();
	}
	else if(lookahead == '{'){
		parse_table_const();
	}
	else 
		print_error();
}

void parse_atomic_const(){
	if (lookahead == INTCONST || lookahead == STRCONST || lookahead == BOOLCONST){
		next();
	}
	else
		print_error();
}

void parse_table_const(){
	match2('{', __func__);
	if (lookahead == '('){
		parse_tuple_const();
		while (lookahead == ','){
			next();
			parse_tuple_const();
		}
	}
	match2('}', __func__);
}

void parse_tuple_const(){
	match2('(', __func__);
	parse_atomic_const();
	while (lookahead == ','){
		next();
		parse_atomic_const();
	}	
	match2(')', __func__);
}

void parse_if_stat(){
	match2(IF, __func__);
	parse_expr();
	match2(THEN, __func__);
	parse_stat_list();
	if (lookahead == ELSE){
		next();
		parse_stat_list();
	}
	match2(END, __func__);
}

void parse_while_stat(){
	match2(WHILE, __func__);
	parse_expr();
	match2(DO, __func__);
	parse_stat_list();
	match2(END, __func__);
}

void parse_read_stat(){
	match2(READ, __func__);
	parse_specifier();
	match2(IDNAME, __func__);
}

void parse_specifier(){
	if (lookahead == '['){
		next();
		parse_expr();
		match2(']', __func__);
	}
}

void parse_write_stat(){
	match2(WRITE, __func__);
	parse_specifier();
	parse_expr();
}
