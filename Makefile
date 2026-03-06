all:
	gcc lexer.c compiler.c parser.c -g -fsanitize=address -o compiler
