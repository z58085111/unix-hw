#include "job_control.h"

#define BUFFER_SIZE 256
#define TOKEN_DELIM " \t\n\r"
#define BUILTIN_CMD_NUM 5

int is_builtin_cmd(char *);
int execute_builtin_cmd(int, int, char **);
int execute_process(int, char **);

int builtin_cmd_cd (int, char **);
int builtin_cmd_pwd (int, char **);
int builtin_cmd_exit (int, char **);
int builtin_cmd_jobs (int, char **);
int builtin_cmd_fg (int, char **);

void wait_bg_job(int signal);
void wait_fg_job(int, int, char **);
int wait_fg_process(pid_t);
void execute_child(pid_t, int, char **);
void handle_new_job(int, pid_t, int, char **);

extern char wd[BUFFER_SIZE];
extern char *builtin_cmd[BUILTIN_CMD_NUM];
extern int (*builtin_func_ptrs[BUILTIN_CMD_NUM]) (int, char **);
