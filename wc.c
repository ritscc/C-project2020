#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdbool.h>

#define N 1024
#define TOK_DELIM " \t\r\n\a"

typedef struct {
	unsigned int c:1;
	unsigned int m:1;
	unsigned int L:1;
	unsigned int w:1;
	unsigned int cmLw:1;
} options;

int count_byte(char *);
int count_character(char *);
int max_line_byte(char *);
int count_words(char *);
int count_line(char *);

int main(int argc, char **argv){
	int ch;
	int cb, cc, mlb, cw, cl;
	char *target_file;
	options opts = {};

	if(argc == 2 && argv[1][0] != '-'){
		opts.cmLw = 1;
		target_file = argv[1];
	} else {
		while((ch = getopt(argc, argv, "cmLw")) != -1){
			switch(ch){
				case 'c':
					opts.c = 1;
					break;
				case 'm':
					opts.m = 1;
					break;
				case 'L':
					opts.L = 1;
					break;
				case 'w':
					opts.w = 1;
					break;
				default:
					printf("Try 'wc --help' for more information.\n");
					exit(1);
			}
		}
		target_file = argv[optind];
	}

	if((!opts.c && !opts.m && !opts.L && !opts.w && !opts.cmLw) || target_file == NULL){
		printf("Try 'wc --help' for more information.\n");
		exit(1);
	}

	//printf("flags c: %d, m: %d, L: %d, w: %d\n", opts.c, opts.m, opts.L, opts.w);
	//printf("target_file %s\n", target_file);

	if(opts.cmLw || opts.c) cb = count_byte(target_file);
	if(opts.m) cc = count_character(target_file);
	if(opts.cmLw) cl = count_line(target_file);
	if(opts.L) mlb = max_line_byte(target_file);
	if(opts.cmLw || opts.w) cw = count_words(target_file);
	/*
	printf("cb: %d\n", cb);
	printf("mlb: %d\n", mlb);
	printf("cc: %d\n", cc);
	printf("cl: %d\n", cl);
	printf("cw: %d\n", cw);
	*/

	if(opts.cmLw) printf(" %d %d %d %s\n", cl, cw, cb, target_file);
	else {
		if(opts.w) printf(" %d", cw);
		if(opts.c) printf(" %d", cb);
		if(opts.m) printf(" %d", cc);
		if(opts.L) printf(" %d", mlb);
		printf(" %s\n", target_file);
	}

	return 0;
}

int count_byte(char *target_file){
	FILE *fp;
	int cnt = 0;

	if((fp = fopen(target_file, "r")) != NULL)
		while(fgetc(fp) != EOF) cnt++;
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}

int count_character(char *target_file){
	FILE *fp;
	char c;
	int cnt = 0;

	if((fp = fopen(target_file, "r")) != NULL) {
		while((c = fgetc(fp)) != EOF) cnt++;
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}

int max_line_byte(char *target_file){
	FILE *fp;
	char str[N], *ptr;
	int cnt = 0;

	if((fp = fopen(target_file, "r")) != NULL)
		while(fgets(str, N, fp) != NULL){
			int row_cnt = 0;
			ptr = str;
			while(*ptr != '\0' && *ptr != '\n' && *ptr != EOF){
				row_cnt++;
				ptr+=1;
			}
			if(cnt < row_cnt)cnt = row_cnt;
		}
	else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}

int count_words(char *target_file){
	FILE *fp;
	int cnt = 0;
	char str[N];

	if((fp = fopen(target_file, "r")) != NULL) {
		while(fgets(str, N, fp) != NULL){
			char *word = strtok(str, TOK_DELIM);
			while(word != NULL) {
				cnt++;
				word = strtok(NULL, TOK_DELIM);
			}
		}
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}

int count_line(char *target_file){
	FILE *fp;
	char c;
	int cnt = 0;

	if((fp = fopen(target_file, "r")) != NULL) {
		while(1){
			c = fgetc(fp);
			if(c == '\n')cnt++;
			if(c == EOF)break;
		}
	} else {
		printf("wc: %s: No such file or directory\n", target_file);
		return -1;
	}
	fclose(fp);

	return cnt;
}
