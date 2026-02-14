#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include <stdio.h>
struct ast_expression;
typedef struct ast_expression AST_EXPR;
struct ast_statement;
typedef struct ast_statement AST_STATEMENT;
typedef struct ast_code_block AST_CODE_BLOCK;
struct ast_func_params;
typedef struct ast_func_params AST_FUNC_PARAMS;
typedef enum 
{
	AST_NULL_BIN_OPS_T,
	AST_ADD_T,
	AST_SUB_T,
	AST_MUL_T,
	AST_DIV_T,
	AST_EQ_T,
	AST_NEQ_T,
}binary_ops;
typedef enum
{
	AST_NULL_T,
	AST_DEC_T,
	AST_INIT_T,
	AST_ASSIGN_T,
	AST_IF_CASE_T,
	AST_WHILE_CASE_T,
	AST_FOR_T,
	AST_FUNC_T,
	AST_NEW_LINE_T,
}AST_T;

typedef enum 
{
	AST_DATA_TYPES_NULL,
	AST_INT_T,
	//can add other data types here
}AST_DATA_TYPES;

typedef struct 
{
	AST_DATA_TYPES data_type;
	char* iden;
	//should add enty in ths symbol table
}AST_IDEN;
typedef enum 
{
	AST_NULL_EXPR_T,
	AST_IDEN_T,
	AST_VAL_T,
	AST_BIN_EXPR_T,
}AST_EXPR_T;


struct ast_expression
{
	AST_EXPR_T ast_exp_type;
	union
	{
		AST_IDEN identifier;
		int value;
		struct
		{
			binary_ops bin_ops;
			AST_EXPR *expr_l;
			AST_EXPR *expr_r;
		}expression;
	};	
};

typedef struct 
{
	AST_IDEN *identifier;
	AST_EXPR *expression;
}AST_INIT;

typedef struct 
{
	AST_IDEN *identifier;
}AST_DEC;
typedef struct 
{
	AST_IDEN *identifier;
	AST_EXPR *expresssion;
}AST_ASSIGN;
typedef struct 
{
	AST_EXPR *test_case_expression;
	AST_CODE_BLOCK *code_block;
}AST_IF_CASE;
typedef struct 
{
	AST_EXPR *test_case_expression;
	AST_CODE_BLOCK *code_block;
}AST_WHILE_CASE;
typedef struct
{
	AST_CODE_BLOCK *init_expressions;	//can have multiple independent expressions
	AST_EXPR *test_case_expression;
	AST_CODE_BLOCK *implict_expressions;
	AST_CODE_BLOCK *code_block;
}AST_FOR_CASE;
struct ast_func_params
{
	AST_IDEN identifier;
	AST_FUNC_PARAMS* next;
};
typedef struct 
{
	AST_DATA_TYPES return_data_type;
	AST_FUNC_PARAMS *paramters_list;
	int parameter_count;
	AST_IDEN* identifier;
	AST_CODE_BLOCK *code_block;
}AST_FUNC;
struct ast_statement
{
	AST_T statement_type;
	union
	{
		AST_FOR_CASE *for_statement;
		AST_WHILE_CASE *while_statement;
		AST_IF_CASE *if_statement;
		AST_FUNC *func_statement;
		AST_ASSIGN *assign_statement;
		AST_DEC *dec_statement;
		AST_INIT *init_statement;
	};
	AST_STATEMENT* next;
};
typedef enum 
{
	AST_CODE_BLOCK_NULL,
	AST_IF_CODE_BLOCK,
	AST_FOR_CODE_BLOCK,
	AST_WHILE_CODE_BLOCK,
	AST_FUNC_CODE_BLOCK,
	AST_PROGRAM,
}AST_CODE_BLOCK_T;

struct ast_code_block
{
	AST_CODE_BLOCK_T code_block_type;
	AST_STATEMENT *statement;
};
AST_CODE_BLOCK* get_code_block(token_t );

#endif