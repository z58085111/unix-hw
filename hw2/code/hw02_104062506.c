#include "hw2.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

long OPEN_MAX;

int is_opened(int fd) {
    return lseek(fd, 0, SEEK_CUR)>=0 || 0<=fd || fd<=2;
}

int is_error(int oldfd, int newfd) {
    return is_opened(oldfd) != 1 ||
           newfd < 0 || newfd >= OPEN_MAX ||
	       oldfd < 0 || oldfd >= OPEN_MAX ;
}

int mydup2(int oldfd, int newfd){
    OPEN_MAX = sysconf(_SC_OPEN_MAX);
    int i=0, j;
    int dupfd[OPEN_MAX], dupnum=0;

    if(is_error(oldfd, newfd) == 1) {
    	i = -1;
    } else if(oldfd == newfd) {
    	i = newfd;
    } else if(is_opened(newfd) == 1) {
    	close(newfd);
    }
    
    while (i != newfd && i != -1) {
		i = dup(oldfd);
		dupfd[dupnum++] = i;
	} 
	for(j=dupnum-2; j>=0; j--) {
		close(dupfd[j]);
	}

	return i;
}
