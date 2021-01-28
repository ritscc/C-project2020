#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFSIZE 1024
#define DEFAULT_MAX_ARGC 10
#define TOK_DELIM " \t\r\n\a"

int main(void) {
    char input[BUFSIZE];

    while (1) {
        // prompt & input
        printf("rcsh> ");
        fgets(input, BUFSIZE, stdin);
        input[strlen(input) - 1] = '\0';

        //引数の個数とコマンド保存用文字列配列の宣言
        int argc = 0, max_argc = DEFAULT_MAX_ARGC;
        char **argv = calloc(DEFAULT_MAX_ARGC, sizeof(char *));

        // strtokは1回目の呼び出しではTOK_DELIMを一切含まない最初の部分をinputから切り出す
        //この際word直後のTOK_DELIM文字は\0に置き換えられる
        char *word = strtok(input, TOK_DELIM);

        while (word != NULL) {
            argv[argc++] = word;

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

            // 2回目以降はstrtokが1回目に呼び出した文字列を保持しているためNULLを与える
            word = strtok(NULL, TOK_DELIM);
        }

        if (argc < 1) continue;
        argv[argc] = NULL;

        pid_t pid = fork();
        if (pid == 0) {
            execvp(argv[0], argv);
            perror("child process error:");
            exit(1);
        }

        int status;
        wait(&status);
        printf("Child process exited with status %d\n", WEXITSTATUS(status));

        free(argv);
    }
    return 0;
}