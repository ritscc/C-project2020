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
    //printf("%d\n",num1);
    while ((fgets(str2[num2], MAX_BUF - 1, file[1])) != NULL) {
        num2++;
    }
    //printf("%d\n",num2);
    int searchA,searchB;
    for (searchA=0,searchB=0;searchA<num1;searchA++,searchB++) {
        int insertion=0;
        int delete=0;
        //printf("%d %d\n",searchA,searchB);
        //int a=strcmp(str1[searchA],str2[searchB]);
        if(strcmp(str1[searchA],str2[searchB])){
            for (int i = searchA + 1; i < num1; i++) {
                if (!strcmp(str1[i], str2[searchB])) {
                    for (int j = searchA; j < i; j++) {
                        printf("<%s", str1[j]);
                    }
                    searchA = i;
                    delete = 1;
                    break;
                }
            }

            if (!delete) {
                for (int i = searchB + 1; i < num2; i++) {
                    if (!strcmp(str1[searchA], str2[i])) {
                        for (int j = searchB; j < i; j++) {
                            printf(">%s", str2[j]);
                        }
                        searchB = i;
                        insertion = 1;
                        break;
                    }
                }
            }
            if(!delete&&!insertion) printf(">%s", str2[searchB]);
        }
    }
    for(int i=searchB;i<num2;i++){
        printf(">%s",str2[i]);
    }
    
    printf("\n\n");
    fclose(file[0]);
    fclose(file[1]);

    return 0;
}