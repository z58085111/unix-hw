#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define JOB_MAX_NUM 30
#define JOB_STATUS_NUM 5
#define JOB_RUNNING 0
#define JOB_STOPPED 1
#define JOB_CONTINUED 2
#define JOB_TERM 3
#define JOB_DONE 4

typedef struct _job {
	int id;
	int status;
	int argc;
	char **argv;
	pid_t pid;
} job;

void add_fg_job(pid_t, int, char **);
void add_bg_job(pid_t, int, char **);
void delete_job(int);
int check_dead_job(int);
void check_jobs_status(void);
int new_job_id(void);
job *get_new_job(int, pid_t, int, char **);

int get_job_index(pid_t);
void set_job_status(int, int);
void set_job_status_by_pid(pid_t, int);
void print_job_status(int);

extern char *JOB_STATUS[JOB_STATUS_NUM];
extern int job_num;
extern job *jobs[JOB_MAX_NUM+1];
