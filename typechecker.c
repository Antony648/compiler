#include "typechecker.h"
#include "symbol_table.h"
#include <stdio.h>
#include "parser.h"
#include <string.h>
SYMBOL_TABLE_ELEM* last=NULL;
int leading_scope=0;
void death_lever()
{
	//most dangerous function implementation ever
	//highly prone to mem leakage
	exit(1);
}
SYMBOL_TABLE_ELEM* find_sym_tbl_elem(SYMBOL_TABLE_ELEM* reverse_end,char* identifier)
{
	int len=strlen(identifier)+1;
	while(reverse_end)
	{
		if(reverse_end->is_start)
			goto jump;
		if(!strncmp(reverse_end->values.identifier,identifier,len))
			return reverse_end;
jump:
		reverse_end=reverse_end->prev;
	}
	return NULL;
}
void find_expression_symbols(SYMBOL_TABLE_ELEM* reverse_end,AST_EXPR* expression,int line_number)
{
  	switch (expression->ast_exp_type)
  	{
	  case AST_NULL_EXPR_T:
	  	return;
	  case AST_IDEN_T:
	  {
	  	SYMBOL_TABLE_ELEM*temp=find_sym_tbl_elem(reverse_end, expression->identifier->iden);
	  	if(!temp)
	  	{
	  		printf("sematic error:%d:usage without decclaration variable '%s'\n",line_number,expression->identifier->iden);
	        death_lever();
	  	}
	  	expression->identifier->pointer=temp;
	  	return;
	  }
	  case AST_VAL_T:
	  	return ;
	  case AST_BIN_EXPR_T:
	  	{
	  		find_expression_symbols(reverse_end, expression->expression.expr_l, line_number);
	  		find_expression_symbols(reverse_end, expression->expression.expr_r, line_number);
	  		return;
	  	}
    
  	}
}
SYMBOL_TABLE_ELEM* get_sym_tbl_malloc(char* identifier)
{
	SYMBOL_TABLE_ELEM* temp=malloc(sizeof(SYMBOL_TABLE_ELEM));
	if(temp)
	{
		printf("malloc failure\n");
		death_lever();
	}
	memset(temp,0,sizeof(SYMBOL_TABLE_ELEM));
	if(!identifier)
		return temp;
	int len=strlen(identifier)+1;
	temp->values.identifier=malloc(len);
	if(temp->values.identifier)
	{
		printf("malloc failure\n");
		death_lever();
	}
	strncpy(temp->values.identifier,identifier,len);
	return temp;
}
void get_symb_tbl_assign(SYMBOL_TABLE_ELEM* rtn_val,AST_STATEMENT* temp)
{
	SYMBOL_TABLE_ELEM* assign_temp=find_sym_tbl_elem(rtn_val, temp->assign_statement->identifier->iden);
  	if(!assign_temp)
  	{
  		printf("sematic error:%d:usage without decclaration variable '%s'\n",temp->line_number,temp->assign_statement->identifier->iden);
  		death_lever();
  	}
  	temp->assign_statement->identifier->pointer=assign_temp;
  	find_expression_symbols(rtn_val,temp->assign_statement->expresssion,temp->line_number);
  	return;
}
void get_symb_tbl_init(SYMBOL_TABLE_ELEM** rtn_val_addr,AST_STATEMENT* temp)
{
	SYMBOL_TABLE_ELEM* rtn_val=*rtn_val_addr;
	SYMBOL_TABLE_ELEM* init_temp=get_sym_tbl_malloc(temp->init_statement->identifier->iden);
  	init_temp->prev=rtn_val;
  	//rtn_val->values.references++;
  	rtn_val=init_temp;
  	temp->init_statement->identifier->pointer=init_temp;
}
void get_symb_tbl_dec(SYMBOL_TABLE_ELEM** rtn_val_addr,AST_STATEMENT* temp)
{
	SYMBOL_TABLE_ELEM* rtn_val=*rtn_val_addr;
	SYMBOL_TABLE_ELEM* dec_temp=get_sym_tbl_malloc(temp->dec_statement->identifier->iden);
  	dec_temp->prev=rtn_val;
  	//rtn_val->values.references++;
  	temp->dec_statement->identifier->pointer=dec_temp;
  	rtn_val=dec_temp;
  	return;
}
SYMBOL_TABLE_ELEM* generate_symbol_table(AST_CODE_BLOCK* ast_tree,SYMBOL_TABLE_ELEM* prev)
{
	SYMBOL_TABLE_ELEM* rtn_val=get_sym_tbl_malloc(NULL);
	rtn_val->is_start=true;
	rtn_val->prev=prev;
	if(prev)
		prev->values.references++;
	AST_STATEMENT* temp=ast_tree->statement;
	while(temp)
	{
          switch (temp->statement_type) {
          case AST_NULL_T:
          {
          	printf("null type statement\n");
          	break;
          }
          case AST_DEC_T:
          {
          	get_symb_tbl_dec(&rtn_val,temp);
          	break;
          }
          case AST_INIT_T:
          {
          	get_symb_tbl_init(&rtn_val,temp);
          	break;
          }
          case AST_ASSIGN_T:
          {
          	get_symb_tbl_assign(rtn_val,temp);
          	break;
          }
          case AST_IF_CASE_T:
          {
          	find_expression_symbols(rtn_val, temp->if_statement->test_case_expression, temp->line_number);
          	generate_symbol_table(temp->if_statement->code_block, rtn_val);
          	break;
          }
          case AST_WHILE_CASE_T:
          {
          	find_expression_symbols(rtn_val, temp->while_statement->test_case_expression, temp->line_number);
          	generate_symbol_table(temp->while_statement->code_block, rtn_val);
          }
          case AST_FOR_T:
          {

          }
          case AST_FUNC_T:
          {
          	
          }
          case AST_FUNC_CALL_T:
          {
          	SYMBOL_TABLE_ELEM* fucn_call_temp=find_sym_tbl_elem(rtn_val,temp->func_call->identifier->iden);
          	if(!fucn_call_temp)
          	{
          		printf("semantic error:%d:calling undecclared function '%s'\n",temp->line_number,temp->func_call->identifier->iden);
          		death_lever();		
          	}
          	AST_FUNC_CALL_PARAMS* i=temp->func_call->parameters_list;
          	while(i)
          	{
          		find_expression_symbols(rtn_val, i->expr, temp->line_number);
          		i=i->next;
          	}
          }
          case AST_RETURN_T:
          {
          	find_expression_symbols(rtn_val,temp->return_statement->expression,temp->line_number);
          	break;
          }
          case AST_CODE_BLOCK_TYPE:
            break;
          }
                temp=temp->next;
	}


	ast_tree->last=rtn_val;
	
	return rtn_val;

}