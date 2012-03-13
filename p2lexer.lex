%{
#include <stdio.h>
#include "p2lexer.h"
Lexval lexval;
int line = 1;
%}

%option noyywrap

/* general definitions */

delimiter 	[ \t]
newline		\n
spacing		({delimiter}|{newline})+
letter		[a-zA-Z]
digit		[0-9]

/* constants */

intconst	{digit}+
strconst	\"([^\"])*\"
boolconst	true|false

id			{letter}({letter}|{digit})*

%%

program		{ret(PROGRAM);}
end			{ret(END);}
integer		{ret(INTEGER);}
string		{ret(STRING);}
boolean		{ret(BOOLEAN);}
id			{ret(ID);}
table		{ret(TABLE);}

if			{ret(IF);}
then		{ret(THEN);}
else		{ret(ELSE);}

while		{ret(WHILE);}
do			{ret(DO);}

join		{ret(JOIN);}
project		{ret(PROJECT);}
select		{ret(SELECT);}
exists		{ret(EXISTS);}
all			{ret(ALL);}
extend		{ret(EXTEND);}
update		{ret(UPDATE);}
rename		{ret(RENAME);}

read		{ret(READ);}
write		{ret(WRITE);}

and			{ret(AND);}
or			{ret(OR);}
not			{ret(NOT);}

">"			{ret(GT);}
">="		{ret(GE);}
"<"			{ret(LT);}
"<="		{ret(LE);}
"="			{ret(EQ);}
"!="		{ret(NEQ);}

":"			{ret(':');}
","			{ret(',');}
";"			{ret(';');}
"+"			{ret('+');}
"-"			{ret('-');}
"*"			{ret('*');}
"/"			{ret('/');}
"("			{ret('(');}
")"			{ret(')');}
"["			{ret('[');}
"]"			{ret(']');}
"{"			{ret('{');}
"}"			{ret('}');}

{intconst}	{lexval.ival = atoi(yytext); ret(INTCONST);}
{strconst}	{lexval.sval = newstring(yytext); ret(STRCONST);}
{boolconst}	{lexval.ival = (yytext[0] == 'f' ? 0 : 1); ret(BOOLCONST);}

{id}		{lexval.sval = newstring(yytext); ret(IDNAME);}

{delimiter}	;
{newline}	{line++;}

. 			{ret(ERROR);}

%%

main()
{
  yylex();
}

void ret(int val){
	fprintf(stderr, "line %i: %i (%s)", line, val, yytext);
	
	if (val == INTCONST){ fprintf(stderr, " (int constant: %i)", lexval.ival); }
	
	fprintf(stderr, "\n");
	
	if (val == ERROR){
		fprintf(stderr, "Error on line %i, aborting...\n", line);
		exit(1);
	}
	
	//return val;
}

char *newstring(char *s){
	char *p = malloc(strlen(s) + 1);
	strcpy(p, s);
	return(p);
}

