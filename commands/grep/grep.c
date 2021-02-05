#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define N 256
#define LINE 10000

typedef struct match_line_t{
	int line_number;
	char line[N];
}match_line_t;

typedef struct unmatch_line_t{
	int line_number;
	char line[N];
}unmatch_line_t;


int max(int a, int b){
	if(a>b) return a;
	else return b;
}

int main(int argc, char** argv){

	match_line_t match_line[LINE]={};
	unmatch_line_t unmatch_line[LINE]={};

	FILE *file;
	char LineFromFile[N];
	char GetLine[N];
	int i;
	int line_count = 1;

	/*エラー処理*/
	if(argc < 3){
		fprintf(stderr, "Usage: ./grep [option] <search> <filename>\n");
		return 1;
	}

	if((file = fopen(argv[argc-1], "r")) == NULL) {
		fprintf(stderr, "This file does not exist\n");
	}

	int opt_i = 0;
	int opt_v = 0;
	int opt_n = 0;
	//option select
	for(i=0;i<argc-2;i++){
		if(argv[i][0] == '-'){
			int j=1;
			while(j<sizeof(argv[i])){
				switch(argv[i][j]){
					case 'i':
						opt_i++;
						break;
					case 'v':
						opt_v++;
						break;
					case 'n':
						opt_n++;
						break;
				}
				j++;
			}
		}
	}

	/*文字取得*/	
	for(i=0; i<sizeof(argv[argc-2]);i++){
		GetLine[i] = argv[argc-2][i];
	}
	
	int count = 0;
	int se_count = 0;
	//devided MatchLine and UnMatchLine from LineFromFile//
	while(fgets(LineFromFile, N, file) != NULL){
		if(opt_i){
			for(i=0;i<N;i++){
				GetLine[i] = tolower(GetLine[i]);
				LineFromFile[i] = tolower(LineFromFile[i]);
			}
		}
		if(strstr(LineFromFile,GetLine) == NULL){
			match_line[count].line_number = line_count; 
			strncpy(match_line[count].line,LineFromFile,N);
			count++;
		}else if(strstr(LineFromFile,GetLine) != NULL){
			unmatch_line[se_count].line_number = line_count; 
			strncpy(unmatch_line[se_count].line,LineFromFile,N);
			se_count++;
		}
		line_count++;
	}
	
	i = 0;
	int number;
	char line[N];
	while(i < max(count,se_count)){
		if(opt_v){
			number = match_line[i].line_number;
			strncpy(line,match_line[i].line,N);
		}else{
			number = unmatch_line[i].line_number;
			strncpy(line,unmatch_line[i].line,N);
		}


		if(opt_n){
			if(number == 0){
				break;
			}
			printf("%6d  ", number);
		}

		printf("%s",line);
		//print_text(line,GetLine);

		i++;
	}

	fclose(file);
	return 0;
}
