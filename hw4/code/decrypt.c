#define _GNU_SOURCE
#define SEED_MAX 100
#define WORD_MAX 256
#define LINE_MAX 1024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>
#include <pthread.h>

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
int flag=0;

void guess_thread(void *arg)
{
	int pi = *(int *)arg;
	struct crypt_data data;
	if(flag == 1) return;
	for(int i2=0; i2<seed_size && flag==0; i2++) {
		for(int i3=0; i3<seed_size && flag==0; i3++) {
			if(pi^i2 && i2^i3 && i3^pi) {
				char pwd[3*WORD_MAX+1]={0};
				pwd[0] = '\0';
				sprintf(pwd, "%s%s%s", seed[pi], seed[i2], seed[i3]);
				data.initialized = 0;
				char *code = crypt_r(pwd, crypt_info, &data);
				if(strcmp(code, ans) == 0) {
					flag = 1;
					printf("%s\n", pwd);
					return;
				}
			}
		}
	}
}

void guess(void)
{
	pthread_t tid[seed_size];
	int pi[seed_size];

	for(int i=0; i<seed_size; i++) {
		pi[i] = i;
		pthread_create(&tid[i], NULL, (void *)guess_thread, (void *)&pi[i]);
	}
	for(int i=0; i<seed_size; i++) {
		pthread_join(tid[i], NULL);
	}
}

int main(int argc, char *argv[])
{
	if(argc < 2) {
		printf("Usage: ./decrypt <password file> [dic_filepath]\n");
		exit(1);
	}

	if(argc >= 3) 
		dic_src = argv[2];

	load_dictionary(dic_src);
	parse_input(argv[1]);
	guess();
	return 0;
}
