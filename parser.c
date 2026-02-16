#include "parser.h"
#include "lexer.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
void destroyed_code_block(AST_CODE_BLOCK* code_block);

int token_train_offset=0;
int parser_pov_lc=1;
int seek_next()
{
	int rtn_val=token_train_offset+1;
	while(rtn_val<token_count && token_train[rtn_val].token_type==TOKEN_NEW_LINE)
		rtn_val+=1;
	return rtn_val<token_count?rtn_val:token_count-1;
}
int seek_mulitple(int val)
{
	int rtn_val=token_train_offset+val;
	while(rtn_val<token_count && token_train[rtn_val].token_type==TOKEN_NEW_LINE)
		rtn_val+=1;
	return rtn_val<token_count?rtn_val:token_count-1;
}
int move_next()
{
	token_train_offset+=token_train_offset;
	while(token_train_offset <token_count && token_train[token_train_offset].token_type==TOKEN_NEW_LINE)
	{
		token_train_offset+=1;
		parser_pov_lc+=1;
	}
	return token_train_offset<token_count?token_train_offset:token_count-1;
}
void destroy_expression(AST_EXPR *temp)
{

}
void destroy_identifier(AST_IDEN *temp)
{
	if(!temp)
		return;
	if(temp->iden)
	{
		free(temp->iden);
		temp->iden=NULL;
	}
	free(temp);
	temp=NULL;
	return;

}
void destroy_statement(AST_STATEMENT* statement)
{
/*
AST_DEC_T,
	AST_INIT_T,
	AST_ASSIGN_T,
	AST_IF_CASE_T,
	AST_WHILE_CASE_T,
	AST_FOR_T,
	AST_FUNC_T,
	AST_NEW_LINE_T,*/	

}
void destroy_statement_loop(AST_STATEMENT* statement)
{
	AST_STATEMENT *temp=NULL;
	while(statement)
	{
		temp=statement;
		statement=statement->next;
		destroy_statement(temp);
		temp=NULL;
	}
}
void destroy_code_block(AST_CODE_BLOCK* code_block)
{
	if(!code_block)
		return;
	if(code_block->statement)
		destroy_statement_loop(code_block->statement);
	free(code_block);
	code_block=NULL;
	return;
}

AST_EXPR* get_expression(int mode)
{
	//mode 1 :semicolon or normal
	//mode 2: for , or ; for_init
	//mode 3: for , or ) for_implict
	//mode 4: rpar for while test and if test
}
AST_FUNC* get_function(){}
AST_INIT* get_init()
{
	AST_DATA_TYPES temp_dt=0;
	switch(token_train[token_train_offset].token_type)
	{
	case TOKEN_INT:
		temp_dt=AST_INT_T;
		break;
	//add other data types later
	default:
		printf("syntax error:line %d: unknown data type in initialization\n",parser_pov_lc);
		return NULL;

	}
	move_next();//datatye consume
	if(token_train[token_train_offset].token_type!=TOKEN_ID)
	{
		printf("syntax error:line %d:no identifer in initialization\n",parser_pov_lc );
		return NULL;
	}
	AST_INIT* temp=malloc(sizeof(AST_INIT));
	if(!temp)
	{
		printf("malloc failure \n");
		return NULL;
	}
	memset(temp,0,sizeof(AST_INIT));
	temp->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->identifier)
	{
		printf("malloc failure");
		goto error_end;
	}
	memset(temp->identifier,0,sizeof(AST_IDEN));
	char* buffer=NULL;
	int len=strlen(token_train[token_train_offset].id_str);
	buffer=malloc(len+1);
	if(!buffer)
	{
		printf("malloc failure");
		goto error_end;
	}
	strncpy(buffer,token_train[token_train_offset].id_str,len);
	buffer[len]='\0';
	temp->identifier->iden=buffer;
	temp->identifier->data_type=temp_dt;
	move_next();//identifer consume
	if(token_train[token_train_offset].token_type!=TOKEN_ASSIGN)
	{
		printf("syntax error:line %d:no assignment statemnt\n",parser_pov_lc );
		goto error_end;
	}
	move_next();//assignmnt consume
	temp->expression=(TOKEN_SEMICOLON);
	if(!temp->expression)
	{
		printf("failed in getting expression\n");	//can commet out later
		goto error_end;
	}
	move_next();//expression consume
	if(token_train[token_train_offset].token_type!=TOKEN_SEMICOLON)
	{
		printf("syntax error:line %d: no terminating semicolon\n",parser_pov_lc);
		goto error_end;
	}
	move_next();//semicolon consume
	return temp;
error_end:
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp->expression)
		destroy_expression(temp->expression);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}
void destroy_assignment(AST_ASSIGN* temp)
{
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp->expresssion)
		destroy_expression(temp->expresssion);
	free(temp);
	temp=NULL;
}
AST_ASSIGN* get_assignment()
{
	if(token_train[token_train_offset].token_type!=TOKEN_ID)
	{
		printf("syntax error:line %d: non identifier start for assignment\n",parser_pov_lc);
		return NULL;
	}
	AST_ASSIGN *temp=malloc(sizeof(AST_ASSIGN));
	if(!temp)
	{
		printf("malloc failure");
		return NULL;
	}
	memset(temp,0,sizeof(AST_ASSIGN));
	temp->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->identifier)
	{
		printf("malloc failure");
		goto error_end;
	}
	memset(temp->identifier,0,sizeof(AST_IDEN));
	char*buffer=NULL;
	int len=strlen(token_train[token_train_offset].id_str);
	buffer=malloc(len+1);
	if(!buffer)
	{
		printf("malloc failure");
		goto error_end;
	}
	strncpy(buffer,token_train[token_train_offset].id_str,len);
	buffer[len]='\0';
	temp->identifier->iden=buffer;
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_ASSIGN)
	{
		printf("syntax error:line %d:no assignment operator\n",parser_pov_lc);
		goto error_end;
	}
	move_next();
	temp->expresssion=get_expression(1);
	if(!temp->expresssion)
	{
		printf("error in getting expression"); //can comment out later will get suitable error statement from get_expression
		goto error_end;		
	}
	move_next();
	if(token_train[token_train_offset].token_type==TOKEN_SEMICOLON)
	{
		printf("syntax error:line %d:no ending semicolon\n",parser_pov_lc);
		goto error_end;
	}
	move_next();
	return temp;
error_end:
	destroy_assignment(temp);
	return NULL;
}
AST_IF_CASE* get_if(){
	if(token_train[token_train_offset].token_type!=TOKEN_IF)
	{
		printf("syntax error:line %d",parser_pov_lc);
		return NULL;
	}
	move_next();	//if consume
	if(token_train[token_train_offset].token_type!=TOKEN_LPAR)
	{
		printf("syntax error:line %d:if keyword not followed by paranthesis\n",parser_pov_lc);
		return NULL;
	}
	move_next();	//lpar
	AST_IF_CASE *temp=malloc(sizeof(AST_IF_CASE));
	if(!temp)
	{
		printf("malloc failure");
		return NULL;
	}
	temp->test_case_expression=get_expression(4);
	if(!temp->test_case_expression)
	{
		printf("failure in getting expression\n");
		goto error_end;
	}
	move_next(); 	//expression
	if(token_train[token_train_offset].token_type!=TOKEN_RPAR)
	{
		printf("syntax error:line %d:no right paranthesis after condition\n",parser_pov_lc);
		goto error_end;
	}
	move_next();	//rpar
	if(token_train[token_train_offset].token_type!=TOKEN_LBRACE)
	{
		printf("syntax error:line %d:no right paranthesis\n",parser_pov_lc);
		goto error_end;
	}
	move_next();	//lbrace
	temp->code_block=get_code_block(TOKEN_LPAR);
	if(!temp->code_block)
	{
		printf("error in getting code block");
		goto error_end;
	}
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_RBRACE)
	{
		printf("syntax error:line %d:error in finding end of code block\n",parser_pov_lc);
		goto error_end;
	}
	move_next();	//rbrace
	return temp;
error_end:
	if(temp->test_case_expression)
		destroy_expression(temp->test_case_expression);
	if(temp->code_block)
		destroy_code_block(temp->code_block);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}

AST_WHILE_CASE* get_while(){
	if(token_train[token_train_offset].token_type!=TOKEN_WHILE)
	{
		printf("syntax error:line %d: ",parser_pov_lc);
		return NULL;
	}
	move_next();	//consume while
	if(token_train[token_train_offset].token_type !=TOKEN_LPAR)
	{
		printf("syntax error:line %d: while not followed by lpar\n",parser_pov_lc);
		return NULL;
	}
	AST_WHILE_CASE* temp=malloc(sizeof(AST_WHILE_CASE));
	if(!temp)
	{
		printf("malloc faliure\n");
		return NULL;
	}
	memset(temp,0,sizeof(AST_WHILE_CASE));
	move_next();	//consume lpar
	temp->test_case_expression=get_expression(4);
	if(!temp)
	{
		printf("failed in getting test expresion");
		goto error_end;
	}
	move_next();	//consume expression
	if(token_train[token_train_offset].token_type !=TOKEN_RPAR)
	{
		printf("syntax error:line %d:no rpar after expression in while\n",parser_pov_lc);
		goto error_end;
	}
	move_next();	//consume rpar
	if(token_train[token_train_offset].token_type !=TOKEN_LBRACE)
	{
		printf("syntax error:line %d:no lbarce after condition speicification in while\n",parser_pov_lc);
		goto error_end;
	}
	move_next();	//consume lbrace
	temp->code_block=get_code_block(TOKEN_RBRACE);
	move_next();	//consume block
	if(token_train[token_train_offset].token_type!=TOKEN_RBRACE)
	{
		printf("syntax error:line %d:no rbrace after while body",parser_pov_lc);
		goto error_end;
	}
	move_next();	//consume rbace
	return temp;
error_end:
	if(temp->test_case_expression)
		destroy_expression(temp->test_case_expression);
	if(temp->code_block)
		destroy_code_block(temp->code_block);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}
AST_STATEMENT* get_for_implict_assign()
{
	if(token_train[token_train_offset].token_type!=TOKEN_ID)
	{
		printf("syntax error:line %d:assignment statements in implict assign in for should start with identifier\n",parser_pov_lc);
		return NULL;
	}
	AST_STATEMENT* temp=malloc(sizeof(AST_STATEMENT));
	if(!temp)
	{
		printf("failure in malloc");
		return NULL;
	}
	temp->statement_type=AST_ASSIGN_T;
	temp->assign_statement=malloc(sizeof(AST_ASSIGN));
	if(!temp->assign_statement)
	{
		printf("malloc failure");
		goto error_end;
	}
	memset(temp->assign_statement,0,sizeof(AST_ASSIGN));
	temp->assign_statement->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->assign_statement->identifier)
	{
		printf("malloc failure");
		goto error_end;
	}
	memset(temp->assign_statement->identifier,0,sizeof(AST_ASSIGN));
	int len=strlen(token_train[token_train_offset].id_str);
	temp->assign_statement->identifier->iden=malloc(len+1);
	if(!temp->assign_statement->identifier->iden)
	{
		printf("malloc failure");
		goto error_end;
	}
	strncpy(temp->assign_statement->identifier->iden,token_train[token_train_offset].id_str,len);
	temp->assign_statement->identifier->iden[len]='\0';
	//data type is unknown now shouldset in semantic phase
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_ASSIGN)
	{
		printf("syntax error:line %d:no assignment operator after identifier in for implict",parser_pov_lc);
		goto error_end;
	}
	move_next();
	temp->assign_statement->expresssion=get_expression(3);	//should stop at LPAR  also
	if(!temp->assign_statement->expresssion)
	{
		printf("syntax error:line %d:assignment operator not followd by proper expression for implict",parser_pov_lc);
		goto error_end;
	}
	switch(token_train[seek_next()].token_type)
	{
	case TOKEN_LPAR:
		{
			return temp;
		}
	case TOKEN_COMMA:
		{
			move_next();
			return temp;
		}
	default:
		{
			printf("syntax error:line %d:assignment statement does not end properly in for implict",parser_pov_lc);
			goto error_end;
		}
	}
	return temp;
error_end:
	if(temp->assign_statement)
		destroy_assignment(temp->assign_statement);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;

}
AST_CODE_BLOCK* get_for_implict()
{
	AST_CODE_BLOCK* first=malloc(sizeof(AST_CODE_BLOCK));
	if(!first)
	{
		printf("malloc failure");
		return NULL;
	}
	first->statement=get_for_implict_assign();
	AST_STATEMENT* temp=first->statement;
	if(!temp)
	{
		printf("failure in getting expression");
		goto error_end;
	}
	while(token_train[seek_next()].token_type!=TOKEN_RPAR)
	{
		temp->next=get_for_implict_assign();
		if(!temp->next)
		{
			printf("failure in getting expression");
			goto error_end;
		}
		temp=temp->next;
	}
	return first;
error_end:
	if(first)
	{
		destroy_code_block(first);
		first=NULL;
	}
	return first;
}
AST_DEC* get_for_init_dec(token_t tok_t)
{
	AST_DEC *temp=malloc(sizeof(AST_DEC));
	if(!temp)
	{
		printf("malloc faliure");
		return NULL;
	}
	memset(temp,0,sizeof(AST_DEC));
	temp->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->identifier)
	{
		printf("faliure in malloc");
		goto error_end;
	}
	switch(tok_t)
	{
		case TOKEN_INT:
			temp->identifier->data_type=AST_INT_T;
			break;
		//add your data types
		default:
			goto error_end;
	}
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_ID)
	{
		printf("syntax error:line %d:no identifier in decclaration in for init\n",parser_pov_lc );
		goto error_end;
	}
	int len=strlen(token_train[token_train_offset].id_str);
	temp->identifier->iden=malloc(len+1);
	if(!temp->identifier->iden)
	{
		printf("malloc failure");
		goto error_end;
	}
	strncpy(temp->identifier->iden,token_train[token_train_offset].id_str,len);
	temp->identifier->iden[len]='\0';
	
	if(token_train[seek_next()].token_type==TOKEN_SEMICOLON)
	{
		return  temp;
	}	
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_COMMA)
	{
		printf("syntax error:line %d:no comma separation after expresssion\n",parser_pov_lc);
		goto error_end;
	}
	return temp;

error_end:
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}
AST_INIT* get_for_init_init(token_t tok_t)
{
	AST_INIT *temp=malloc(sizeof(AST_INIT));
	if(!temp)
	{
		printf("malloc faliure");
		return NULL;
	}
	memset(temp,0,sizeof(AST_INIT));
	temp->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->identifier)
	{
		printf("failure in malloc");
		goto error_end;
	}
	memset(temp->identifier,0,sizeof(AST_IDEN));
	temp->expression=malloc(sizeof(AST_EXPR));
	if(!temp->expression)
	{
		printf("faliure in malloc");
		goto error_end;
	}
	memset(temp->expression,0,sizeof(AST_EXPR));
	switch(tok_t)
	{
		case TOKEN_INT:
				temp->identifier->data_type=AST_INT_T;
				break;
		//add other data types
		default:
				goto error_end;
	}	
	move_next();
	if(token_train[token_train_offset].token_type !=TOKEN_ID)
	{
		printf("syntax error:line %d:no identifier after datatype",parser_pov_lc);
		goto error_end;
	}
	int len=strlen(token_train[token_train_offset].id_str);
	temp->identifier->iden=malloc(len+1);
	if(!temp->identifier->iden)
	{
		printf("malloc failure");
		goto error_end;
	}
	strncpy(temp->identifier->iden,token_train[token_train_offset].id_str,len);
	temp->identifier->iden[len]='\0';
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_ASSIGN)
	{
		printf("syntax error:line %d:no assignment operator\n",parser_pov_lc );
		goto error_end;
	}
	move_next();
	temp->expression=get_expression(2); //should stop for also semicolon
	if(!temp->expression)
	{
		printf("syntax error:line %d:error in getting expression \n",parser_pov_lc);
		goto error_end;
	}
	if(token_train[seek_next()].token_type==TOKEN_SEMICOLON)
	{
		return  temp;
	}	
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_COMMA)
	{
		printf("syntax error:line %d:no comma separation after expresssion\n",parser_pov_lc);
		goto error_end;
	}
	return temp;
error_end:
	if(temp->expression)
		destroy_expression(temp->expression);
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}
AST_STATEMENT* get_for_statement_init()
{
	token_t tok_t=token_train[token_train_offset].token_type;
	AST_STATEMENT* temp=malloc(sizeof(AST_STATEMENT));
	if(!temp)
	{
		printf("malloc failure");
		return NULL;
	}
	memset(temp,0,sizeof(AST_STATEMENT));
	//move_next();
	switch(token_train[seek_mulitple(2)].token_type)
	{
		case TOKEN_COMMA:
			temp->statement_type=AST_DEC_T;
			temp->dec_statement=get_for_init_dec(tok_t);
			if(!temp->dec_statement)
				goto error_end;
			break;
		case TOKEN_ASSIGN:
			temp->statement_type=AST_INIT_T;
			temp->init_statement=get_for_init_init(tok_t);
			if(!temp->assign_statement)
				goto error_end;
			break;
		default:
			printf("syntax error:line %d:non initialization or decclaration in for init part\n",parser_pov_lc);
			goto error_end;
	}
	return  temp;
error_end:
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;
}
AST_CODE_BLOCK* get_for_init()
{
	//this is to generate a code_block for the statements in the
	//init part of the for; it can only accept decclaration
	//or initalization and also will be in same scope as the loop
	AST_CODE_BLOCK* first=malloc(sizeof(AST_CODE_BLOCK));
	if(!first)
	{
		printf("error in malloc");
		return NULL;
	}
	memset(first,0,sizeof(AST_CODE_BLOCK));
	//AST_CODE_BLOCK *temp=first;
	first->statement=get_for_statement_init();
	AST_STATEMENT *temp=first->statement;
	if(!temp)
	{
		printf("error in get_statement\n");
		goto error_end;
	}
	while(token_train[seek_next()].token_type!=TOKEN_SEMICOLON && token_train[seek_next()].token_type!=TOKEN_EOF)
	{
		temp->next=get_for_statement_init();
		if(!temp->next)
		{
			printf("error in get_statement\n");
			goto error_end;
		}
		temp=temp->next;
	}
	move_next();
	if(token_train[token_train_offset].token_type !=TOKEN_SEMICOLON)
	{
		printf("syntax error:line %d:no semi colon after init",parser_pov_lc);
		goto error_end;
	}
	return first;
error_end:
	if(first)
	{
		destroy_code_block(first);
		first=NULL;
	}
	return first;
}

AST_FOR_CASE* get_for(){
	if(token_train[token_train_offset].token_type !=TOKEN_FOR)
	{
		printf("syntax error ");
		return NULL;
	}
	move_next();
	if(token_train[token_train_offset].token_type !=TOKEN_LPAR)
	{
		printf("syntax error:line %d:for not followed by lpar",parser_pov_lc);
		return NULL;
	}
	move_next();
	AST_FOR_CASE *temp=malloc(sizeof(AST_FOR_CASE));
	if(!temp)
	{
		printf("malloc failure");
		goto error_end;
	}
	memset(temp,0,sizeof(AST_FOR_CASE));
	temp->init_expressions=get_for_init();
	if(!temp->init_expressions)
	{
		printf("faliure in getting init code block\n");
		goto error_end;
	}
	temp->test_case_expression=get_expression(1);
	if(!temp->test_case_expression)
	{
		printf("failure in getting expression");
		goto error_end;
	}
	move_next();
	if(token_train[token_train_offset].token_type!=TOKEN_SEMICOLON)
	{
		printf("syntax error:line %d:error condition expression in if terminated without semicolon",parser_pov_lc);
		goto error_end;
	}
	
error_end:
	

}
AST_DEC* get_decclaration()
{

	AST_DEC *temp=malloc(sizeof(AST_DEC));
	if(!temp)
	{
		printf("malloc failure\n");
		return NULL;
	}
	memset(temp,0,sizeof(AST_DEC));
	temp->identifier=malloc(sizeof(AST_IDEN));
	if(!temp->identifier)
	{
		printf("malloc failure\n");
		goto error_end;
	}
	memset(temp->identifier,0,sizeof(AST_IDEN));
	char *buffer=NULL;
	int len;
	switch(token_train[token_train_offset].token_type)
	{
		case TOKEN_INT:
		{
			temp->identifier->data_type=AST_INT_T;
			move_next();
			if(token_train[token_train_offset].token_type!=TOKEN_ID)
			{
				printf("syntax error:line %d:data type followed by non identifer in get_decclaration\n",parser_pov_lc );
				goto error_end;
			}
			len=strlen(token_train[token_train_offset].id_str);
			buffer=malloc(len+1);
			if(!buffer)
			{
				printf("malloc failure");
				goto error_end;
			}
			strncpy(buffer,token_train[token_train_offset].id_str,len);
			buffer[len]='\0';
			temp->identifier->iden=buffer;

			move_next();
			if(token_train[token_train_offset].token_type!=TOKEN_SEMICOLON)
			{
				printf("syntax error:line %d:decclaration with no semicolon\n",parser_pov_lc );
				goto error_end;
			}
		}
		//can add other data types here
		default:
			printf("syntax error:line %d:unrecognized data type for decclaration",parser_pov_lc);
			goto error_end;
	}
	return  temp;

error_end:
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp)
	{
		free(temp);
		temp=NULL;
	}
	return temp;

}
int  get_statement(AST_STATEMENT* statement)
{
/*
AST_DEC_T,
	AST_INIT_T,
	AST_ASSIGN_T,
	AST_IF_CASE_T,
	AST_WHILE_CASE_T,
	AST_FOR_T,
	AST_FUNC_T,
	AST_NEW_LINE_T,*/
	AST_STATEMENT* temp=malloc(sizeof(AST_STATEMENT));
	if(!temp)
	{
		printf("malloc failure");
		return NULL;
	}
	memset(temp,0,sizeof(AST_STATEMENT));
	temp->line_number=parser_pov_lc;
	switch(token_train[token_train_offset].token_type)
	{
		case TOKEN_INT:
		{
			//could be init dec or func
			token_t type=token_train[seek_mulitple(2)].token_type;
			switch(type)
			{
				case TOKEN_ASSIGN:
				{
					temp->statement_type=AST_INIT_T;
					temp->init_statement=get_init();
					if(!temp->init_statement)
						goto error_end;
					break;
				}
				case TOKEN_SEMICOLON:
				{
					temp->statement_type=AST_DEC_T;
					temp->dec_statement=get_decclaration();
					if(!temp->dec_statement)
						goto error_end;
					break;
				}
				case TOKEN_LPAR:
				{
					temp->statement_type=AST_FUNC_T;
					temp->func_statement=get_function();
					if(!temp->func_statement)
						goto error_end;
					break;
				}
				default:
					goto error_end;
			}
			
		}
		case TOKEN_ID:
		{
			//assignment
			temp->statement_type=AST_ASSIGN_T;
			temp->assign_statement=get_assignment();
			if(!temp->assign_statement)
				goto error_end;
			break;
		}
		case TOKEN_IF:
		{
			temp->statement_type=AST_IF_CASE_T;
			temp->if_statement=get_if();
			if(!temp->if_statement)
				goto error_end;
			break;
		}
		case TOKEN_FOR:
		{
			temp->statement_type=AST_FOR_T;
			temp->for_statement=get_for();
			if(!temp->for_statement)
				goto error_end;
			break;
		}
		case TOKEN_WHILE:
		{
			temp->statement_type=AST_WHILE_CASE_T;
			temp->while_statement=get_while();
			if(!temp->while_statement)
				goto error_end;
			break;
		}
		default:
			goto error_end;
	}

	statement=temp;
	return 0;
error_end:
	printf("syntax error in line %d\n",parser_pov_lc);
	if(temp)
		free(temp);
	statement=NULL;
	return -1;

}
void destroyed_code_block(AST_CODE_BLOCK* code_block)
{

}
AST_CODE_BLOCK* get_code_block(token_t end)
{
	AST_CODE_BLOCK* prgm=malloc(sizeof(AST_CODE_BLOCK));
	AST_STATEMENT* temp=NULL;
	if(!prgm)
	{
		printf("malloc failure");
		destroy_code_block(prgm);
		exit(1);
	}
	memset(prgm,0,sizeof(AST_CODE_BLOCK));
	prgm->code_block_type=AST_PROGRAM;
	if(token_train[seek_next()].token_type==end || token_train[seek_next()].token_type==TOKEN_EOF)
	{
		return prgm;
	}
	if(get_statement(prgm->statement)<0)
	{
		printf("error:no content in input file\n");
		destroy_code_block(prgm);
		exit(1);
	}	
	temp=prgm->statement;
	int rtn_val=0;
	while(temp)
	{
		if(token_train[seek_next()].token_type==end || token_train[seek_next()].token_type==TOKEN_EOF)
		{
			return prgm;
		}
		rtn_val=get_statement(temp->next);
		switch(rtn_val)
		{
			case 0:	
				break;
			case -1:
			{
				printf("error in getting statement\n");
				destroy_code_block(prgm);
				exit(1);
			}
		}
		temp=temp->next;
	}

}