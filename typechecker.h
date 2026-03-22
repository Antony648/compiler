#ifndef TYPECHECKER_H
#define TYPECHECKER_H
#include "parser.h"
#include "symbol_table.h"
SYMBOL_TABLE_ELEM* generate_symbol_table(AST_CODE_BLOCK*,SYMBOL_TABLE_ELEM* ,int,AST_STATEMENT*);

#endif