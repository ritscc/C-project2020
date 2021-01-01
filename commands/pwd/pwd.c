nclude <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define N 256

int main(int argc, char** argv){

	FILE *file;
	char LineFromFile[N];
	char GetLine[N];

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
	int opt_o = 0;
	int opt_C = 0;
	int opt_C_line = 0;
	/*オプション*/
	for(int i=0;i<argc-2;i++){
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
		        case 'o':
				opt_o++;
				break;
			case 'C':
				opt_C++;
				opt_C_line = atoi(argv[i+1]);
				break;
		    }
		    j++;
		}
	    }
	}

	/*文字取得*/	
	for(int i=0; i<sizeof(argv[argc-2]);i++){
		GetLine[i] = argv[argc-2][i];
	}
	int line=1;
	while (fgets(LineFromFile, N, file) != NULL){
	    if(strstr(LineFromFile,GetLine) == NULL && opt_v){
		if(opt_n)   printf("%6d	    %s",line,LineFromFile);
		else	printf("%s",LineFromFile);
	    }else if(strstr(LineFromFile,GetLine) != NULL && !opt_v){
		if(opt_n)   printf("%6d	    %s",line,LineFromFile);
		else	printf("%s",LineFromFile);
	    }
	    line++;
	}
	fclose(file);
	return 0;
}
