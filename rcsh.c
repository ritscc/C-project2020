#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char** read_cmd(int*);
pid_t execute(char *argv[]);

int main(void) {
    while (1) {
        printf("rcsh> ");
        int argc;
        char** argv = read_cmd(&argc);
        if(argc < 1) continue;

        if(strcmp(argv[0], "exit") == 0) break;
        pid_t pid = execute(argv);
        int status;
        wait(&status);
        printf("Exited with status %d\n", WEXITSTATUS(status));
        free(argv);
    }
    return 0;
}

char** read_cmd(int* argc) {
    const int BUFSIZE = 1024;
    const char TOK_DELIM[] = " \t\r\n\a";

    
    int arg_cnt = 0, max_argc = 10;
    //char input[BUFSIZE];
    char *input = malloc(sizeof(char)*BUFSIZE);
    char **argv = malloc(max_argc * sizeof(char *));

    // input
    fgets(input, BUFSIZE, stdin);
    if (feof(stdin)) {
        return 0;
    }
    input[strlen(input) - 1] = '\0';

    // strtokは2回目の呼び出しではTOK_DELIMを一切含まない最初の部分をinputから切り出す
    //この際word直後のTOK_DELIM文字は\0に置き換えられる
    char *word = strtok(input, TOK_DELIM);
    argv[arg_cnt++] = word;

    while ((word = strtok(NULL, TOK_DELIM))) {
        argv[arg_cnt++] = word;

        //引数の個数が配列の要素数より多かった時の対処
        if (arg_cnt >= max_argc) {
            max_argc *= 2;

            // reallocは第1引数の内容をコピーした新しい領域を返す.
            argv = realloc(argv, max_argc * sizeof(char *));
            if (argv == NULL) {
                fprintf(stderr, "rcsh: allocation error\n");
                exit(1);
            }
        }
    }

    if (argv[arg_cnt] != NULL) {
        argv = realloc((char**)*argv, (arg_cnt + 2) * sizeof(char *));
        argv[arg_cnt] = NULL;
    }

    *argc = arg_cnt;
    return argv;
}

// gcc -O2でコンパイルするとなぜか動くがフラグがないとargvが壊れる
pid_t execute(char *argv[]) {
    pid_t pid = fork();
    if (pid != 0) return pid;

    execvp(argv[0], argv);
    perror("Child Process Error");
    exit(1);
}

