#include "lexer.h"
#include "parser.h"
#include "pretty_print.h"
#include "symbol_table.h"
#include "typechecker.h"
#include "codegen.h"
#include <fcntl.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
	char *file_name;
	if(argc<2)
	{
		file_name="./output.asm";
	}
	else
		file_name=argv[1];
	FILE* file_ptr=NULL;
	file_ptr=fopen("input.txt","r");
	if(!file_ptr)
	{
		printf("failed in opening file\n");
		return -1;
	}
	init_token_train();
	populate_token_train(file_ptr);
	AST_CODE_BLOCK* ast_tree=get_code_block(TOKEN_EOF,AST_PROGRAM);
	if(!ast_tree)
	{
		printf("failure in generation ast tree.\n");
		goto error_end;
	}
	pretty_print(ast_tree);
	SYMBOL_TABLE_ELEM *symb_tbl=generate_symbol_table(ast_tree, NULL, 0, NULL);
	if(generate_code(file_name, symb_tbl, ast_tree))
		printf("output in %s\n",file_name);
	else
		printf("failure in codegen phase\n");
	death_lever();
	destroy_code_block(ast_tree);
error_end:
	destroy_token_train();
	fclose(file_ptr);
	
}