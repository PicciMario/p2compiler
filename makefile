all: parser_ebnf

parser_ebnf: p2lexer.o parser_ebnf.o
	cc p2lexer.o parser_ebnf.o -o parser_ebnf

parser_ebnf.o: parser_ebnf.c
	cc -o parser_ebnf.o -c parser_ebnf.c

p2lexer.o: p2lexer.c
	cc -o p2lexer.o -c p2lexer.c

p2lexer.c: p2lexer.lex
	flex -o p2lexer.c --header-file=p2lexer.h p2lexer.lex
	
clean:
	rm p2lexer.o p2lexer.c p2lexer.h parser_ebnf.o parser_ebnf
