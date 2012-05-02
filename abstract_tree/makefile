all: abs_tree

abs_tree: abs_tree.o p2lexer.o node.o
	cc -Wall -Wextra -o abs_tree *.o

node.o: node.c node.h
	cc -std=c99 -Wall -Wextra -o node.o -c node.c

abs_tree.o: abs_tree.c
	cc -Wall -Wextra -o abs_tree.o -c abs_tree.c

abs_tree.c: abs_tree.y p2lexer.c
	bison -vd -o abs_tree.c abs_tree.y

p2lexer.o: p2lexer.c 
	cc -Wall -Wextra -o p2lexer.o -c p2lexer.c

p2lexer.c: p2lexer.lex
	flex -o p2lexer.c --header-file=p2lexer.h p2lexer.lex

clean:
	rm -f abs_tree.c abs_tree.h abs_tree.output p2lexer.c p2lexer.h *.o abs_tree
