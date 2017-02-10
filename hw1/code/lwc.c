#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OPTION_NUM 3
#define MAX_FILE_NUM 10

char error_msg[512];

/*parse argument functions*/
int check_file_path(const char *file_path[], const int file_num)
{
	FILE *fp;
	int i, error=0, not_found=0, not_found_index[MAX_FILE_NUM]={0};
	for(i=0; i<file_num; i++) {
		fp = NULL;
		if((fp = fopen(file_path[i], "r")) != NULL){
			fclose(fp);
		} else{
			not_found_index[not_found++] = i;
		}
	}
	if(not_found == file_num) {
		error = 1;
		sprintf(error_msg, "wc: %s: No such file or directory\n", file_path[not_found_index[0]]);
	}
	return error;
}

int set_opt_num(const char opt, int *opts) 
{
	switch (opt) {
		case 'l':
			*opts |= 0x01;
			return 0;
		case 'w':
			*opts |= 0x02;
			return 0;
		case 'c':
			*opts |= 0x04;
			return 0;
		default:
			sprintf(error_msg, "wc: invalid option -- '%c'\nTry 'wc --help' for more information.\n", opt);
			return 1;	
	}
}

int set_opts(const char arg[], int *opts)
{
	int i, error=0;
	for(i=1, error=0; i<strlen(arg) && error==0; i++){
		error = set_opt_num(arg[i], opts);
	}
	return error;
}

int parse_lwc_arg(const int argc, const char *argv[], int *opts, char *file_path[], int *file_num)
{
	char *arg;
	int i, error=0;
	FILE *fp;

	if(argc < 2) {
		printf("please enter at least one option\n");
		printf("Usage: <lwc.exe> <option> <path>\n");
	} 
	for(i=1; i<argc && error==0; i++) {
		arg = (char *)argv[i];
		if(arg[0] == '-') 
			error = set_opts((const char *)arg, opts);
		else
			file_path[(*file_num)++] = arg;
	}
	return error;
}

void set_print_format(const int opts, const int byte_cnt, char *format)
{
	int max_digit;
	if(opts==1 || opts==2 || opts==4) {
		sprintf(format, "%%d ");
	} else {
		max_digit = sprintf(format, "%d", byte_cnt);
		memset(format, '\0', max_digit);
		sprintf(format, "%%%dd ", max_digit);
	}
}

void print_lwc_result(const int opts, const char *file_path, const int cntr[])
{
	int i, mask;
	char format[128]={'\0'}, result[512]={'\0'}, index=0;

	set_print_format(opts, cntr[MAX_OPTION_NUM-1], format);
	for(i=0, mask=0x01; i<MAX_OPTION_NUM; i++, mask<<=1) {
		if((opts & mask) != 0) {
			index += sprintf(&result[index], format, cntr[i]);
		}
	}
	sprintf(&result[index], "%s\n", file_path);

	printf("%s", result);
}

void count_file_content(const int opts, FILE *fp, int cntr[])
{	
	int word_flag = 0, j;
	char ch;

	while ((ch = fgetc(fp)) != EOF) {
		cntr[2]++;
		switch(ch) {
			case '\0': case ' ':  case '\t': 
	        case '\n': case '\r': 
	            if (word_flag) { word_flag = 0; cntr[1]++; }
	            if (ch=='\n') cntr[0]++;
	            break;
	        default: 
	        	word_flag = 1;
		}
	}
	fclose(fp);
}

void word_count(const int opts, const char *file_path[], const int file_num)
{
	/*  cntr[0]: line
		cntr[1]: word
		cntr[2]: byte */
	int cntr[MAX_OPTION_NUM]={0}, total[MAX_OPTION_NUM]={0};
	int i, j;
	FILE *fp;
	
	for(i=0; i<file_num; i++) {
		if((fp = fopen(file_path[i], "r")) == NULL) 
			continue;
		
		count_file_content(opts, fp, cntr);
		print_lwc_result(opts, file_path[i], cntr);
		for(j=0; j<MAX_OPTION_NUM; j++) {
			total[j] += cntr[j];
			cntr[j] = 0;
		}
	}
	if(file_num > 1) {
		print_lwc_result(opts, "total", total);
	}
}

void print_error_msg(const int error)
{
	fprintf(stderr, "%s", error_msg);
	exit(error);
}

int main (int argc, char *argv[]) 
{
	int error=0, file_num=0, opts=0;
	char *file_path[MAX_FILE_NUM];

	if((error = parse_lwc_arg(argc, (const char **)argv, &opts, file_path, &file_num)) != 0)
		print_error_msg(error);
	if((error = check_file_path((const char **)file_path, file_num)) != 0)
		print_error_msg(error);
	word_count(opts, (const char **)file_path, file_num);

	return 0;
}
