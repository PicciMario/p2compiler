%{
	#include "node.h"
	#include "p2lexer.h"
	#include <stdio.h>
	
	#define YYSTYPE Pnode
	
	Pnode root = NULL;
	void yyerror(char* message);
%}

%token PROGRAM END INTEGER STRING BOOLEAN ID TABLE IF THEN ELSE
%token WHILE DO JOIN PROJECT SELECT EXISTS ALL EXTEND UPDATE RENAME
%token READ WRITE AND OR NOT GT GE LT LE EQ NEQ INTCONST STRCONST BOOLCONST
%token ERROR

%%

	program		:	PROGRAM stat_list END {
						$$ = root = nontermnode(NPROGRAM);
						$$->child = nontermnode(NSTAT_LIST);
						$$->child->child = $2;
					}
				;

	stat_list	:	stat ';' stat_list {
						$$ = $1;
						$$->brother = $3;
					}
				|	stat
				;
	
	stat		:	def_stat
				|	assign_stat
				|	if_stat
				|	while_stat
				|	read_stat
				|	write_stat
				;
	
	def_stat	:	type id_list {
						$$ = nontermnode(NDEF_STAT);
						$$->child = $1;
						$1->brother = $2;
					}
				;
			
	id_list		: 	ID {$$ = idnode();} ',' id_list {
						$$ = $2;
						$$->brother = $4;
					}
				|	ID {$$ = idnode();}
				;
				
	type		:	atomic_type
				|	table_type
				;
	
	atomic_type	:	INTEGER 	{ $$ = pseudotermnode(T_ATOMIC_TYPE, INTEGER); }
				|	STRING 		{ $$ = pseudotermnode(T_ATOMIC_TYPE, STRING); }
				|	BOOLEAN 	{ $$ = pseudotermnode(T_ATOMIC_TYPE, BOOLEAN); }
				;
				
	table_type	:	TABLE '(' attr_list ')' {
						$$ = nontermnode(NTABLE_TYPE);
						$$->child = $3;
					}
				;
				
	attr_list	:	attr_decl ',' attr_list {
						$$ = $1;
						$$->brother = $3; 
					}
				|	attr_decl
				;

	attr_decl	:	atomic_type ID {
						$$ = nontermnode(NATTR_DECL);
						$$->child = $1;
						$$->child->brother = idnode();
					}
				;
	
	assign_stat	:	ID {$$ = idnode();} '=' expr {
						$$ = nontermnode(NASSIGN_STAT);
						$$->child = $2;
						$$->child->brother = $4;
					}
				;
	
	expr		:	expr bool_op bool_term {
						$$ = $2;
						$$->child = $1;
						$$->child->brother = $3;
					}
				|	bool_term
				;
	
	bool_op		:	AND {$$ = pseudotermnode(T_LOGIC_EXPR, AND); }
				|	OR {$$ = pseudotermnode(T_LOGIC_EXPR, OR); }
				;
	
	bool_term	:	comp_term comp_op comp_term {
						$$ = $2;
						$$->child = $1;
						$$->child->brother = $3;
					}
				|	comp_term
				;
				
	comp_op		:	EQ 	{$$ = pseudotermnode(T_COMP_EXPR, EQ); }
				|	NEQ	{$$ = pseudotermnode(T_COMP_EXPR, NEQ); }
				|	GE 	{$$ = pseudotermnode(T_COMP_EXPR, GE); }
				|	GT 	{$$ = pseudotermnode(T_COMP_EXPR, GT); }
				|	LE 	{$$ = pseudotermnode(T_COMP_EXPR, LE); }
				|	LT 	{$$ = pseudotermnode(T_COMP_EXPR, LT); }
				;
	
	comp_term	:	comp_term low_bin_op low_term {
						$$ = $2;
						$$->child = $1;
						$$->child->brother = $3;
					}
				| 	low_term
				;
	
	low_bin_op	:	'+'	{$$ = pseudotermnode(T_MATH_EXPR, '+'); }
				|	'-'	{$$ = pseudotermnode(T_MATH_EXPR, '-'); }
				;
				
	low_term	:	low_term high_bin_op factor {
							$$ = $2;
							$$->child = $1;
							$$->child->brother = $3;
					}
				|	factor
				;
	
	high_bin_op	:	'*' {$$ = pseudotermnode(T_MATH_EXPR, '*'); }
				|	'/' {$$ = pseudotermnode(T_MATH_EXPR, '/'); }
				|	join_op
				;
				
	factor		:	unary_op factor {
						$$ = $1;
						$$->child = $2;
					}
				|	'(' expr ')' {$$ = $2;}
				|	ID {$$ = idnode();}
				|	constant
				;
				
	unary_op	:	'-' {$$ = pseudotermnode(T_NEG_EXPR, '-');}
				|	NOT {$$ = pseudotermnode(T_NEG_EXPR, NOT);}
				|	project_op
				|	select_op
				|	exists_op
				|	all_op
				|	update_op{$$ = keynode(T_ERROR);}
				|	extend_op{$$ = keynode(T_ERROR);}
				|	rename_op{$$ = keynode(T_ERROR);}
				;
				
	join_op		:	JOIN '[' expr ']' {
						$$ = nontermnode(NJOIN_OP);
						$$->brother = $3;
					}
				;

	project_op	:	PROJECT '[' id_list ']' {
						$$ = nontermnode(NPROJECT_OP);
						$$->brother = $3;
					}
				;
				
	select_op	:	SELECT '[' expr ']' {
						$$ = pseudotermnode(T_SELECT_EXPR, SELECT);
						$$->brother = $3;
					}
				;				
				
	exists_op	:	EXISTS '[' expr ']' {
						$$ = pseudotermnode(T_SELECT_EXPR, EXISTS);
						$$->brother = $3;
					}
				;				
				
	all_op		:	ALL '[' expr ']' {
						$$ = pseudotermnode(T_SELECT_EXPR, ALL);
						$$->brother = $3;
					}
				;		
				
	extend_op	:	EXTEND '[' atomic_type ID '=' expr ']' {
						$$ = nontermnode(NEXTEND_OP);
						$$->brother = $3;
						$$->brother->brother = $4;
						$$->brother->brother->brother = $6;
					}
				;
				
	update_op	:	UPDATE '[' ID '=' expr ']' {
						$$ = nontermnode(NUPDATE_OP);
						$$->brother = $3;
						$$->brother->brother = $5;
					}	
				;
	
	rename_op	:	RENAME '[' id_list ']' {
						$$ = nontermnode(NRENAME_OP);
						$$->brother = $3;
					}	
				;
	
	constant	:	atomic_const
				|	table_const
				;
				
	atomic_const	:	INTCONST {$$ = intconstnode();}
					|	STRCONST {$$ = strconstnode();}
					|	BOOLCONST {$$ = boolconstnode();}
					;
	
	table_const	:	'{' table_instance '}' {
						$$ = nontermnode(NTABLE_CONST);
						$$->child = $2;
					}
				;
				
	table_instance	:	tuple_list
					|	atomic_type_list
					;
	
	tuple_list	:	tuple_const ',' tuple_list {
						$$ = $1;
						$$->brother = $3;
					}
				|	tuple_const
				;
				
	tuple_const	:	'(' atomic_const_list ')' {
						$$ = nontermnode(NTUPLE_CONST);
						$$->child = $2;
					}
				;
				
	atomic_const_list	:	atomic_const ',' atomic_const_list {
								$$ = $1;
								$$->brother = $3;
							}
						|	atomic_const
						;
						
	atomic_type_list	:	atomic_type ',' atomic_type_list {
								$$ = $1;
								$$->brother = $3;
							}
						|	atomic_type
						;
						
	if_stat		:	IF expr THEN stat_list else_part END {
						$$ = nontermnode(NIF_STAT);
						$$->child = $2;
						$$->child->brother = nontermnode(NSTAT_LIST);
						$$->child->brother->child = $4;
						$$->child->brother->brother = $5;
					}
				;
				
	else_part	:	ELSE stat_list {
						$$ = nontermnode(NSTAT_LIST);
						$$->child = $2;
					}
				|	{$$ = NULL;}
				;
				
	while_stat	:	WHILE expr DO stat_list END {
						$$ = nontermnode(NWHILE_STAT);
						$$->child = $2;
						$$->child->brother = nontermnode(NSTAT_LIST);
						$$->child->brother->child = $4;
					}
				;
				
	read_stat	:	READ specifier ID {
						$$ = nontermnode(NREAD_STAT);
						$$->child = idnode();
						$$->child->brother = $2;
					}
				;
	
	write_stat	:	WRITE specifier	ID {
						$$ = nontermnode(NWRITE_STAT);
						$$->child = idnode();
						$$->child->brother = $2;
					}
				;
				
	specifier	:	'[' expr ']' {$$ = $2;}
				|	{$$ = NULL;}
				;
				
%%

void yyerror(char* message){
	fprintf(stderr, "%s - %s (line %d)\n", message, yytext, line);
}

int main(){
	int result = 0;
	line = 1;
	result = yyparse();
	if (result == 0){
		printTree(root, 0);
		printGraphvizTree(root, 0, 0, NULL);
		return 0;
	}
	return 1;
}
