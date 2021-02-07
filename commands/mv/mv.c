#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char** argv){
    FILE *fp;
    int ret;
    struct stat buf;

    if(argc != 3){
      printf("argument shape is not correct!\n");
      exit(1);
    }

    if((fp = fopen(argv[1], "r")) == NULL){
      printf("%s is not exist\n", argv[1]);
      exit(1);
    }
    //if argv[2] is directory
    ret = stat(argv[2], &buf);
    if(S_ISDIR(buf.st_mode)){
    
      char newfilename[256];
      char srash[] = "/";

      // strlcpyなどはx86_64アーキテクチャのみで動く？
      strlcpy(newfilename, argv[2], sizeof(newfilename));
      strlcat(newfilename, srash, sizeof(newfilename));
      strlcat(newfilename, argv[1], sizeof(newfilename));

      if(rename(argv[1], newfilename) == 0){
        printf("%s move to %s\n", argv[1], newfilename);
        exit(0);
      }
      else{
        printf("destination directory is not exist,move is not possible\n");
        exit(1);
      }
    }

    //if argv[2] is filename
    if(rename(argv[1], argv[2]) == 0){
      printf("%s move to %s\n", argv[1], argv[2]);
      exit(0);
    }
    else{
      printf("destination name is not exist, so move is not possible\n");
      exit(1);
    }

    return 0;
}

