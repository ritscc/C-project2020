#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void) {
    // argvはNULL Terminateである必要がある
    char* cmd[] = {"/bin/sleep", "3", NULL};
    pid_t pid = fork();
    if (pid == 0) {
        puts("I am child proc");
        execvp(cmd[0], cmd);
        perror("child process error:");
        exit(1);
    }

    int status;
    wait(&status);
    printf("Child process exited with status %d\n", WEXITSTATUS(status));

    return 0;
}