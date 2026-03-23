#include "typechecker.h"
#include "symbol_table.h"
#include <stdio.h>
#include "parser.h"
#include <string.h>
SYMBOL_TABLE_ELEM* last=NULL;
DEATH_MAP_ELEM* end=NULL;
int leading_scope=0;
void get_symb_tbl_func_call(SYMBOL_TABLE_ELEM*,AST_FUNC_CALL*,int);
AST_DATA_TYPES get_expr_data_type(AST_EXPR* expression)
{
	//for now we can assume all expression 
	//return int as datatype
	return AST_INT_T;
}
void destroy_symbol_tbl_elem(SYMBOL_TABLE_ELEM* elem)
{
	if(!elem)
		return;
	if(elem->elem_type==SYMB_TBL_IDEN)
	{
		 if(elem->values.iden_values.identifier)
		 	free(elem->values.iden_values.identifier);

	}
	free(elem);
	return;
}
void death_start(SYMBOL_TABLE_ELEM* elem)
{

	while(elem)
	{
		if(elem->references)
			return;
		if(elem->prev)
			elem->prev->references--;

		
		SYMBOL_TABLE_ELEM* temp=elem->prev;
		destroy_symbol_tbl_elem(elem);
		elem=temp;
		
	}
}
void death_lever()
{
	//most dangerous function implementation ever
	//highly prone to mem leakage
	while(end)
	{
		DEATH_MAP_ELEM* temp=end;
		end=end->prev;
		death_start(temp->pointer);
		free(temp);
	}
	exit(1);
}
void add_death_map(SYMBOL_TABLE_ELEM* elem)
{
	DEATH_MAP_ELEM* temp=malloc(sizeof(DEATH_MAP_ELEM));
	if(!temp)
	{
		printf("malloc failure death map elem add death map\n");
		death_lever();
	}
	memset(temp,0,sizeof(DEATH_MAP_ELEM));
	temp->pointer=elem;
	if(end)
		temp->prev=end;
	end=temp;
	return;
}
int min(int a,int b)
{
	return a<b?a:b;
}
SYMBOL_TABLE_ELEM* find_sym_tbl_elem(SYMBOL_TABLE_ELEM* reverse_end,char* identifier,SYMBOL_TABLE_ELEM_T type)
{
	int len=strlen(identifier)+1;
	while(reverse_end)
	{
		if(reverse_end->elem_type==SYMB_TBL_START || reverse_end->elem_type!=type)
			goto jump;
    switch (type) 
    {
	    case SYMB_TBL_NULL:
	    	printf("null symbol :find_sym_tbl_elem\n");
	    	break;
	    case SYMB_TBL_START:
	    	printf("start symbol:find_sym_tbl_elem\n");
	    	break;
	    case SYMB_TBL_IDEN:
    	{
    		int len2=strlen(reverse_end->values.iden_values.identifier)+1;
    		if(!strncmp(reverse_end->values.iden_values.identifier,identifier,min(len,len2)))
					return reverse_end;
				break;
    	}
	    case SYMB_TBL_FUNC:
	    {
	    	int len2=strlen(((AST_FUNC*)reverse_end->values.func_values.function)->identifier->iden);
	    	if(!strncmp(((AST_FUNC*)reverse_end->values.func_values.function)->identifier->iden, identifier, min(len,len2)))
	    	{
	    		return  reverse_end;
	    	}
	      break;
	    }
    }
jump:
		reverse_end=reverse_end->prev;
	}
	return NULL;
}
void find_expression_symbols(SYMBOL_TABLE_ELEM* reverse_end,AST_EXPR* expression,int line_number)
{
		if(!expression)
				return;
  	switch (expression->ast_exp_type)
  	{
		  case AST_NULL_EXPR_T:
		  	return;
		  case AST_IDEN_T:
		  {
		  	SYMBOL_TABLE_ELEM*temp=find_sym_tbl_elem(reverse_end, expression->identifier->iden,SYMB_TBL_IDEN);
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

	    case AST_FUNC_CALL_TYPE:
	      {
	      	get_symb_tbl_func_call(reverse_end,expression->func_call,line_number);
	      	return;
	      }
  }
}
SYMBOL_TABLE_ELEM* get_sym_tbl_malloc(char* identifier)
{
	SYMBOL_TABLE_ELEM* temp=malloc(sizeof(SYMBOL_TABLE_ELEM));
	if(!temp)
	{
		printf("malloc failure symbtbl get symb tbl malloc\n");
		death_lever();
	}
	memset(temp,0,sizeof(SYMBOL_TABLE_ELEM));
	if(!identifier)
		return temp;
	int len=strlen(identifier)+1;
	temp->values.iden_values.identifier=malloc(len);
	if(!temp->values.iden_values.identifier)
	{
		printf(" malloc failure identifier  get_sym_tbl malloc\n");
		death_lever();
	}
	strncpy(temp->values.iden_values.identifier,identifier,len);
	return temp;
}

void get_symb_tbl_assign(SYMBOL_TABLE_ELEM* rtn_val,AST_STATEMENT* temp)
{
	SYMBOL_TABLE_ELEM* assign_temp=find_sym_tbl_elem(rtn_val, temp->assign_statement->identifier->iden,SYMB_TBL_IDEN);
	if(!assign_temp)
	{
		printf("sematic error:%d:usage without decclaration variable '%s'\n",temp->line_number,temp->assign_statement->identifier->iden);
		if(!rtn_val->references)
			death_start(rtn_val);
		death_lever();
	}
	if(assign_temp->values.iden_values.data_type!=get_expr_data_type(temp->assign_statement->expresssion))
	{
		printf("semantic error:%d:assignment of wrong datatype ",temp->line_number);
		if(!rtn_val->references)
			death_start(rtn_val);
		death_lever();
	}
	temp->assign_statement->identifier->pointer=assign_temp;
	find_expression_symbols(rtn_val,temp->assign_statement->expresssion,temp->line_number);
	return;
}
bool is_present_in_same_scope(SYMBOL_TABLE_ELEM* end,char* identifier)
{
	while(end && end->elem_type!=SYMB_TBL_START)
	{
		if(!strncmp(end->values.iden_values.identifier,identifier,strlen(identifier)))
			return true;
		end=end->prev;
	}
	return false;
}
void get_symb_tbl_init(SYMBOL_TABLE_ELEM** rtn_val_addr,AST_INIT* init_stmt,int line_number)
{
		if(init_stmt->data_type!=get_expr_data_type(init_stmt->expression))
		{
			printf("semantic error:%d:mismatch in init\n",line_number);
			if(!(*rtn_val_addr)->references)
				death_start(*rtn_val_addr);
			death_lever();
		}
		if(is_present_in_same_scope(*rtn_val_addr, init_stmt->identifier->iden))
		{
			printf("semantic error:%d:decclared before in same scope\n",line_number);
			if(!(*rtn_val_addr)->references)
				death_start(*rtn_val_addr);
			death_lever();
		}
		
		SYMBOL_TABLE_ELEM* init_temp=get_sym_tbl_malloc(init_stmt->identifier->iden);
  	init_temp->prev=*rtn_val_addr;
  	init_temp->values.iden_values.data_type=init_stmt->data_type;
  	init_temp->elem_type=SYMB_TBL_IDEN;
  	(*rtn_val_addr)->references++;
  	*rtn_val_addr=init_temp;
  	init_stmt->identifier->pointer=init_temp;
}

void get_symb_tbl_dec(SYMBOL_TABLE_ELEM** rtn_val_addr,AST_IDEN* identifier,AST_DATA_TYPES data_t)
{
		
		SYMBOL_TABLE_ELEM* dec_temp=get_sym_tbl_malloc(identifier->iden);
  	dec_temp->prev=*rtn_val_addr;
  	dec_temp->values.iden_values.data_type=data_t;
  	dec_temp->elem_type=SYMB_TBL_IDEN;
  	(*rtn_val_addr)->references++;
  	identifier->pointer=dec_temp;
  	*rtn_val_addr=dec_temp;
  	return;
}

void get_symb_tbl_func_call(SYMBOL_TABLE_ELEM* rtn_val,AST_FUNC_CALL* func_call,int line_number)
{
	SYMBOL_TABLE_ELEM* fucn_call_temp=find_sym_tbl_elem(rtn_val,func_call->identifier->iden,SYMB_TBL_FUNC);
	if(!fucn_call_temp)
	{
		printf("semantic error:%d:calling undecclared function '%s'\n",line_number,func_call->identifier->iden);
		if(!rtn_val->references)
			death_start(rtn_val);
		death_lever();		
	}
	AST_FUNC_CALL_PARAMS* i=func_call->parameters_list;
	while(i)
	{
		find_expression_symbols(rtn_val, i->expr, line_number);
		i=i->next;
	}
}

SYMBOL_TABLE_ELEM* generate_symbol_table(AST_CODE_BLOCK* ast_tree,SYMBOL_TABLE_ELEM* prev,int mode,AST_STATEMENT* passed)
{
	//mode
	//0 ->normal functions
	//1->function codes 
	//2->for codes
	SYMBOL_TABLE_ELEM* rtn_val=get_sym_tbl_malloc(NULL);
	rtn_val->elem_type=SYMB_TBL_START;
	rtn_val->prev=prev;
	if(prev)
		prev->references++;
	AST_STATEMENT* temp=ast_tree->statement;
	if(mode==1)
	{
		if(!passed)
		{
			printf("internal error:calling mode without passing statment\n");
			if(!rtn_val->references)
				death_start(rtn_val);
			death_lever();
		}
		if(passed->statement_type!=AST_FUNC_T)
		{
			printf("illegal call\n");
			if(!rtn_val->references)
				death_start(rtn_val);
			death_lever();
		}
		AST_FUNC_PARAMS* func_type=passed->func_statement->paramters_list;
		while(func_type)
		{
			if(func_type->identifier)
			{
				get_symb_tbl_dec(&rtn_val, func_type->identifier,func_type->data_type);
			}
			func_type=func_type->next;
		}

	}
	else if(mode ==2)
	{
		if(!passed)
		{
			printf("internal error:calling mode without passing statment\n");
			if(!rtn_val->references)
				death_start(rtn_val);
			death_lever();
		}
		if(passed->statement_type!=AST_FOR_T)
		{
			printf("illegal case\n");
			if(!rtn_val->references)
				death_start(rtn_val);
			death_lever();
		}
		if(!passed->for_statement->init_expressions)
			goto main_code_start;
		AST_STATEMENT *for_temp_init=passed->for_statement->init_expressions->statement;
		while(for_temp_init)
		{
			switch(for_temp_init->statement_type)
			{
				case AST_DEC_T:
          {
          	get_symb_tbl_dec(&rtn_val,for_temp_init->dec_statement->identifier,for_temp_init->dec_statement->data_type);
          	break;
          }
        case AST_INIT_T:
          {
          	get_symb_tbl_init(&rtn_val,for_temp_init->init_statement,for_temp_init->line_number);
          	break;
          }
        case AST_ASSIGN_T:
          {
          	get_symb_tbl_assign(rtn_val,for_temp_init);
          	break;
          }
         default:
         	 {
         	 	printf("sematic layer:unsupported type statements found in for init\n");
         	 	break;
         	 }
			}
			for_temp_init=for_temp_init->next;
		}
	}
main_code_start:
	while(temp)
	{
      switch (temp->statement_type) 
      {
          case AST_NULL_T:
          {
          	printf("null type statement\n");
          	break;
          }
          case AST_DEC_T:
          {
          	get_symb_tbl_dec(&rtn_val,temp->dec_statement->identifier,temp->dec_statement->data_type);
          	break;
          }
          case AST_INIT_T:
          {
          	get_symb_tbl_init(&rtn_val,temp->init_statement,temp->line_number);
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
          	generate_symbol_table(temp->if_statement->code_block, rtn_val,0,NULL);
          	break;
          }
          case AST_WHILE_CASE_T:
          {
          	find_expression_symbols(rtn_val, temp->while_statement->test_case_expression, temp->line_number);
          	generate_symbol_table(temp->while_statement->code_block, rtn_val,0,NULL);
          	break;
          }
          case AST_FOR_T:
          {
          	generate_symbol_table(temp->for_statement->code_block, rtn_val, 2, temp);
          	break;
          }
          case AST_FUNC_T:
          {
          	SYMBOL_TABLE_ELEM *func_temp=get_sym_tbl_malloc(NULL);
          	func_temp->prev=rtn_val;
          	func_temp->elem_type=SYMB_TBL_FUNC;
          	rtn_val->references++;
          	func_temp->values.func_values.function=(void*)temp->func_call;
          	rtn_val=func_temp;
          	generate_symbol_table(temp->func_statement->code_block, rtn_val, 1,temp);
          	break;
          }
          case AST_FUNC_CALL_T:
          {
          	get_symb_tbl_func_call(rtn_val, temp->func_call,temp->line_number);
          	break;
          }
          case AST_RETURN_T:
          {
          	find_expression_symbols(rtn_val,temp->return_statement->expression,temp->line_number);
          	break;
          }
          case AST_CODE_BLOCK_TYPE:
          {
          	generate_symbol_table(temp->code_block, rtn_val, 0, NULL);
            break;
          }
       }
                temp=temp->next;
	}

	if(mode==2)
	{
		if(passed->statement_type!=AST_FOR_T)
		{
			if(!rtn_val->references)
				death_start(rtn_val);
			death_lever();
		}
		if(!passed->for_statement->implict_expressions)
			goto end;
		AST_STATEMENT *for_temp_implict=passed->for_statement->implict_expressions->statement;
		while(for_temp_implict)
		{
			switch(for_temp_implict->statement_type)
			{
				case AST_ASSIGN_T:
				{
					get_symb_tbl_assign(rtn_val, for_temp_implict);
					break;
				}
				default:
					printf("sematic error:unexpected statement in for implict\n");
					break;
			}
		}

	}
end:
	ast_tree->last=rtn_val;
	if(!rtn_val->references)
		add_death_map(rtn_val);
	//add to death map
	return rtn_val;

}