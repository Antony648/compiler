#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
typedef enum 
{
	TOKEN_NULL,
	TOKEN_INT,
	TOKEN_ID,
	TOKEN_INT_VAL,
	TOKEN_IF,
	TOKEN_WHILE,
	TOKEN_ASSIGN,
	TOKEN_FOR,
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LPAR,
	TOKEN_RPAR,
	TOKEN_ADD,
	TOKEN_SUB,
	TOKEN_MUL,
	TOKEN_DIV,
	TOKEN_EQUAL,
	TOKEN_NEQUAL,
	TOKEN_SEMICOLON,
	TOKEN_NEW_LINE,
	TOKEN_RETURN,
	TOKEN_LESS_THAN,
	TOKEN_GREATER_THAN,
	TOKEN_LESS_THAN_EQUAL,
	TOKEN_GREATER_THAN_EQUAL,
	TOKEN_EOF,
}token_t;
typedef struct
{
	token_t token_type;
	union
	{
		int value;char* id_str;
	};
}token_obj;
extern int token_count;
extern token_obj* token_train;
#define MAX_TOKEN_LEN 256
void init_token_train();
void destroy_token_train();
void populate_token_train(FILE* filep);
#endif