all:
	gcc lexer.c compiler.c parser.c  pretty_print.c -g -fsanitize=address -o compiler
