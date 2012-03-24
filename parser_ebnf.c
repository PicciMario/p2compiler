#include <stdio.h>
#include <stdlib.h>
#include "p2symbols.h"
#include "p2lexer.h"
#include "parser_ebnf.h"

int lookahead;

int main() {
	line = 1;
	parse();
	return 0;
}

void next(){
	int result = yylex();
	if (result == ERROR){
		printf("Errore sintattico alla linea %d - %s", line, yytext);
	}
	else {
		lookahead = result;
	}
}

void match(int symbol){
	if (lookahead == symbol){
			next();
	}
	else {
		printf("Linea %d: Mi aspettavo %d che è diverso da %d\n", line, symbol, lookahead);
	}
}

void print_error(){
		printf("Errore alla linea %d\n", line);
}

void parse(){
	next();
	parse_program();
	
}

void parse_program(){
		match(PROGRAM);
		parse_stat_list();
		match(END);		
}

void parse_stat_list(){
	parse_stat();
	while (lookahead == ';') {
		match(';');
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
		case ID:
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
			print_error();
	}
}

void parse_def_stat(){
	parse_type();
	parse_id_list();
}

void parse_id_list() {
	match(ID);
	while(lookahead == ',') {
		match(',');
		match(ID);
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
			print_error();
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
	}
}

void parse_table_type() {
	match(TABLE);
	match('(');
	parse_attr_list();
	match(')');
}

void parse_attr_list() {
	parse_attr_decl();
	while (lookahead == ','){
		match(',');
		parse_attr_decl();
	}
}

void parse_attr_decl() {
	match(ID);
	match(':');
	parse_atomic_type();
}

void parse_assign_stat() {
	match(ID);
	match('=');
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
		parse_constant();
	}
	else if (lookahead == ID){
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
			print_error();
	}
}

void parse_join_op(){
	match(JOIN);
	match('[');
	parse_expr();
	match(']');
}

void parse_project_op(){
	match(PROJECT);
	match('[');
	parse_id_list();
	match(']');
}

void parse_select_op(){
	match(SELECT);
	match('[');
	parse_expr();
	match(']');
}

void parse_exists_op(){
	match(EXISTS);
	match('[');
	parse_expr();
	match(']');
}

void parse_all_op(){
	match(ALL);
	match('[');
	parse_expr();
	match(']');
}

void parse_extend_op(){
	match(EXTEND);
	match('[');
	parse_atomic_type();
	match(ID);
	match('=');
	parse_expr();
	match(']');
}

void parse_update_op(){
	match(UPDATE);
	match('[');
	match(ID);
	match('=');
	parse_expr();
	match(']');
}

void parse_rename_op(){
	match(JOIN);
	match('[');
	parse_id_list();
	match(']');
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
	match('{');
	if (lookahead == '('){
		parse_tuple_const();
		while (lookahead == ','){
			next();
			parse_tuple_const();
		}
	}
	match('}');
}

void parse_tuple_const(){
	match('(');
	parse_atomic_const();
	while (lookahead == ','){
		next();
		parse_atomic_const();
	}	
	match(')');
}

void parse_if_stat(){
	match(IF);
	parse_expr();
	match(THEN);
	parse_stat_list();
	if (lookahead == ELSE){
		next();
		parse_stat_list();
	}
	match(END);
}

void parse_while_stat(){
	match(WHILE);
	parse_expr();
	match(DO);
	parse_stat_list();
	match(END);
}

void parse_read_stat(){
	match(READ);
	parse_specifier();
	match(ID);
}

void parse_specifier(){
	if (lookahead == '['){
		parse_expr();
		match(']');
	}
}

void parse_write_stat(){
	match(READ);
	parse_specifier();
	parse_expr();
}
