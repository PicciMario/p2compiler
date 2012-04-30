%{
	#include "p2lexer.h"
	#include <stdio.h>

	void yyerror(char* message);
%}

%token PROGRAM END INTEGER STRING BOOLEAN ID TABLE IF THEN ELSE
%token WHILE DO JOIN PROJECT SELECT EXISTS ALL EXTEND UPDATE RENAME
%token READ WRITE AND OR NOT GT GE LT LE EQ NEQ INTCONST STRCONST BOOLCONST
%token IDNAME ERROR

%%

ciao: PROGRAM

%%

void yyerror(char* message){
	fprintf(stderr, "%s\n", message);
}

int main(){
	return yyparse();
}
