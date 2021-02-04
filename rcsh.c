#include <ctype.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "inner_cmds.c"
#include "read_cmd.c"

pid_t execute(char *, char **);
void sigchld_wait(int);

int main(void) {
    // bg実行終了後のリソース回収処理
    if (signal(SIGCHLD, sigchld_wait) == SIG_ERR) {
        fprintf(stderr, "rcsh: signal(SIGCHLD) failed\n");
        exit(1);
    }

    int argc = 0, status;
    bool exec_bg = false;
    char **argv;
    pid_t pid;

    // 組み込みコマンドの取得
    int inner_cmds_cnt;
    char **inner_cmds = get_internal_cmd(&inner_cmds_cnt);

    while (1) {
        // read
        if ((argv = read_cmd(&argc, &exec_bg)) == NULL) {
            break;
        }

        // no input
        if (argc < 1) {
            free_argv(argv);
            continue;
        }

        // exit
        if (strcmp(argv[0], "exit") == 0) {
            free_argv(argv);
            break;
        }

        // update cmdpath if internal command
        char *cmd = check_inner_cmd(inner_cmds_cnt, inner_cmds, argv);

        // execute
        pid = execute(cmd, argv);

        // wait
        if (!exec_bg) {
            wait(&status);
        }

        free_argv(argv);
    }

    free_inner_cmds(inner_cmds_cnt, inner_cmds);
    return 0;
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
