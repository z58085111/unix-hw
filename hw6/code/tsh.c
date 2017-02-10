#include "tsh_exec.h"

extern int (*builtin_func_ptrs[]) (char **);
extern char *builtin_cmd[];
extern char wd[BUFFER_SIZE];

void shell_loop(void);
char *read_line(void);
char **parse_line(char *, int *);
int execute(int, char **);
char *reallocate_buff(char *, size_t *);

int main(int argc, char **argv)
{
	getcwd(wd, sizeof(wd));
	shell_loop();
	return 0;
}

void shell_loop(void)
{
	char *line = NULL;
	char **arg = NULL;
	int argc = 0;

	while(1) {
		printf("tsh:%s$", wd);
		line = read_line();
		arg = parse_line(line, &argc);
		execute(argc, arg);
		free(line);
	}
}

char *read_line()
{
	char *buffer = malloc(sizeof(char)*BUFFER_SIZE);
	size_t buff_size = BUFFER_SIZE;
	char ch;
	int len = 0;

	if(!buffer) {
		fprintf(stderr, "tsh: allocate buffer failed.\n");
		exit(1);
	}
	
	while (1) {
		ch = getchar();
		
		if(ch == EOF || ch == '\n') {
			buffer[len] = '\0';
			return buffer;
		}

		buffer[len++] = ch;
		if(len >= buff_size) {
			reallocate_buff(buffer, &buff_size);
		}
	}

	return buffer;
}

char *reallocate_buff(char *buff, size_t *buff_size)
{
	*buff_size += BUFFER_SIZE;
	buff = realloc(buff, *buff_size);
	if(!buff) {
		fprintf(stderr, "tsh: allocate read-line buffer failed.\n");
		exit(1);
	}
	return buff;
}

char **parse_line(char *line, int *token_num)
{
	char *const dupline = strdup(line);
	char *substr = strtok(dupline, TOKEN_DELIM);
	char **token_buffer = malloc(sizeof(char *)*BUFFER_SIZE);
	int buffer_size = BUFFER_SIZE;
	*token_num = 0;	

	if(!token_buffer) {
		fprintf(stderr, "tsh: allocate read-line buffer failed.\n");
		exit(1);
	}
	while(substr != NULL) {
		token_buffer[(*token_num)++] = substr;
		if(*token_num >= buffer_size) {
			buffer_size += BUFFER_SIZE;
			token_buffer = realloc(token_buffer, sizeof(char *)*buffer_size);
			if(!token_buffer) {
				fprintf(stderr, "tsh: allocate read-line buffer failed.\n");
				exit(1);
			}
		}
		substr = strtok(NULL, TOKEN_DELIM);
	}

	return token_buffer;
}

int execute(int argc, char **arg)
{	
	if(arg[0] == NULL) {
		fprintf(stderr, "tsh: no argument.\n");
		exit(1);
	}
	int builtin_num = is_builtin_cmd(arg[0]);

	return (builtin_num >= 0)? execute_builtin_cmd(builtin_num, arg)
							 : execute_process(argc, arg);
}
