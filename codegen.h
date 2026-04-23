#ifndef CODEGEN_H
#define CODEGEN_H
#define DIGIT_SIZE 10
#include <stdbool.h>
#include "parser.h"
#include "symbol_table.h"
bool generate_code(const char*,SYMBOL_TABLE_ELEM*,AST_CODE_BLOCK*);
#endif