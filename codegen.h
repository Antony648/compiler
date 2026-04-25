#ifndef CODEGEN_H
#define CODEGEN_H
#define DIGIT_SIZE 10
#include <stdbool.h>
#include "parser.h"
#include "symbol_table.h"
bool generate_code(const char*,SYMBOL_TABLE_ELEM*,AST_CODE_BLOCK*);
void generate_code_codeblock(AST_CODE_BLOCK* code_block,int fd,int add_val,int* function_context,int* var_size);
#endif