#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAX_BUF (256)

int main(int argc,char *argv[]){
    FILE *file[2];
    char str1[MAX_BUF][MAX_BUF];
    char str2[MAX_BUF][MAX_BUF];

    if(argc!=3){
        printf("usage:diff file1 file2\n");
        return 0;
    }

    file[0] = fopen(argv[1], "r");
    file[1] = fopen(argv[2], "r");
    if (file[0] == NULL||file[1] == NULL) {
        printf("no exist\n\n");
    }
    int num1=0,num2=0;
    while ((fgets(str1[num1], MAX_BUF - 1, file[0])) != NULL) {
        num1++;
    }
    while ((fgets(str2[num2], MAX_BUF - 1, file[1])) != NULL) {
        num2++;
    }
    for (int i=0;i<num1;i++) {
        if(strcmp(str1[i],str2[i])){
            printf("%d\n",i+1);
            printf("%s",str2[i]);
        }
    }
    printf("\n\n");
    fclose(file[0]);
    fclose(file[1]);

    return 0;
}