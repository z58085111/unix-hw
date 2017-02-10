#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#define BUFFER_SIZE 4096

void recv_msg(int);

int main(int argc, char **argv)
{
	int sockfd, port;
	struct sockaddr_in servaddr;
	if(argc < 3) {
		printf("usage: cli <Server IP Address> <Port>\n");
		return 0;
	}
	/*init*/
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error\n");
		exit(1);
	}
	port = atoi(argv[2]);
	bzero(&servaddr, sizeof(servaddr));
	/*setup socket*/
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		fprintf(stderr, "inet_pton failed for %s\n", argv[1]);
		exit(1);
	}
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) <0 ) {
		fprintf(stderr, "connect to server failed.\n");
		exit(1);
	}
	recv_msg(sockfd);
	return 0;
}

void recv_msg(int sockfd)
{
	ssize_t bytenum;
	char buf[BUFFER_SIZE+1];

	do {
		bytenum = recv(sockfd, buf, sizeof(buf), MSG_NOSIGNAL);
		if(bytenum > 0)
			printf("recv msg from server: %s\n", buf);
	} while(bytenum > 0);
	printf("socket is closed.\n");
}
