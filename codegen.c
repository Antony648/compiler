#include "codegen.h"
#include "parser.h"
#include "symbol_table.h"
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
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
void generate_code_bin_exp(int fd,AST_EXPR* expr,int line_no)
{
    if(expr->expression.bin_ops==AST_NULL_BIN_OPS_T)
    {
       printf("ir_codgen_warning:line %d:unexpected binary expression",line_no);
       exit(0); 
    }
    generate_code_expression(fd,expr->expression.expr_l,line_no);
    write(fd,"\tpush eax\n",10);
    generate_code_expression(fd,expr->expression.expr_r,line_no);
    write(fd,"\tpop ebx\n",9);
    switch(expr->expression.bin_ops)
    {

    
    case AST_ADD_T:
        write(fd,"\tadd eax,ebx\n",13);
        break;
    //the below four will be handled by for , if ,while
    //as we are already using jump conditions there
    case AST_NEQ_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsetne al\n",10);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_LESS_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsetl al\n",9);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_GREAT_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsetg al\n",9);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_LEQ_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsetle al\n",10);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_GEQ_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsetge al\n",10);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_EQ_T:
        write(fd,"\tcmp ebx,eax\n",13);
        write(fd,"\tsete al\n",9);
        write(fd,"\tmovzx eax,al\n",14);
        break;
    case AST_SUB_T:
        write(fd,"\tsub ebx,eax\n",13);
        write(fd,"\tmov eax,ebx\n",13);
        break;
    case AST_MUL_T:
        write(fd,"\tmul ebx\n",13);
        break;
    case AST_DIV_T:
        write(fd,"\tmov ecx,eax\n",13);
        write(fd,"\tmov eax,ebx\n",13);
        write(fd,"\tdiv ecx\n",9);
        break;
    
    case AST_NULL_BIN_OPS_T: 
      break;
    }
}
void generate_code_expression(int fd,AST_EXPR* expr,int line_no)
{
    //write code to generate x86 intel nasm for evalutation 
    //and output to eax
    if(expr->ast_exp_type==AST_BIN_EXPR_T)
    {
        generate_code_bin_exp(fd, expr, line_no);
        return;
    }

    char temp[66]={0};
    if(expr->ast_exp_type==AST_FUNC_CALL_T)
    {
        //reimplementation of function call more like copy -paste in content of generate_code_statements
        //thinking of calling generate_code_statements on 
        AST_FUNC_CALL_PARAMS* params_array[expr->func_call->parameter_count];
        int params_array_count=-1;
        AST_FUNC_CALL_PARAMS* temp_params=expr->func_call->parameters_list;
        while(temp_params)
        {
            params_array_count+=1;
            params_array[params_array_count]=temp_params;
            temp_params=temp_params->next;
        }
        while(params_array_count>-1)
        {
            generate_code_expression(fd,params_array[params_array_count]->expr,line_no);
            write(fd,"\tpush eax\n",11);
            params_array_count-=1;   
        }
        sprintf(temp, "\tcall function_%s\n",expr->func_call->identifier->iden);
        write(fd,temp,strlen(temp));
        memset(temp,0,strlen(temp));
        return;
    }
    int temp_val=0;
    switch(expr->ast_exp_type)
    {

    case AST_NULL_EXPR_T:
        printf("ir_codgen_warning:line %d:null epxression\n",line_no);
        break;
    case AST_FUNC_CALL_TYPE:
        //implemented above
        break;
    case AST_IDEN_T:
        temp_val=expr->identifier->pointer->values.iden_values.temp_val;
        if(temp_val<0)
            sprintf(temp, "\tmov eax,dword[ebp%d]\n",temp_val);
        else
            sprintf(temp,"\tmov eax,dword[ebp+%d]\n",temp_val);
        write(fd,temp,strlen(temp));

        break;
    case AST_VAL_T:
        sprintf(temp,"\tmov eax,%d\n",expr->value);
        write(fd,temp,strlen(temp));
        break;
    default:
        printf("ir_codgen_warning:line %d:default expression\n",line_no);
      break;
    }
}
void generate_code_statements(int fd,AST_STATEMENT* stmt,int* function_context,int *var_size)
{
    char temp[66]={0};
    int temp_var_size;
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
            sprintf(temp,";create variable varno:%d\n",*function_context);
            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            sprintf(temp, "\tsub esp,%d\n",get_size_bytes(stmt->dec_statement->data_type));
            write(fd,temp,strlen(temp));
            break;
        case AST_INIT_T:
            *function_context+=1;
            *var_size+=get_size_bytes(stmt->init_statement->data_type);
            stmt->init_statement->identifier->pointer->values.iden_values.temp_val=*var_size;
            sprintf(temp,";create and init variable varno:%d\n",*function_context);
            write(fd,temp,strlen(temp));

            memset(temp,0,strlen(temp));
            sprintf(temp, "\tsub esp,%d\n",get_size_bytes(stmt->init_statement->data_type));
            write(fd,temp,strlen(temp));

            generate_code_expression(fd,stmt->init_statement->expression,stmt->line_number);
            memset(temp,0,strlen(temp));
            if((stmt->init_statement->identifier->pointer->values.iden_values.temp_val)>=0)
                sprintf(temp,"\tmov dword[ebp+%d],eax\n",stmt->init_statement->identifier->pointer->values.iden_values.temp_val);
            else
                sprintf(temp,"\tmov dword[ebp%d],eax\n",stmt->init_statement->identifier->pointer->values.iden_values.temp_val);
            write(fd,temp,strlen(temp));
            break;
        case AST_ASSIGN_T:
            generate_code_expression(fd,stmt->assign_statement->expresssion,stmt->line_number);
            if((stmt->assign_statement->identifier->pointer->values.iden_values.temp_val)>=0)
                sprintf(temp, "\tmov dword[ebp+%d],eax\n",stmt->assign_statement->identifier->pointer->values.iden_values.temp_val);
            else
                sprintf(temp, "\tmov dword[ebp%d],eax\n",stmt->assign_statement->identifier->pointer->values.iden_values.temp_val);
            write(fd,temp,strlen(temp));
            break;
        //we are going to handle not equal, greaterthan, less than, greater than or equ, less than or equ
        //below it will increase code size but will reduce redundant code  lines in output asm
        case AST_IF_CASE_T:
            sprintf(temp,"if_start_%d:\n",if_count);
            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            temp_var_size=*var_size;
            if(stmt->if_statement->test_case_expression->ast_exp_type==AST_BIN_EXPR_T)
            {
                generate_code_bin_exp(fd,stmt->if_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne if_b_start%d\n\tjmp if_b_end%d\n",if_count,if_count);
            }
            else
            {
                generate_code_expression(fd,stmt->if_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne if_b_start%d\n\tjmp if_b_end%d\n",if_count,if_count);
            }

            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->if_statement->code_block, fd, if_count, function_context, var_size);
            if_count+=1;
            if(*var_size>temp_var_size)
            {
                //reclaim space obtained by for subset
                sprintf(temp,"\tadd esp,%d\n",*var_size-temp_var_size);
                write(fd,temp,strlen(temp));
                *var_size=temp_var_size;
            }
            break;
        case AST_WHILE_CASE_T:
            sprintf(temp, "while_start_%d:\n",while_count);
            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            temp_var_size=*var_size;
            if(stmt->while_statement->test_case_expression->ast_exp_type==AST_BIN_EXPR_T)
            {
                generate_code_bin_exp(fd,stmt->while_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne while_b_start%d\n\tjmp while_b_end%d\n",if_count,if_count);
            }
            else
            {
                generate_code_expression(fd,stmt->while_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne while_b_start%d\n\tjmp while_b_end%d\n",while_count,while_count);
            }

            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->while_statement->code_block, fd, while_count, function_context, var_size);
            while_count+=1;
            if(*var_size>temp_var_size)
            {
                //reclaim space obtained by for subset
                sprintf(temp,"\tadd esp,%d\n",*var_size-temp_var_size);
                write(fd,temp,strlen(temp));
                *var_size=temp_var_size;
            }
            break;
        case AST_FOR_T:
            sprintf(temp, "for_start_%d:\n",for_count);
            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            temp_var_size=*var_size;
            if(stmt->for_statement->init_expressions)
                generate_code_codeblock(stmt->for_statement->init_expressions, fd, for_count, function_context, var_size);
            if(stmt->for_statement->test_case_expression->ast_exp_type==AST_BIN_EXPR_T)
            {
                generate_code_bin_exp(fd,stmt->for_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne for_b_start%d\n\tjmp for_b_end%d\n",if_count,if_count);
            }
            else
            {
                generate_code_expression(fd,stmt->for_statement->test_case_expression,stmt->line_number);
                sprintf(temp,"\tcmp eax,0\n\tjne for_b_start%d\n\tjmp for_b_end%d\n",for_count,for_count);
            }
            write(fd,temp,strlen(temp));
            memset(temp,0,strlen(temp));
            generate_code_codeblock(stmt->for_statement->code_block, fd, for_count,function_context, var_size);
            for_count+=1;
            if(*var_size>temp_var_size)
            {
                //reclaim space obtained by for subset
                sprintf(temp,"\tadd esp,%d\n",*var_size-temp_var_size);
                write(fd,temp,strlen(temp));
                *var_size=temp_var_size;
            }
            break;
        case AST_FUNC_T:
            break;
        case AST_FUNC_CALL_T:
            break;
        case AST_RETURN_T:
            generate_code_expression(fd, stmt->return_statement->expression, stmt->line_number);
            write(fd,"\tret\n",5);
            break;
        case AST_CODE_BLOCK_TYPE:
            temp_var_size=*var_size;
            generate_code_codeblock(stmt->code_block, fd, 0, function_context, var_size);
            if(*var_size>temp_var_size)
            {
                //reclaim space obtained by for subset
                sprintf(temp,"\tadd esp,%d\n",*var_size-temp_var_size);
                write(fd,temp,strlen(temp));
                *var_size=temp_var_size;
            }
            break;
    }
    if(stmt->statement_type==AST_FUNC_T)
    {
        int stmt_function_context=0;
        int stmt_var_size=0;
        int param_offset=-8;
        AST_FUNC_PARAMS *param_temp=NULL;
        sprintf(temp,"function_%s:\n",stmt->func_statement->identifier->iden);
        write(fd,temp,strlen(temp));
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
        generate_code_codeblock(stmt->func_statement->code_block,fd,function_count,&stmt_function_context,&stmt_var_size);
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
            generate_code_expression(fd,params_array[params_array_count]->expr,stmt->line_number);
            write(fd,"\tpush eax\n",11);
            params_array_count-=1;   
        }
        sprintf(temp, "\tcall function_%s\n",stmt->func_call->identifier->iden);
        write(fd,temp,strlen(temp));
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
        	start_ptr="global _start:\n\tjmp function_main\n";
        	end_ptr="\tmov ebx,eax\n\tmov eax,1\n\tint 0x80\n\t_end:\n";
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

        write(fd,"\tpush ebp\n\tmov ebp,esp\n",23);

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
            write(fd,start,strlen(start));
            break;
        case AST_FOR_CODE_BLOCK:
            memset(start,0,strlen(start));
            sprintf(start,"\tjmp for_init_end%d\n",value_at_hand);
            write(fd,start,strlen(start));
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
	int file_ptr=open(file_name,O_CREAT|O_TRUNC|O_WRONLY,0644);
	if(file_ptr<0)
	{
		perror("codegen:failure in creation of output file\n");
		return false;
	}
    generate_code_codeblock(parse_tree,file_ptr, 0, &function_count, 0);
	close(file_ptr);
	return true;
}