#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <omp.h>
#define SEED_MAX 100
#define WORD_MAX 256
#define LINE_MAX 1024

int seed_size=0;
char seed[SEED_MAX][WORD_MAX+1];
char used_map[SEED_MAX];
char ans[LINE_MAX+1];
char crypt_info[LINE_MAX+1];
char *dic_src = "./john.txt";

void parse_input(char *input_path)
{
	int i=0;	
	char buf[LINE_MAX+1], *start, *end, *crypt_info_end;
	FILE *fp = fopen(input_path, "r");
	if(fp == NULL) {
		printf("input file not found\n");
		exit(1);
	}
	if(fgets(buf, LINE_MAX, fp) == NULL) {
		printf("cannot find password info.\n");
		exit(1);	
	}
	start = strchr(buf, '$');
	end = strchr(start, ':');
	crypt_info_end = strrchr(buf, '$');
	while (start != end) {
		if(start != crypt_info_end) {
			crypt_info[i] = *start;
		}
		ans[i++] = *start;
		start++;
	}
}

void load_dictionary(char *dic_path)
{
	int len=0;
	char buf[WORD_MAX+1];
	FILE *fp = fopen(dic_path, "r");
	if(fp == NULL) {
		printf("dictionary file not found\n");
		exit(1);
	}
	while(fgets(seed[seed_size], WORD_MAX, fp) != NULL) {
		len = strlen(seed[seed_size]);
		seed[seed_size][len-1] = '\0';
		seed_size++;
	}
}

void guess()
{
	int flag=0;
	struct crypt_data data;

	#pragma omp parallel for schedule(dynamic) num_threads(seed_size)
	for(int i=0; i<seed_size; i++) {
		if(flag == 1) continue;
		for(int j=0; j<seed_size && flag==0; j++) {
			for(int k=0; k<seed_size && flag==0; k++) {
				if(i^j && j^k && k^i) {
					char pwd[3*WORD_MAX+1]={0};
					pwd[0] = '\0';
					sprintf(pwd, "%s%s%s", seed[i], seed[j], seed[k]);
					data.initialized = 0;
					char *code = crypt_r(pwd, crypt_info, &data);
					#pragma omp flush(code)
					if(strcmp(code, ans) == 0) {
						flag = 1;
						#pragma omp flush(flag)
						printf("%s\n", pwd);
					}
				}
			}
		}
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("Usage: ./decrypt <password file> [dic_filepath]\n");
		exit(1);
	}

	if(argc >= 3) {
		dic_src = argv[2];
	}	
	load_dictionary(dic_src);
	parse_input(argv[1]);
	guess();
	return 0;
}
