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

AST_EXPR* get_expression(token_t end){}
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
	temp->expression=get_expression(TOKEN_SEMICOLON);
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
	temp->expresssion=get_expression(TOKEN_SEMICOLON);
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
	if(temp->identifier)
		destroy_identifier(temp->identifier);
	if(temp->expresssion)
		destroy_expression(temp->expresssion);
	free(temp);
	temp=NULL;
	return temp;
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
	temp->test_case_expression=get_expression(TOKEN_RPAR);
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
	temp->test_case_expression=get_expression(TOKEN_RPAR);
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