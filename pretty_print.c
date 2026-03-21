#include "pretty_print.h"
#include "parser.h"


void pretty_print(AST_CODE_BLOCK* param1)
{
	switch(param1->code_block_type)
	{

        case AST_CODE_BLOCK_NULL:
        	printf("null code block:illegal\n");
        	break;
        case AST_IF_CODE_BLOCK:
        	printf("if code block\n");
        	break;
        case AST_FOR_CODE_BLOCK:
        	printf("for code block\n");
        	break;
        case AST_WHILE_CODE_BLOCK:
        	printf("while code block\n");
        	break;
        case AST_FUNC_CODE_BLOCK:
        	printf("function code block\n");
        	break;	
        case AST_PROGRAM:
        	printf("start of program\n");
        	break;
        case AST_FOR_IMPLICT:
        	printf("for implict section\n");
        	break;
        case AST_FOR_INIT:
        	printf("for init section\n");
          break;
        case AST_NORM_CODE_BLOCK:
        	printf("normal code block\n");
          break;
        }
    AST_STATEMENT* temp=param1->statement;
    while(temp)
    {
    	print_statement(temp);
    	temp=temp->next;
    }

}
void print_decclare(AST_DEC* param1){
	printf("decclare:%s\n",param1->identifier->iden);
	return;
}
void print_init(AST_INIT* param1){
	printf("initialize:%s:",param1->identifier->iden);
	print_expression(param1->expression);
	return;
}
void print_assign(AST_ASSIGN* param1){
	printf("assign:%s:",param1->identifier->iden);
	print_expression(param1->expresssion);
	return;
}
void print_if(AST_IF_CASE* param1){
	printf("if test case:\n");
	print_expression(param1->test_case_expression);
	pretty_print(param1->code_block);
	return;
}
void print_while(AST_WHILE_CASE* param1){
	printf("while test case:\n");
	print_expression(param1->test_case_expression);
	pretty_print(param1->code_block);
	return;
}
void print_for(AST_FOR_CASE* param1){
	printf("for test case:\ninit expressions for:\n");
	pretty_print(param1->init_expressions);
	printf("expression for:\n");
	print_expression(param1->test_case_expression);
	printf("implict expressions for:");
	pretty_print(param1->implict_expressions);
	printf("code block for:\n");
	pretty_print(param1->code_block);
	return;

}
void print_fucntion_params(AST_FUNC_PARAMS* paramlist,int count)
{
	printf("function params:\n");
	while(  paramlist && paramlist->identifier)
	{
          switch (paramlist->identifier->data_type) 
          {
	          case AST_DATA_TYPES_NULL:
	          	printf("illegal data type null");
	          	break;
	          case AST_INT_T:
	          	printf("datatype:int:");
	            break;
          }
			printf("name:%s\n",paramlist->identifier->iden);
		paramlist=paramlist->next;
		count--;
	}
}
void print_fucntion_call_params(AST_FUNC_CALL_PARAMS* parameterlist, int count)
{
	printf("function call paramters\n");
	AST_FUNC_CALL_PARAMS* temp=parameterlist;
	while( temp)
	{
		print_expression(temp->expr);
		count--;
		temp=temp->next;
	}
}
void print_function(AST_FUNC* param1){
	printf("function:%s\n",param1->identifier->iden);
	print_fucntion_params(param1->paramters_list,param1->parameter_count);
	pretty_print(param1->code_block);
	return;
}
void print_function_call(AST_FUNC_CALL* param1){
	printf("function call:%s\n",param1->identifier->iden);
	print_fucntion_call_params(param1->parameters_list,param1->parameter_count);
	return;
}
void print_retrun(AST_RETURN* param1){
	printf("return ");
	print_expression(param1->expression);
	return;
}

void print_statement(AST_STATEMENT* param1)
{
	switch(param1->statement_type)
	{

        case AST_NULL_T:
        	printf("null statement :illegal\n");
        	break;
        case AST_DEC_T:
        	print_decclare(param1->dec_statement);
        	break;
        case AST_INIT_T:
        	print_init(param1->init_statement);
        	break;
        case AST_ASSIGN_T:
        	print_assign(param1->assign_statement);
        	break;
        case AST_IF_CASE_T:
        	print_if(param1->if_statement);
        	break;
        case AST_WHILE_CASE_T:
        	print_while(param1->while_statement);
        	break;
        case AST_FOR_T:
        	print_for(param1->for_statement);
        	break;
        case AST_FUNC_T:
        	print_function(param1->func_statement);
        	break;
        case AST_FUNC_CALL_T:
        	print_function_call(param1->func_call);
        	break;
        case AST_RETURN_T:
        	print_retrun(param1->return_statement);
        	break;
        case AST_CODE_BLOCK_TYPE:
        	pretty_print(param1->code_block);
        	break;
        
        }
}
void print_expression(AST_EXPR* param1)
{
	printf("expression:");
	if(!param1)
		return ;
	switch(param1->ast_exp_type)
	{

        case AST_NULL_EXPR_T:
        	printf("error null expression\n");
        	break;
        case AST_IDEN_T:
        	printf("identifier:%s\n",param1->identifier->iden);
        	break;
        case AST_VAL_T:
        	printf("value:%d\n",param1->value);
        	break;
        case AST_BIN_EXPR_T:
        	printf("\n\t");
        	printf("operation\n");
        	//print operation first
        	switch(param1->expression.bin_ops)
        	{

                case AST_NULL_BIN_OPS_T:
                	printf("null case illegal\n");
                	break;
                case AST_ADD_T:
                	printf("+\n");
                	break;
                case AST_SUB_T:
                	printf("-\n");
                	break;
                case AST_MUL_T:
                	printf("*\n");
                	break;
                case AST_DIV_T:
                	printf("/\n");
                	break;
                case AST_EQ_T:
                	printf("==\n");
                	break;
                case AST_NEQ_T:
                	printf("!=\n");
                	break;
                case AST_LESS_T:
                	printf("<\n");
                	break;
                case AST_GREAT_T:
                	printf(">\n");
                	break;
                case AST_LEQ_T:
                	printf("<=\n");
                	break;
                case AST_GEQ_T:
                	printf(">=\n");
                  break;
                }
					printf("left expression:");
        	print_expression(param1->expression.expr_l);
        	printf("\tright expression:");
        	print_expression(param1->expression.expr_r);
        	printf("\n");
          break;
        }
}