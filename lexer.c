#include "lexer.h"


token_obj * token_train=NULL;
int token_count=0;
int line_number=1;
int token_cur_max =256;
void init_token_train()
{
	token_train=malloc(sizeof(token_obj)*token_cur_max);
	if(!token_train)
	{
		printf("malloc failure");
		exit(1);
	}
	memset(token_train,0,sizeof(token_obj)*token_cur_max);
	return;
}
int add_next_token(FILE *file_ptr)
{
	if(token_count >=token_cur_max)
	{
		token_obj *temp; token_cur_max*=2;
		temp=realloc(token_train,sizeof(token_obj)*token_count);
		if(!temp)
		{
			printf("malloc failure\n");
			destroy_token_train();
			exit(1);
		}
		token_train=temp;
	}
	int ch; char* buffer=NULL;
	while(1)
	{
		ch=fgetc(file_ptr);
		if(ch==EOF)
			break;
		if(ch == '\n')
		{
			token_train[token_count].token_type=TOKEN_NEW_LINE;
			token_count++;
			line_number++;
			return 0;
		}
		if(isspace(ch))
		{
			continue;
		}
		if(isalpha(ch))
		{
			token_train[token_count].token_type=TOKEN_ID;
			buffer=(char* )malloc(MAX_TOKEN_LEN+1);
			if(!buffer)
			{
				printf("malloc failure\n");
				destroy_token_train();
				exit(1);
			}
			memset(buffer,0,MAX_TOKEN_LEN+1);
			char *temp=buffer;
			*temp=(char)ch;temp++;
			while((ch=fgetc(file_ptr))!=EOF)
			{
				if(isalnum(ch) || ch == '_')
				{
					*temp=(char)ch;temp++;
				}
				else
				{
					ungetc(ch,file_ptr);
					break;
				}
			}
			if(!strcmp(buffer,"int"))	
				token_train[token_count].token_type=TOKEN_INT;
			else if(!strcmp(buffer,"while"))
				token_train[token_count].token_type=TOKEN_WHILE;
			else if(!strcmp(buffer,"if"))
				token_train[token_count].token_type=TOKEN_IF;
			else if(!strcmp(buffer,"for"))
				token_train[token_count].token_type=TOKEN_FOR;
			else if(!strcmp(buffer,"return"))
				token_train[token_count].token_type=TOKEN_RETURN;
			else
			{
				int len=strlen(buffer);
				token_train[token_count].token_type=TOKEN_ID;
				token_train[token_count].id_str=malloc(len+1);
				if(!token_train[token_count].id_str)
				{
					printf("malloc failure");
					destroy_token_train();
					exit(1);
				}
				strncpy(token_train[token_count].id_str,buffer,len);
				token_train[token_count].id_str[len]='\0';
			}
			token_count++;
			if(buffer)
			{
				free(buffer);
				buffer=NULL;
			}
			return 0;
		}
		if(isdigit(ch))
		{
			token_train[token_count].token_type=TOKEN_INT_VAL;
			buffer=(char*)malloc(MAX_TOKEN_LEN+1);
			if(!buffer)
			{
				printf("malloc failure");
				destroy_token_train();
				exit(1);
			}
			memset(buffer,0,MAX_TOKEN_LEN+1);
			char *temp=buffer;
			*temp=(char)ch;
			temp++;
			while((ch=fgetc(file_ptr))!=EOF)
			{
				if(isdigit(ch))
				{
					*temp=(char)ch;temp++;
				}
				else
				{
					ungetc(ch,file_ptr);
					break;
				}
			}
			token_train[token_count].value=atoi(buffer);
			token_count++;
			if(buffer)
			{
				free(buffer);
				buffer=NULL;
			}
			return 0;
		}
		if(ch== '=')
		{
			ch=fgetc(file_ptr);
			if(ch==(int) '=')
			{
				token_train[token_count].token_type=TOKEN_EQUAL;
				token_count++;
				return 0;
			}
			else
			{
				ungetc(ch,file_ptr);
				token_train[token_count].token_type=TOKEN_ASSIGN;
				token_count++;
				return 0;
			}
		}
		if(ch == '!')
		{
			ch=fgetc(file_ptr);
			if(ch==(int) '=')
			{
				token_train[token_count].token_type=TOKEN_NEQUAL;
				token_count++;
				return 0;
			}
			else
			{
				return -1;
			}
			
		}
		char c=(char) ch;
		switch(c)
		{
			case '+': 
				token_train[token_count].token_type=TOKEN_ADD;
				token_count++;
				break;
			case '-':
				token_train[token_count].token_type=TOKEN_SUB;
				token_count++;
				break;
			case '*':
				token_train[token_count].token_type=TOKEN_MUL;
				token_count++;
				break;
			case '/':
				token_train[token_count].token_type=TOKEN_DIV;
				token_count++;
				break;
			case '{':
				token_train[token_count].token_type=TOKEN_LBRACE;
				token_count++;
				break;
			case '}':
				token_train[token_count].token_type=TOKEN_RBRACE;
				token_count++;
				break;
			case '(':
				token_train[token_count].token_type=TOKEN_LPAR;
				token_count++;
				break;
			case ')':
				token_train[token_count].token_type=TOKEN_RPAR;
				token_count++;
				break;
			case ';':
				token_train[token_count].token_type=TOKEN_SEMICOLON;
				token_count++;
				break;
			case '>':
				{
					int k=fgetc(file_ptr);
					if(k=='=')
					{
						token_train[token_count].token_type=TOKEN_GREATER_THAN_EQUAL;
						token_count++;
						break;
					}
					else
					{
						token_train[token_count].token_type=TOKEN_GREATER_THAN;
						token_count++;
						ungetc(k,file_ptr);
						break;
					}
				}
			case '<':
				{
					int k=fgetc(file_ptr);
					if(k == '=')
					{
						token_train[token_count].token_type=TOKEN_LESS_THAN_EQUAL;
						token_count++;
						break;	
					}
					else
					{
						token_train[token_count].token_type=TOKEN_LESS_THAN;
						token_count++;
						ungetc(k,file_ptr);
						break;
					}
				}
			default:
				return -1;
		}
		return 0;
	}	

	
	if(ch==EOF)
	{
		token_train[token_count].token_type=TOKEN_EOF;
		token_count++;
		return 1;
	}
}
void populate_token_train(FILE *file_ptr)
{
	int rtn_val;
	while(1)
	{
		rtn_val=add_next_token(file_ptr);
		if(rtn_val>0)
			break;
		if(rtn_val<0)
		{
			printf("error in  program syntax unrecognized line:%d\n", line_number);
			destroy_token_train();
			exit(1);
		}
	}
}
void destroy_token_train()
{
	if(!token_train)
		return;
	for(int i=0;i<token_count;i++)
	{
		if(token_train[i].token_type==TOKEN_ID)
		{
			if(token_train[i].id_str)
				free(token_train[i].id_str);
		}
	}
	free(token_train);
}