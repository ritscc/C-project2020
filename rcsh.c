#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/**************************************************/ 
/*Function declarations for builtin shell commands*/
/**************************************************/
int rcsh_cd(char** args);
int rcsh_help(char** args);
int rcsh_exit(char** args);

/********************************************************************/ 
/*List of builtin commands followed by thier corresponding functions*/
/********************************************************************/
char* builtin_str[]= {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[])(char**) = {
  &rcsh_cd,
  &rcsh_help,
  &rcsh_exit
};

int rcsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

/*********************************/ 
/*Builtin function implemetations*/
/*********************************/
int rcsh_cd(char** args) {
  if (args[1] == NULL) {
    fprintf(stderr, "rcsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("rcsh");
    }
  }

  return 1;
}

int rcsh_help(char** args) {
  int i;
  printf("RCSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in: \n");

  for (i = 0; i < rcsh_num_builtins(); i++) {
    printf(" %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");

  return 1;
}

int rcsh_exit(char** args) {
  return 0;
}

int rcsh_execute(char** args) {
  int i;

  if (args[0] == NULL) {
    return 1;
  }

  for (i = 0; i < rcsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return rcsh_launch(args);
}


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

#define LSH_RL_BUFSIZE 1024
// char *rcsh_read_line(void) {
//     int bufsize = LSH_RL_BUFSIZE;
//     int position = 0;
//     char *buffer = malloc(sizeof(char) * bufsize);
//     int c;

//     if (!buffer) {
//         fprintf(stderr, "rcsh: allocation error\n");
//         exit(-1);
//     }

//     while (1) {
//         c = getchar();

//         // 終了まできたらnullにして終わり
//         if (c == EOF || c == '\n') {
//             buffer[position] = '\0';
//             return buffer;
//         } else {
//             buffer[position] = c;
//         }
//         position++;

//         // もしbufferを超えた時
//         if (position >= bufsize) {
//             bufsize += LSH_RL_BUFSIZE;
//             buffer = realloc(buffer, bufsize);
//             if (!buffer) {
//                 fprintf(stderr, "rcsh: allocation error\n");
//                 exit(-1);
//             }
//         }
//     }
// }

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
