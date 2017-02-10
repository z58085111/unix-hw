#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILENAME_LEN 256
#define BUFFER_SIZE 1024

char source_path[FILENAME_LEN+1];
char target_path[FILENAME_LEN+1];

int error_code = 0;
char error_msg[512];

void set_error(int code, char *format, ... ) 
{
	va_list valist;
	va_start(valist, format);

	error_code = code;
	sprintf(error_msg, format, valist);
	va_end(valist);
}

void handle_error()
{
	fprintf(stderr, "%s", error_msg);
	exit(error_code);
}

char *parse_filename(char *path) {
	char *ptr = strrchr(path, '/');
	ptr = (ptr == NULL)? path : ptr+1;
	return ptr;
}

int is_dir(char *path) 
{
	DIR* dir = opendir(path);
	
	if(dir != NULL) {
		closedir(dir);
        return 1;
	}
	
	if(errno == ENOTDIR || errno == ENOENT) {
		return 0;
	}

	return -1;
}

int open_target()
{
	int t_len = strlen(target_path);
	if(is_dir(target_path)) {
		if(target_path[t_len-1] != '/') { 
			target_path[t_len] = '/';
			target_path[t_len+1] = '\0';
		}
		strcat(target_path, parse_filename(source_path));
	}
	return open(target_path, O_CREAT | O_WRONLY, S_IRWXO | S_IRWXU);
}

int parse_argu(int argc, char *argv[]) 
{
	int i;
	char *arg;

	for(i=1; i<argc && error_code==0; i++) {
		arg = argv[i];
		if(i == 1)
			strcpy(source_path, arg);
		else if(i == 2)
			strcpy(target_path, arg);
		else {
			set_error(1, "usage: cp [-R [-H | -L | -P]] [-fi | -n] [-apvX] source_file target_file\ncp [-R [-H | -L | -P]] [-fi | -n] [-apvX] source_file ... target_director\n");
		}
	}
	return error_code;
}

void copy()
{
	int srcfd, tarfd;
	char ch = '\0';
	
	if((srcfd = open(source_path, O_RDONLY)) < 0) {
		printf("%s,%d\n", source_path, srcfd);
		set_error(1, "cp: %s: No such file or directory\n", source_path);
		handle_error();
	}

	if((tarfd = open_target()) < 0) {
		set_error(1, "cp: cannot write to target file: %s\n", source_path);
		handle_error();
	}

	while(read(srcfd, &ch, 1) == 1) {
		if(ch == 0) ch = '\0';			
		write(tarfd, &ch, 1);
	}
	
	close(srcfd);
	close(tarfd);
}

int main(int argc, char *argv[])
{
	int *srcfd, *tarfd;

	if(parse_argu(argc, argv) != 0)
		handle_error();

	copy();
	return 0;
}
