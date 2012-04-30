%{
#include <stdio.h>
#include "abs_tree.h"
//Lexval lexval;
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

program		{return(PROGRAM);}
end			{return(END);}
integer		{return(INTEGER);}
string		{return(STRING);}
boolean		{return(BOOLEAN);}
id			{return(ID);}
table		{return(TABLE);}

if			{return(IF);}
then		{return(THEN);}
else		{return(ELSE);}

while		{return(WHILE);}
do			{return(DO);}

join		{return(JOIN);}
project		{return(PROJECT);}
select		{return(SELECT);}
exists		{return(EXISTS);}
all			{return(ALL);}
extend		{return(EXTEND);}
update		{return(UPDATE);}
rename		{return(RENAME);}

read		{return(READ);}
write		{return(WRITE);}

and			{return(AND);}
or			{return(OR);}
not			{return(NOT);}

">"			{return(GT);}
">="		{return(GE);}
"<"			{return(LT);}
"<="		{return(LE);}
"=="		{return(EQ);}
"!="		{return(NEQ);}

":"			{return(':');}
","			{return(',');}
";"			{return(';');}
"+"			{return('+');}
"-"			{return('-');}
"*"			{return('*');}
"/"			{return('/');}
"("			{return('(');}
")"			{return(')');}
"["			{return('[');}
"]"			{return(']');}
"{"			{return('{');}
"}"			{return('}');}
"="			{return('=');}

{intconst}	//{lexval.ival = atoi(yytext); return(INTCONST);}
{strconst}	//{lexval.sval = newstring(yytext); return(STRCONST);}
{boolconst}	//{lexval.ival = (yytext[0] == 'f' ? 0 : 1); return(BOOLCONST);}

{id}		//{lexval.sval = newstring(yytext); return(IDNAME);}

{delimiter}	;
{newline}	//{line++;}

. 			{return(ERROR);}

%%

//char *newstring(char *s){
//	char *p = malloc(strlen(s) + 1);
//	strcpy(p, s);
//	return(p);
//}

