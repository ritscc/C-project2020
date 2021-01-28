#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int read_cmd(char ***);
pid_t execute(char *argv[]);

int main(void) {
    char **argv;

    while (1) {
        printf("rcsh> ");
        if (read_cmd(&argv) < 1) continue;
        pid_t pid = execute(argv);
        int status;
        wait(&status);
        printf("Exited with status %d\n", WEXITSTATUS(status));
        free(argv);
    }
    return 0;
}

int read_cmd(char ***argv) {
    const int BUFSIZE = 1024;
    const char TOK_DELIM[] = " \t\r\n\a";

    int argc = 0, max_argc = 10;
    char input[BUFSIZE];
    *argv = (char **)calloc(max_argc, sizeof(char *));

    // input
    fgets(input, BUFSIZE, stdin);
    if (feof(stdin)) {
        return 0;
    }
    input[strlen(input) - 1] = '\0';

    // strtokは2回目の呼び出しではTOK_DELIMを一切含まない最初の部分をinputから切り出す
    //この際word直後のTOK_DELIM文字は\0に置き換えられる
    char *word = strtok(input, TOK_DELIM);
    (*argv)[argc++] = word;

    while ((word = strtok(NULL, TOK_DELIM))) {
        (*argv)[argc++] = word;

        //引数の個数が配列の要素数より多かった時の対処
        if (argc >= max_argc) {
            max_argc *= 2;

            // reallocは第1引数の内容をコピーした新しい領域を返す.
            argv = realloc(argv, max_argc * sizeof(char *));
            if (argv == NULL) {
                fprintf(stderr, "rcsh: allocation error\n");
                exit(1);
            }
        }
    }

    if ((*argv)[argc] != NULL) {
        argv = realloc(argv, (argc + 2) * sizeof(char *));
        (*argv)[argc] = NULL;
    }

    return argc;
}

// gcc -O2でコンパイルするとなぜか動くがフラグがないとargvが壊れる
pid_t execute(char *argv[]) {
    pid_t pid;
    pid = fork();
    if (pid != 0) return pid;

    execvp(argv[0], argv);
    perror("Child Process Error");
    exit(1);
}
