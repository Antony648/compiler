#include "lexer.h"
#include <fcntl.h>
#include <stdio.h>

int main(int argc,char* argv[])
{
	char *file_name;
	if(argc<2)
	{
		file_name="output.asm";
	}
	else
		file_name=argv[1];
	FILE* file_ptr=NULL;
	file_ptr=fopen("input.txt","r");
	if(!file_ptr)
	{
		printf("failed in opening file\n");
		return -1;
	}
	init_token_train();
	populate_token_train(file_ptr);

	destroy_token_train();
	fclose(file_ptr);
	
}