#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **read_cmd(int *argc, bool *exec_bg) {
    const int BUFSIZE = 1024;
    const char TOK_DELIM[] = " \t\r\n\a";

    int arg_cnt = 0, max_argc = 10;
    // char input[BUFSIZE];
    char *input = malloc(sizeof(char) * BUFSIZE);
    char **argv = malloc(max_argc * sizeof(char *));

    // input
    printf("rcsh> ");
    if (fgets(input, BUFSIZE, stdin) == NULL) {
        return NULL;
    }
    input[strlen(input) - 1] = '\0';

    // バックグラウンド実行のフラグを設定
    // 同時に引数から&記号を削除
    char *pos_and;
    if ((pos_and = strchr(input, '&')) != NULL) {
        *exec_bg = true;
        *pos_and = '\0';
    }

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

    // execvは最後にNULLが入っていないと動作しないので末尾にNULLを追加
    if (argv[arg_cnt] != NULL) {
        argv = realloc(argv, (arg_cnt + 1) * sizeof(char *));
        argv[arg_cnt] = NULL;
    }

    // 入力が\nなど空白だった場合の処理
    if (argv[0] == NULL) {
        arg_cnt--;
    }

    *argc = arg_cnt;
    return argv;
}

void free_argv(char **argv) {
    free(*argv);
    free(argv);
}
