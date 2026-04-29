#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <time.h>
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

    case AST_DATA_TYPES_NULL:
        printf("ir_codgen_error:unexpected datatype\n");
        exit(0);
    case AST_INT_T:
        return 4;
    }
}
void generate_code_expression(AST_EXPR* expr)
{
    //write code to generate x86 intel nasm for evalutation 
    //and output to eax
}
void generate_code_statements(int fd,AST_STATEMENT* stmt,int* function_context,int *var_size)
{
    char temp[66]={0};

    switch(stmt->statement_type)
    {
        case AST_NULL_T:
            printf("ir_codgen error:got null type statement\n");
            break;
        case AST_DEC_T:
            *function_context+=1;   //creates new variable inside function context
            *var_size+=get_size_bytes(stmt->dec_statement->data_type);  //addes size for later
            stmt->dec_statement->identifier->pointer->values.iden_values.temp_val=*var_size;
            //setting the ebp-<value> ast temp_val in symbol table
            sprintf(temp,";create variable varno:%d",*function_context);
            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));
            sprintf(temp, "\tsub esp,%d\n",get_size_bytes(stmt->dec_statement->data_type));
            write(fd,temp,strlen(temp)+1);
            break;
        case AST_INIT_T:
            *function_context+=1;
            *var_size+=get_size_bytes(stmt->init_statement->data_type);
            stmt->init_statement->identifier->pointer->values.iden_values.temp_val=*var_size;
            sprintf(temp,";create and init variable varno:%d\n",*function_context);
            write(fd,temp,strlen(temp)+1);

            memset(temp,0,strlen(temp));
            sprintf(temp, "\tsub esp,%d\n",get_size_bytes(stmt->dec_statement->data_type));
            write(fd,temp,strlen(temp)+1);

            generate_code_expression(stmt->init_statement->expression);
            memset(temp,0,strlen(temp));
            sprintf(temp,"\tmov dword[ebp-%d],eax\n",stmt->init_statement->identifier->pointer->values.iden_values.temp_val);
            write(fd,temp,strlen(temp)+1);
            break;
        case AST_ASSIGN_T:
            generate_code_expression(stmt->assign_statement->expresssion);
            sprintf(temp, "\tmov dword[ebp-%d],eax\n",stmt->assign_statement->identifier->pointer->values.iden_values.temp_val);
            write(fd,temp,strlen(temp)+1);
            break;
        case AST_IF_CASE_T:
            sprintf(temp,"if_start_%d:\n",if_count);
            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));

            generate_code_expression(stmt->if_statement->test_case_expression);
            sprintf(temp,"\tcmp eax,0\n\t je if_b_start%d\n\tjmp if_b_end%d\n",if_count,if_count);

            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->code_block, fd, if_count, function_context, var_size);
            if_count+=1;
            break;
        case AST_WHILE_CASE_T:
            sprintf(temp, "while_start_%d:\n",while_count);
            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));

            generate_code_expression(stmt->while_statement->test_case_expression);
            sprintf(temp,"\tcmp eax,0\n\t je while_b_start%d\n\tjmp while_b_end%d",while_count,while_count);

            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->code_block, fd, while_count, function_context, var_size);
            while_count+=1;
            break;
        case AST_FOR_T:
            sprintf(temp, "for_start_%d:\n",for_count);
            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));

            generate_code_expression(stmt->for_statement->test_case_expression);
            sprintf(temp,"\tcmp eax,0\n\t je for_b_start%d\n\tjmp for_b_end%d",for_count,for_count);

            write(fd,temp,strlen(temp)+1);
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->code_block, fd, for_count,function_context, var_size);
            for_count+=1;
            break;
        case AST_FUNC_T:
            break;
        case AST_FUNC_CALL_T:
            break;
        case AST_RETURN_T:
            write(fd,"\tret\n",6);
            break;
        case AST_CODE_BLOCK_TYPE:
            break;
    }
    if(stmt->statement_type==AST_FUNC_T)
    {
        int stmt_function_context=0;
        int stmt_var_size=0;
        int param_offset=-8;
        AST_FUNC_PARAMS *param_temp=NULL;
        sprintf(temp,"function_%s:",stmt->func_statement->identifier->iden);
        write(fd,temp,strlen(temp)+1);
        memset(temp,0,strlen(temp));

        //set paramters
        param_temp=stmt->func_statement->paramters_list;
        while(param_temp)
        {
            if(param_temp->identifier)
            {
                param_temp->identifier->pointer->values.iden_values.temp_val=param_offset;
                param_offset-=get_size_bytes(param_temp->identifier->pointer->values.iden_values.data_type);
            }
            param_temp=param_temp->next;
        }
        generate_code_codeblock(stmt->code_block,fd,0,&stmt_function_context,&stmt_var_size);
        function_count+=1;
    }
    if(stmt->statement_type==AST_FUNC_CALL_T)
    {
        AST_FUNC_CALL_PARAMS* params_array[stmt->func_call->parameter_count];
        int params_array_count=-1;
        AST_FUNC_CALL_PARAMS* temp_params=stmt->func_call->parameters_list;
        while(temp_params)
        {
            params_array_count+=1;
            params_array[params_array_count]=temp_params;
            temp_params=temp_params->next;
        }
        while(params_array_count>-1)
        {
            generate_code_expression(params_array[params_array_count]->expr);
            write(fd,"\tpush eax\n",11);
            params_array_count-=1;   
        }
        sprintf(temp, "\tcall function_%s\n",stmt->func_call->identifier->iden);
        write(fd,temp,strlen(temp)+1);
        memset(temp,0,strlen(temp));
    }
}
void generate_code_codeblock(AST_CODE_BLOCK* code_block,int fd,int add_val,int* function_context,int* var_size)
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
        	start_ptr="normal_b_start%d:\n";
        	end_ptr="normal_b_end%d:\n";
        	value_at_hand=normal_count;
        	normal_count++;
        	break;
        case AST_IF_CODE_BLOCK:
        	start_ptr="if_b_start%d:\n";
        	end_ptr="if_b_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_FOR_CODE_BLOCK:
        	start_ptr="for_b_start%d:\n";
        	end_ptr="for_b_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_WHILE_CODE_BLOCK:
        	start_ptr="while_b_start%d:\n";
        	end_ptr="while_b_end%d:\n";
        	value_at_hand=add_val;
        	break;
        case AST_FUNC_CODE_BLOCK:
        	start_ptr="function_b_start%d:\n";
        	end_ptr="function_b_end%d:\n";
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
    if(code_block->code_block_type==AST_FUNC_CODE_BLOCK)
    {

        write(fd,"\tpush ebp\n\tmov ebp,esp\n",25);

    }
    AST_STATEMENT* stmt=code_block->statement;
    while(stmt)
    {
    	generate_code_statements(fd,stmt,function_context,var_size);
    	stmt=stmt->next;
    }
    switch(code_block->code_block_type)
    {
        case AST_WHILE_CODE_BLOCK:
            memset(start,0,strlen(start));
            sprintf(start,"\tjmp while_start_%d\n",value_at_hand);
            write(fd,start,strlen(start)+1);
            break;
        case AST_FOR_CODE_BLOCK:
            memset(start,0,strlen(start));
            sprintf(start,"\tjmp for_start_%d\n",value_at_hand);
            write(fd,start,strlen(start)+1);
            break;
        /*case AST_FUNC_CODE_BLOCK:
            write(fd,"\tret\n",6);
            break;*/
        default:
            break;
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