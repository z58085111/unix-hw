#include "job_control.h"

job *jobs[JOB_MAX_NUM+1];

char *JOB_STATUS[] = {
	"Running",
	"Stopped",
	"Continued",
	"Terminated",
	"Done"
};

job *get_new_job(int id, pid_t pid, int argc, char **argv)
{
	job *new_job = (job *) malloc(sizeof(job));
	new_job->id = id;
	new_job->pid = pid;
	new_job->argc = argc;
	new_job->argv = argv;
	new_job->status = 0;
	return new_job;
}

int new_job_id(void)
{
	int i;
	for(i=1; i<=JOB_MAX_NUM; i++) {
		if(jobs[i] == NULL)
			return i;
	}
	return -1;
}

void add_fg_job(pid_t pid, int argc, char **argv)
{
	job *new_job = get_new_job(0, pid, argc, argv);
	jobs[0] = new_job;
}

void add_bg_job(pid_t pid, int argc, char **argv)
{
	int jid = new_job_id();
	if((jid = new_job_id()) < 0) {
		fprintf(stderr, "job space run out");
		return ;		
	}
	printf("[%d] %d\n", jid, pid);
	job *new_job = get_new_job(jid, pid, argc, argv);
	jobs[jid] = new_job;
}

void delete_job(int id)
{
	if(id <= JOB_MAX_NUM && id >= 0 && jobs[id] != NULL) {
		print_job_status(id);
		free(jobs[id]);
		jobs[id] = NULL;
	}
}

int get_job_index(pid_t pid)
{
	int i;
	for(i=0; i<=JOB_MAX_NUM; i++) {
		if(jobs[i]!=NULL && jobs[i]->pid == pid)
			return i;
	}
	return -1;
}

void print_job_status(int i)
{
	int j;
	if(jobs[i] == NULL) return;
	printf("[%d]  %s\t\t", jobs[i]->id, JOB_STATUS[jobs[i]->status]);
	for(j=0; j<jobs[i]->argc; j++) {
		printf(" %s", jobs[i]->argv[j]);
	}
	printf("\n");
}

void set_job_status_by_pid(pid_t pid, int status)
{
	int index = get_job_index(pid);
	if(index >= 0)
		set_job_status(index, status);
}

void set_job_status(int id, int status)
{
	if(jobs[id] == NULL) return;

	if (WIFEXITED(status)) {
		jobs[id]->status = JOB_DONE;
    } else if (WIFSIGNALED(status)) {
		jobs[id]->status = JOB_TERM;
    } else if (WIFSTOPPED(status)) {
		jobs[id]->status = JOB_STOPPED;
    } else if(WIFCONTINUED(status)) {
		jobs[id]->status = JOB_CONTINUED;
	}
}

int check_dead_job(int i)
{
	if(jobs[i] == NULL) {
		return 0;
	} else if(jobs[i]->status == JOB_DONE) {
		return 1;
	} else if(jobs[i]->status == JOB_TERM) {
		return 1;
	}
}

void check_jobs_status()
{
	int i;
	for(i=1; i<=JOB_MAX_NUM; i++) {
		if(check_dead_job(i))
			delete_job(i);
	}
}


