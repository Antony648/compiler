all:
	gcc lexer.c compiler.c -g -fsanitize=address -o compiler