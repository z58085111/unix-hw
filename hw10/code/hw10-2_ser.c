#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#define BUFFER_SIZE 4096

int get_total_proc_num(void);
void report_proc_num(int, struct sockaddr_in);

int main(int argc, char **argv)
{
	/*socket needs vars*/
	int listenfd, port, clifd;
	socklen_t socklen;
	pid_t childpid;
	struct sockaddr_in servaddr, cliaddr;
	
	if(argc!=2){
		printf("usage: ser <Port>\n");
		return ;
	}
	/*init*/
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	port = atoi(argv[1]);

	/*set main socket*/
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	if(bind(listenfd,(struct sockaddr *)&servaddr, sizeof(servaddr))<0) {
		fprintf(stderr, "bind failed\n");
		exit(1);
	}
	if(listen(listenfd, 1024) < 0) {
		fprintf(stderr, "listen failed\n");
		exit(1);
	}
	
	/*wait for connections*/
	while(1) {
		printf("Waiting for connections\n");
		socklen = sizeof(cliaddr);
		clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &socklen);
		if((childpid = fork()) == 0) {
			close(listenfd);
			report_proc_num(clifd, cliaddr);
			exit(0);
		}
		close(clifd);
	}
	return 0;
}

int get_total_proc_num(void)
{
	struct sysinfo si;
	return (sysinfo(&si) == 0) ? (int)si.procs : (int)-1;
}

void report_proc_num(int clifd, struct sockaddr_in cliaddr)
{
	ssize_t bytenum;
	char buf[BUFFER_SIZE+1]={'\0'};

	do {
		sprintf(buf, "process num of server: %d", get_total_proc_num());
		bytenum = send(clifd, buf, strlen(buf), MSG_NOSIGNAL);
		if(bytenum>0) {
			printf("send msg to %s:%d successfully\n", inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port));
			sleep(2);
		}
		else {
			printf("client %s:%d disconnected!\n", inet_ntoa(cliaddr.sin_addr), htons(cliaddr.sin_port));
		}
	} while (bytenum > 0);
}
