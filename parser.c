#include "parser.h"
#include "lexer.h"
#include <cstdio>
#include <errno.h>
#include <stdio.h>
int token_train_offset=-1;
int parser_pov_lc=1;
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
AST_STATEMENT* get_statement()
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
	
}
AST_CODE_BLOCK* get_code_block()
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
	prgm->statement=get_statement();
	temp=prgm->statement;
	if(!temp)
	{
		printf("error:no content in input file\n");
		destroy_code_block(prgm);
		exit(1);
	}
	while(temp)
	{
		temp->next=get_statement();
		temp=temp->next;
	}

}