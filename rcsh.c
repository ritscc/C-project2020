#include <ctype.h>
#include <dirent.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

char **read_cmd(int *, bool *);
pid_t execute(char *, char **);
void sigchld_wait(int);
char **get_internal_cmd(int *);

const int MAX_CMD_LENGTH = 128;

int main(void) {
    // bg実行終了後のリソース回収処理
    if (signal(SIGCHLD, sigchld_wait) == SIG_ERR) {
        fprintf(stderr, "rcsh: signal(SIGCHLD) failed\n");
        exit(1);
    }

    // 組み込みコマンドの取得
    int internal_cmds_cnt;
    char **internal_cmds = get_internal_cmd(&internal_cmds_cnt);

    while (1) {
        printf("rcsh> ");
        int argc = 0;
        bool exec_bg = false;
        char **argv = read_cmd(&argc, &exec_bg);
        if (argc < 1) {
            free(*argv);
            free(argv);
            continue;
        }

        if (strcmp(argv[0], "exit") == 0) break;

        // 組み込みコマンドかどうかを判定してコマンド名を生成
        char *cmd = malloc(sizeof(char) * MAX_CMD_LENGTH);
        memset(cmd, 0, sizeof(char) * MAX_CMD_LENGTH);
        if (internal_cmds != NULL) {
            for (int i = 0; i < internal_cmds_cnt; i++) {
                if (strcmp(argv[0], internal_cmds[i]) == 0) {
                    snprintf(cmd, MAX_CMD_LENGTH, "./commands/%s/%s",
                             internal_cmds[i], internal_cmds[i]);
                    break;
                }
            }
        }
        if (cmd[0] == '\0') {
            free(cmd);
            cmd = argv[0];
        }

        pid_t pid = execute(cmd, argv);
        int status;
        if (!exec_bg) {
            wait(&status);
            // printf("Exited with status %d\n", WEXITSTATUS(status));
        }
        free(*argv);
        free(argv);
    }
    return 0;
}

char **read_cmd(int *argc, bool *exec_bg) {
    const int BUFSIZE = 1024;
    const char TOK_DELIM[] = " \t\r\n\a";

    int arg_cnt = 0, max_argc = 10;
    // char input[BUFSIZE];
    char *input = malloc(sizeof(char) * BUFSIZE);
    char **argv = malloc(max_argc * sizeof(char *));

    // input
    fgets(input, BUFSIZE, stdin);
    if (feof(stdin)) {
        return 0;
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

    if (argv[arg_cnt] != NULL) {
        argv = realloc(argv, (arg_cnt + 2) * sizeof(char *));
        argv[arg_cnt] = NULL;
    }

    if (argv[0] == NULL) {
        arg_cnt--;
    }
    *argc = arg_cnt;

    return argv;
}

pid_t execute(char *cmd, char *argv[]) {
    pid_t pid = fork();
    if (pid != 0) return pid;

    execvp(cmd, argv);
    perror("Child Process Error");
    exit(1);
}

void sigchld_wait(int sig) {
    int status;
    waitpid(-1, &status, WNOHANG);
    printf("process exited with status %d\n", WEXITSTATUS(status));
}

char **get_internal_cmd(int *cmd_num) {
    const int MAX_CMD_COUNT = 32;
    char **commands = calloc(MAX_CMD_COUNT, sizeof(char *));
    int cmd_cnt = 0;

    DIR *dir = opendir("./commands");
    if (dir == NULL) {
        fprintf(stderr, "unable to opendir ./commands\n");
        exit(1);
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue;
        }
        int len = strlen(entry->d_name);
        if (len > MAX_CMD_LENGTH) {
            len = MAX_CMD_LENGTH;
        }
        char *cmd = calloc(len, sizeof(char));

        strncpy(cmd, entry->d_name, len);
        commands[cmd_cnt++] = cmd;
        printf("found: %s\n", entry->d_name);
    }

    closedir(dir);
    *cmd_num = cmd_cnt;
    return commands;
}
