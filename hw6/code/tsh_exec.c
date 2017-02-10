#include "tsh_exec.h"

char wd[BUFFER_SIZE];

char *builtin_cmd[] = {
	"cd",
	"pwd"
};

int (*builtin_func_ptrs[]) (char **) = {
	&builtin_cmd_cd,
	&builtin_cmd_pwd
};

int is_builtin_cmd(char *cmd) {
	int i;
	for(i=0; i<BUILTIN_CMD_NUM; i++) {
		if(strcmp(cmd, builtin_cmd[i]) == 0) {
			return i;
		}
	}
	return -1;
}

int execute_builtin_cmd(int i, char **cmd)
{
	return (*builtin_func_ptrs[i])(cmd);
}

int builtin_cmd_cd (char **arg)
{
	char tar[BUFFER_SIZE];	
	if(arg[1] == NULL) {
		chdir(getenv("HOME"));
	} else {
		if(arg[1][0] == '~') {
			sprintf(tar, "%s%s", getenv("HOME"), &arg[1][1]);
			chdir(tar);
		} else {
			chdir(arg[1]);
		}
	}
	getcwd(wd, sizeof(wd));
	return 0;
}

int builtin_cmd_pwd (char **arg)
{
    getcwd(wd, sizeof(wd));
	printf("%s\n", wd);
	return 0;
}


void wait4children(int signal) {  
 	int status;  
 	while(waitpid(-1, &status, WNOHANG) > 0);  
}  

int execute_process(int argc, char **arg)
{
	pid_t pid, wait_pid;
	int status, i;
	int bg = (arg[argc-1][0]=='&')? 1 : 0;

	if(bg){ 
		signal(SIGCHLD, wait4children);
		arg[argc-1] = NULL;
	}
	pid = fork();
	if(pid == 0) {
		if(execvp(arg[0], arg) == -1) {
			if(arg[0][0]=='.' && arg[0][1]=='/')
				fprintf(stderr, "bash: %s No such file or directory\n", arg[0]);
			else
				fprintf(stderr, "%s command not found\n", arg[0]);
		}
		exit(1);
	} else if(pid > 0) {
		if(!bg) {
			do {
				wait_pid = waitpid(pid, &status, WUNTRACED);
			} while (!WIFEXITED(status) && !WIFSIGNALED(status));
		}
	} else {
		fprintf(stderr, "fork() failed.\n");
		exit(1);
	}

	return 0;
}


