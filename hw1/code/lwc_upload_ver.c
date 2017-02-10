#include <stdio.h>
#include <string.h>

#define MAX_OPTION_NUM 3
#define MAX_FILE_NUM 10
#define INVALID_OPTION_MSG "Try 'wc --help' for more information.\n"
#define NOT_EXISTED_MSG "No such file or directory\n"

/*parse argument functions*/
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
			fprintf(stderr, "wc: invalid option -- '%c'\n", opt);
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
		if(arg[0] == '-') {
			error = set_opts((const char *)arg, opts);
		} else {
			fp = NULL;
			if((fp = fopen(arg, "r")) != NULL) {
				file_path[(*file_num)++] = arg;
				fclose(fp);
			} else {
				fprintf(stderr, "wc: %s: ", arg);
				error = 2;
			}
		}
	}
	return error;
}

void print_error_msg(const int error)
{
	switch (error) {
		case 1:
			fprintf(stderr, INVALID_OPTION_MSG);
			break;
		case 2:
			fprintf(stderr, NOT_EXISTED_MSG);
			break;
		default:
			break;
	}
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

void print_lwc_result(const int opts, const char *file_path, int cntr[], const char *format)
{
	int i, mask;
	char result[512]={'\0'}, index=0;

	for(i=0, mask=0x01; i<MAX_OPTION_NUM; i++, mask<<=1) {
		if((opts & mask) != 0) {
			index += sprintf(&result[index], format, cntr[i]);
		}
	}
	sprintf(&result[index], "%s\n", file_path);

	printf("%s", result);
}

void count_file_content(const int opts, const char *file_path, int cntr[])
{	
	int word_flag = 0, j;
	char ch;
	FILE *fp = fopen(file_path, "r");
	
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
	int cntr[MAX_FILE_NUM][MAX_OPTION_NUM]={0}, total[MAX_OPTION_NUM]={0}, max_digit=0;
	int i, j;
	char format[128]={'\0'};
	
	
	for(i=0; i<file_num; i++) {
		count_file_content(opts, file_path[i], cntr[i]);
		if(cntr[i][MAX_OPTION_NUM-1] > max_digit)
			max_digit = cntr[i][MAX_OPTION_NUM-1];
		for(j=0; j<MAX_OPTION_NUM; j++)
			total[j] += cntr[i][j];
	}

	set_print_format(opts, max_digit, format);
	for(i=0; i<file_num; i++) {
		print_lwc_result(opts, file_path[i], cntr[i], format);
	}
	if(file_num > 1) {
		print_lwc_result(opts, "total", total, format);
	}
}

int main (int argc, char *argv[]) 
{
	int error=0, file_num=0, opts=0;
	char *file_path[MAX_FILE_NUM];

	error = parse_lwc_arg(argc, (const char **)argv, &opts, file_path, &file_num);
	if(error == 0)
		word_count(opts, (const char **)file_path, file_num);
	else
		print_error_msg(error);
	return error!=0;
}
