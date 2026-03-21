#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include <stdbool.h>
struct symbol_table_elem;
typedef struct symbol_table_elem SYMBOL_TABLE_ELEM;
struct symbol_table_elem
{
	union{
		struct 
		{
			void *address;
			int references;
			char* identifier;
		}values;
		bool is_start;
	};
	SYMBOL_TABLE_ELEM* prev;
};
#endif