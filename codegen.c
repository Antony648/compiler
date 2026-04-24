#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
int for_count=0;
int while_count=0;
int if_count=0;
int normal_count=0;
int function_count=0;
//
int get_size_bytes(AST_DATA_TYPES type)
{
    switch(type)
    {
            
    }
}
void generate_code_statements(AST_STATEMENT* stmt,int* function_context)
{
    switch(stmt->statement_type)
    {
        case AST_NULL_T:
            printf("ir_codgen error:got null type statement\n");
            break;
        case AST_DEC_T:

            break;
        case AST_INIT_T:
        case AST_ASSIGN_T:
        case AST_IF_CASE_T:
        case AST_WHILE_CASE_T:
        case AST_FOR_T:
        case AST_FUNC_T:
        case AST_FUNC_CALL_T:
        case AST_RETURN_T:
        case AST_CODE_BLOCK_TYPE:
    }
}
void generate_code_codeblock(AST_CODE_BLOCK* code_block,int fd,int add_val,int* function_context)
{
	char end[33]={0};
	char start[35]={0};
	char* start_ptr=NULL;
	char* end_ptr=NULL;
	int value_at_hand=0;

	switch(code_block->code_block_type)
	{

        case AST_CODE_BLOCK_NULL:
        	printf("ir_codgen error:unrecognized code block\n");
        	break;
        case AST_NORM_CODE_BLOCK:
        	start_ptr="normal_start%d:\n";
        	end_ptr="normal_end%d:\n";
        	value_at_hand=normal_count;
        	normal_count++;
        	break;
        case AST_IF_CODE_BLOCK:
        	start_ptr="if_start%d:\n";
        	end_ptr="if_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_FOR_CODE_BLOCK:
        	start_ptr="for_start%d:\n";
        	end_ptr="for_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_WHILE_CODE_BLOCK:
        	start_ptr="while_start%d:\n";
        	end_ptr="while_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_FUNC_CODE_BLOCK:
        	start_ptr="function_start%d:\n";
        	end_ptr="function_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_PROGRAM:
        	start_ptr="_start:\n";
        	end_ptr="_end:\n";
        	break;
        case AST_FOR_IMPLICT:
        	start_ptr="for_implict_start%d:\n";
        	end_ptr="for_implict_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_FOR_INIT:
        	start_ptr="for_init_start%d:\n";
        	end_ptr="for_init_end%d:\n";
        	value_at_hand=add_val;
        	break;
        default:
        	printf("ir_codgen error:expected code blocks\n");
        	break;
    }
    sprintf(start,start_ptr,value_at_hand);
    sprintf(end,end_ptr,value_at_hand);
    write(fd,start,strlen(start));
    AST_STATEMENT* stmt=code_block->statement;
    while(stmt)
    {
    	generate_code_statements(stmt,function_context);
    	stmt=stmt->next;
    }
    write(fd,end,strlen(end));
    return;
}
bool generate_code(const char* file_name,SYMBOL_TABLE_ELEM* sym_tbl,AST_CODE_BLOCK* parse_tree)
{
	int file_ptr=open(file_name,O_CREAT|O_TRUNC|O_WRONLY);
	if(file_ptr<0)
	{
		printf("codegen:failure in creation of output file\n");
		return false;
	}

	
	return true;
}