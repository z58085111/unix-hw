#include "tsh_exec.h"

char wd[BUFFER_SIZE];

char *builtin_cmd[] = {
	"cd",
	"pwd",
	"exit",
	"jobs",
	"fg"
};

int (*builtin_func_ptrs[]) (int, char **) = {
	&builtin_cmd_cd,
	&builtin_cmd_pwd,
	&builtin_cmd_exit,
	&builtin_cmd_jobs,
	&builtin_cmd_fg
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

int execute_builtin_cmd(int i, int argc, char **cmd)
{
	return (*builtin_func_ptrs[i])(argc, cmd);
}

int builtin_cmd_cd (int argc, char **arg)
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

int builtin_cmd_pwd (int argc, char **arg)
{
    getcwd(wd, sizeof(wd));
	printf("%s\n", wd);
	return 0;
}

int builtin_cmd_exit (int argc, char **arg)
{
	exit(0);
}

int builtin_cmd_jobs (int argc, char **argv)
{
	int i;

	for(i=1; i<=JOB_MAX_NUM; i++) {
		print_job_status(i);
	}
}

int builtin_cmd_fg (int argc, char **argv)
{
	int job_id = (argv[1] != NULL)? atoi(argv[1]) : 0;
	if(job_id < 0) {
		fprintf(stderr, "fg %d: job not found.", job_id);
		return -1;
	}

	job *job = jobs[job_id];
	if(kill(job->pid, SIGCONT) < 0) {
		fprintf(stderr, "tsh: kill job %d failed.", job_id);
		return -1;
	}

	tcsetpgrp(0, getpgid(job->pid));
	set_job_status(job->id, JOB_CONTINUED);
	wait_fg_process(job->pid);

	signal(SIGTTOU, SIG_IGN);
	tcsetpgrp(0, getpid());
	signal(SIGTTOU, SIG_DFL);
}

void wait_bg_job(int signal) {  
 	int status;
	pid_t childpid;

 	while((childpid = waitpid(-1, &status, WNOHANG)) > 0) {
		set_job_status_by_pid(childpid, status);
	}  
}

int wait_fg_process(pid_t pid)
{
	int status;
	pid_t childpid;

	do {
		childpid = waitpid(pid, &status, WUNTRACED);
		set_job_status_by_pid(childpid, status);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status) && !WIFSTOPPED(status));

	return status;
}

void wait_fg_job(pid_t childpid, int argc, char *argv[]) {  
	int status;

 	add_fg_job(childpid, argc, argv);
	status = wait_fg_process(childpid);
	
	if(WIFSTOPPED(status)) {
		delete_job(0);
		add_bg_job(childpid, argc, argv);
	}
}

void test()
{
	printf("hihi\n");
}

void execute_child(pid_t pid, int argc, char **argv)
{
	setpgid(0, pid);
	signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
    signal(SIGTSTP, test);
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, SIG_DFL);
	if(execvp(argv[0], argv) == -1) {
		if(argv[0][0]=='.' && argv[0][1]=='/')
			fprintf(stderr, "tsh: %s No such file or directory\n", argv[0]);
		else
			fprintf(stderr, "%s command not found\n", argv[0]);
	}
	exit(1);
}

void handle_new_job(int bg, pid_t childpid, int argc, char **argv)
{
	setpgid(childpid, childpid);
	if(!bg) {
		tcsetpgrp(0, childpid);
		wait_fg_job(childpid, argc, argv);
		
		signal(SIGTTOU, SIG_IGN);
		tcsetpgrp(0, getpid());
		signal(SIGTTOU, SIG_DFL);
	} else {
		add_bg_job(childpid, argc, argv);
	}
}

int execute_process(int argc, char **argv)
{
	pid_t pid;
	int bg = (argv[argc-1][0]=='&')? 1 : 0;

	if(bg){ 
		signal(SIGCHLD, wait_bg_job);
		argv[argc-1] = NULL;
		argc--;
	}
	pid = fork();
	if(pid == 0) {
		execute_child(getpid(), argc, argv);
	} else if(pid > 0) {
		handle_new_job(bg, pid, argc, argv);
	} else {
		fprintf(stderr, "fork() failed.\n");
		exit(1);
	}
	return 0;
}


