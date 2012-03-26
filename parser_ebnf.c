#include <stdio.h>
#include <stdlib.h>
#include "p2symbols.h"
#include "p2lexer.h"
#include "parser_ebnf.h"

int lookahead;
int errors;

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
	parse_program();
}

void parse_program(){
	match2(PROGRAM, __func__);
	parse_stat_list();
	match2(END, __func__);
}

void parse_stat_list(){
	parse_stat();
	while (lookahead == ';') {
		match2(';', __func__);
		parse_stat();
	}
}

void parse_stat(){
	switch (lookahead){
		case INTEGER:
		case STRING:
		case BOOLEAN:
		case TABLE:
			parse_def_stat();
			break;
		case IDNAME:
			parse_assign_stat();
			break;
		case IF:
			parse_if_stat();
			break;
		case WHILE:
			parse_while_stat();
			break;
		case READ:
			parse_read_stat();
			break;
		case WRITE:
			parse_write_stat();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}
}

void parse_def_stat(){
	parse_type();
	parse_id_list();
}

void parse_id_list() {
	match2(IDNAME, __func__);
	while(lookahead == ',') {
		match2(',', __func__);
		match2(IDNAME, __func__);
	}
}

void parse_type() {
	switch (lookahead){
		case INTEGER:
		case STRING:
		case BOOLEAN:
			parse_atomic_type();
			break;
		case TABLE:
			parse_table_type();
			break;
		default:
			fprintf(stderr, "Errore - linea %d, funzione %s\n", line, __func__);
			errors++;
			next();
	}	
}

void parse_atomic_type() {
	switch (lookahead){
		case INTEGER:
		case STRING:
		case BOOLEAN:
			next();
			break;
		default:
			print_error();
			next();
	}
}

void parse_table_type() {
	match2(TABLE, __func__);
	match2('(', __func__);
	parse_attr_list();
	match2(')', __func__);
}

void parse_attr_list() {
	parse_attr_decl();
	while (lookahead == ','){
		match2(',', __func__);
		parse_attr_decl();
	}
}

void parse_attr_decl() {
	match2(IDNAME, __func__);
	match2(':', __func__);
	parse_atomic_type();
}

void parse_assign_stat() {
	match2(IDNAME, __func__);
	match2('=', __func__);
	parse_expr();
}

void parse_expr() {
	parse_bool_term();
	while(lookahead == AND || lookahead == OR){
		next();
		parse_bool_term();
	}
}

void parse_bool_term(){
	parse_comp_term();
	switch (lookahead){
		case EQ:
		case NEQ:
		case GT:
		case GE:
		case LT:
		case LE:
			next();
			parse_comp_term();
			break;		
	}
}

void parse_comp_term(){
	parse_low_term();
	while (lookahead == '+' || lookahead == '-') {
		next();
		parse_low_term();
	}
}

void parse_low_term(){
	parse_factor();
	while(lookahead == '*' || lookahead == '/' || lookahead == JOIN){
		if (lookahead == '*' || lookahead == '/'){
			next();
		}
		else{
			parse_join_op();
		}
		parse_factor();
	}
}

void parse_factor(){
	if (lookahead == '('){
		next();
		parse_expr();
		match2(')', __func__);
	}
	else if (lookahead == IDNAME){
		next();
	}
	else if (lookahead == INTCONST || lookahead == STRCONST || lookahead == BOOLCONST || lookahead == '{'){
		parse_constant();
	}
	else {
		parse_unary_op();
		parse_factor();
	}
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
