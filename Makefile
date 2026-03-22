all:
	gcc lexer.c compiler.c parser.c  pretty_print.c  typechecker.c -g -fsanitize=address -o compiler
