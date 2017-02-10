#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256
#define TOKEN_DELIM " \t\n\r"
#define BUILTIN_CMD_NUM 2

int is_builtin_cmd(char *);
int execute_builtin_cmd(int, char **);
int execute_process(int, char **);

int builtin_cmd_cd (char **);
int builtin_cmd_pwd (char **);

void wait4children(int);

