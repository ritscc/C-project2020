#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define N 256
#define FALSE 0

int main(int argc, char** argv){

	FILE *file;
	char LineFromFile[N];
	char GetLine[N];
	int count;
	for(int i=0; i<sizeof(argv[1]);i++){
		GetLine[i] = argv[1][i];
	}

	if(argc < 3){
		fprintf(stderr, "Usage: ./custom_grep <search> <filename>\n");
		return 1;
	}

	
		
	if((file = fopen(argv[argc-1], "r")) == NULL) {
		fprintf(stderr, "This file does not exist\n");
	}

	while (fgets(LineFromFile, N, file) != NULL){
		if(strstr(LineFromFile,GetLine) != NULL){
			printf("%s",LineFromFile);
		}
	}
	fclose(file);
	return 0;
}
