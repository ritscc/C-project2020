#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*************/
/*rcsh system*/
/*************/


int rcsh_launch(char** args) {
  pid_t pid, wpid;
  int status;

  pid = fork();
  if (pid == 0) {
    // 子プロセス
    if (execvp(args[0], args) == -1) {
      perror("rcsh");
    }
    exit(-1);
  } else if (pid < 0) {
    // フォークでエラー
    perror("rcsh");
  } else {
    // 親プロセス
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

    return 1;
}

int rcsh_execute(char** args) {
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  return rcsh_launch(args);
}


#define LSH_TOK_BUFSIZE 64
#define DELIM " \t\r\n\a"
char** rcsh_split_line(char* line) {
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char** tokens = malloc(bufsize * sizeof(char*));
  char* token;

  if (!tokens) {
    fprintf(stderr, "rcsh:allocation error\n");
    exit(-1);
  }

  token = strtok(line, DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;
        
    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "rcsh: allocation error\n");
        exit(-1);
      }
    }
    token = strtok(NULL, DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

char* rcsh_read_line(void) {
  char* line = NULL;
  ssize_t bufsize = 0;
  getline(&line, &bufsize, stdin);
  return line;
}


void rcsh(void) {
  char* line;
  char** args;
  int status;

  do {
    printf("rcsh > ");
    line = rcsh_read_line();
    args = rcsh_split_line(line);
    status = rcsh_execute(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char** argv) {
  // Load config files, if any.

  // Run command loop.
  rcsh();

  // Perform any shutdown/cleanup.

  return 1;
}
