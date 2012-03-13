all: p2lexer
p2lexer: p2lexer.c
	cc -o p2lexer p2lexer.c
p2lexer.c: p2lexer.lex
	flex -o p2lexer.c p2lexer.lex