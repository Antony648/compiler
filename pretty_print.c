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
        	printf("function\n");
        	break;	
        case AST_PROGRAM:
        	printf("start of program\n");
        	break;
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
	printf("initialize:%s:\n",param1->identifier->iden);
	print_expression(param1->expression);
	return;
}
void print_assign(AST_ASSIGN* param1){
	printf("assign:%s:\n",param1->identifier->iden);
	print_expression(param1->expresssion);
	return;
}
void print_if(AST_IF_CASE* param1){
	printf("if test case:\n");
	print_expression(param1->test_case_expression);
	printf("if code block\n");
	pretty_print(param1->code_block);
	return;
}
void print_while(AST_WHILE_CASE* param1){
	printf("while test case:\n");
	print_expression(param1->test_case_expression);
	printf("while code block\n");
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
void print_fucntion_params(AST_FUNC_PARAMS* param1)
{
	while(param1)
	{
          switch (param1->identifier->data_type) 
          {
	          case AST_DATA_TYPES_NULL:
	          	printf("illegal data type null");
	          	break;
	          case AST_INT_T:
	          	printf("datatype:int:");
	            break;
          }
			printf("name:%s\n",param1->identifier->iden);
		param1=param1->next;
	}
}
void print_function(AST_FUNC* param1){
	printf("function:%s\n",param1->identifier->iden);
	print_fucntion_params(param1->paramters_list);
	printf("code block\n");
	pretty_print(param1->code_block);
	return;
}
void print_function_call(AST_FUNC_CALL* param1){
	
}
void print_retrun(AST_RETURN* param1){}

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
        
        }
}
void print_expression(AST_EXPR*);
