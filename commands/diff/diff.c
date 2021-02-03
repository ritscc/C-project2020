#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_BUF (256)

int main(int argc,char *argv[]){
    FILE *file[2];
    char str1[MAX_BUF];
    char str2[MAX_BUF];

    if(argc!=3){
        printf("usage:diff file1 file2\n");
        return 0;
    }

    file[0] = fopen(argv[1], "r");
    file[1] = fopen(argv[2], "r");
    if (file[0] == NULL||file[1] == NULL) {
        printf("no exist\n\n");
    }
    while ((fgets(str1, MAX_BUF - 1, file[0])) != NULL) {
        fgets(str2, MAX_BUF - 1, file[1]);
        if(strcmp(str1,str2)){
            printf("%s",str2);
        }
    }
    printf("\n\n");
    fclose(file[0]);
    fclose(file[1]);

    return 0;
}