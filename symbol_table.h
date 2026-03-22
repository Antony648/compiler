#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdbool.h>
struct symbol_table_elem;
typedef struct symbol_table_elem SYMBOL_TABLE_ELEM;
struct death_map_elem;
typedef struct death_map_elem DEATH_MAP_ELEM;
struct death_map_elem
{
	SYMBOL_TABLE_ELEM* pointer;
	DEATH_MAP_ELEM* prev;
};
typedef enum 
{
	AST_DATA_TYPES_NULL,
	AST_INT_T,
	//can add other data types here
}AST_DATA_TYPES;

typedef enum{
	SYMB_TBL_NULL,
	SYMB_TBL_START,
	SYMB_TBL_IDEN,
	SYMB_TBL_FUNC,
}SYMBOL_TABLE_ELEM_T;



struct symbol_table_elem
{
	SYMBOL_TABLE_ELEM_T elem_type;
	union v{
		struct s1
		{
			void *address;
			AST_DATA_TYPES data_type;
			char* identifier;
		}iden_values;
		struct s2
		{
			void *address;
			void *function; //should be used to point the function node in AST 
			//because it contains everything we need.
		}func_values;
	}values;
	int references;
	SYMBOL_TABLE_ELEM* prev;
};


#endif