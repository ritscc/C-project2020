#include <fcntl.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PATHNAME_SIZE 256

void create_pathname(const char *filename, char *pathname);

void option_c(char *argv[]);
int space_judge(char *format[], char *buf);
void option_t(char *pathname);

void illegal_option_err(char option);
void need_argument_err(char option);

void print_all(char *pathname);
void print_file_name(char *pathname);
void print_file_size(struct stat file_info);
void print_file_blocks(struct stat file_info);
void print_file_ioblocks(struct stat file_info);
void print_file_type(struct stat file_info);
void print_device(struct stat file_info);
void print_inode(struct stat file_info);
void print_links(struct stat file_info);
void print_permission(struct stat file_info);
void print_user_info(struct stat file_info);
void print_group_info(struct stat file_info);
void print_access_time(struct stat file_info);
void print_modify_time(struct stat file_info);
void print_change_time(struct stat file_info);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    // stdinの情報だけどエラーで処理
    fprintf(stderr, "usage: ./stat [-L] [-c 書式] [-t] パス名 ...\n");
    exit(EXIT_FAILURE);
  }

  if (argc == 2) {
    if (strncmp("-", argv[argc - 1], 1) == 0) {
      char opt;
      strncpy(&opt, ++argv[argc - 1], 1);
      switch (opt) {
      case 'c':
      case 'L':
      case 't':
        need_argument_err(opt);
        break;
      default:
        illegal_option_err(opt);
        break;
      }
      exit(EXIT_FAILURE);
    } else {
      char pathname[256];
      create_pathname(argv[argc - 1], pathname);
      print_all(pathname);
      return 0;
    }
  }

  if (argc > 2) {
    if (strncmp("-", argv[1], 1) != 0) {
      char pathname[argc - 1][256];
      for (int i = 0; i < argc - 1; i++) {
        create_pathname(argv[i + 1], pathname[i]);
        print_all(pathname[i]);
      }
      exit(EXIT_SUCCESS);
    } else {
      char pathname[256];
      char opt;
      strncpy(&opt, ++argv[1], 1);
      switch (opt) {
      case 'L':
        printf("tyottomuridesita");
        break;
      case 't':
        create_pathname(argv[argc - 1], pathname);
        option_t(pathname);
        break;
      case 'c':
        if (argc < 4) {
          need_argument_err(opt);
        } else {
          option_c(argv);
        }
        break;
      default:
        illegal_option_err(opt);
        break;
      }
      exit(EXIT_SUCCESS);
    }
  }
  return 0;
}
void option_c(char *argv[]) {
  if (strncmp("%", argv[2], 1) != 0) {
    need_argument_err('c');
    return;
  }
  char pathname[256];
  create_pathname(argv[3], pathname);
  struct stat file_info;
  if (stat(pathname, &file_info) == -1) {
    fprintf(stderr, "stat: %s: ", pathname);
    perror("stat");
    exit(EXIT_FAILURE);
  }

  char *format[13];
  int format_num = space_judge(format, argv[2]);
  for (int i = 0; i < format_num; i++) {
    char fmt;
    strncpy(&fmt, ++format[i], 1);
    switch (fmt) {
    case 'a':
      print_permission(file_info);
      break;
    case 'b':
      print_file_blocks(file_info);
      break;
    case 'd':
      print_device(file_info);
      break;
    case 'g':
      print_group_info(file_info);
      break;
    case 'h':
      print_links(file_info);
      break;
    case 'i':
      print_inode(file_info);
      break;
    case 'n':
      print_file_name(argv[4]);
      break;
    case 'o':
      print_file_ioblocks(file_info);
      break;
    case 's':
      print_file_size(file_info);
      break;
    case 'u':
      print_user_info(file_info);
      break;
    case 'x':
      print_access_time(file_info);
      break;
    case 'y':
      print_modify_time(file_info);
      break;
    case 'z':
      print_change_time(file_info);
      break;
    default:
      printf("0");
      break;
    }
  }
  printf("\n");
}
void option_t(char *pathname) {
  struct stat file_info;
  if (stat(pathname, &file_info) == -1) {
    fprintf(stderr, "stat: %s: ", pathname);
    perror("stat");
    exit(EXIT_FAILURE);
  }
  print_file_name(pathname);
  print_file_size(file_info);
  print_file_blocks(file_info);
  print_user_info(file_info);
  print_group_info(file_info);
  print_device(file_info);
  print_inode(file_info);
  print_links(file_info);
  print_access_time(file_info);
  print_modify_time(file_info);
  print_change_time(file_info);
  print_file_ioblocks(file_info);
  return;
}

void need_argument_err(char option) {
  fprintf(stderr, "stat: option requires an argument -- %c\n", option);
  fprintf(stderr, "usage: ./stat [-L] [-c format] [-t] file ...\n");
}
void illegal_option_err(char option) {
  fprintf(stderr, "stat: illegal option -- %c\n", option);
  fprintf(stderr, "usage: ./stat [-L] [-c format] [-t] file ...\n");
}

void create_pathname(const char *filename, char *pathname) {
  if (strncmp("/", filename, 1) != 0) {
    getcwd(pathname, 256);
  }
  strncat(pathname, "/", 1);
  strncat(pathname, filename, 128);
}
void print_all(char *pathname) {
  struct stat file_info;
  if (stat(pathname, &file_info) == -1) {
    fprintf(stderr, "stat: %s: ", pathname);
    perror("stat");
    exit(EXIT_FAILURE);
  }
  printf("File:");
  print_file_name(pathname);
  printf("\n");
  printf("Size:");
  print_file_size(file_info);
  printf("Blocks:");
  print_file_blocks(file_info);
  printf("IO Block:");
  print_file_ioblocks(file_info);
  print_file_type(file_info);
  printf("\n");
  printf("Device:");
  print_device(file_info);
  printf("Inode:");
  print_inode(file_info);
  printf("Links:");
  print_links(file_info);
  printf("\n");
  printf("Access:");
  print_permission(file_info);
  printf("Uid:");
  print_user_info(file_info);
  printf("Gid:");
  print_group_info(file_info);
  printf("\n");
  printf("Access:");
  print_access_time(file_info);
  printf("\n");
  printf("Modify:");
  print_modify_time(file_info);
  printf("\n");
  printf("Change:");
  print_change_time(file_info);
  printf("\n");
}

void print_file_name(char *pathname) { printf("\'%s\' ", pathname); }
void print_file_size(struct stat file_info) {
  printf("%lld ", (long long)file_info.st_size);
}
void print_file_blocks(struct stat file_info) {
  printf("%lld ", (long long)file_info.st_blocks);
}
void print_file_ioblocks(struct stat file_info) {
  printf("%d ", file_info.st_blksize);
}
void print_file_type(struct stat file_info) {
  switch (file_info.st_mode & S_IFMT) {
  case S_IFBLK:
    printf("block device");
    break;
  case S_IFCHR:
    printf("character device");
    break;
  case S_IFDIR:
    printf("directory");
    break;
  case S_IFIFO:
    printf("FIFO/pipe");
    break;
  case S_IFLNK:
    printf("symlink");
    break;
  case S_IFREG:
    printf("regular file");
    break;
  case S_IFSOCK:
    printf("socket");
    break;
  default:
    printf("unknown?");
    break;
  }
}
void print_device(struct stat file_info) { printf("%d ", file_info.st_dev); }
void print_inode(struct stat file_info) {
  printf("%llu ", (long long)file_info.st_ino);
}
void print_links(struct stat file_info) { printf("%d ", file_info.st_nlink); }
void print_permission(struct stat file_info) {
  // file mode
  switch (file_info.st_mode & S_IFMT) {
  case S_IFDIR:
    printf("d");
    break;
  case S_IFLNK:
    printf("l");
    break;
  default:
    printf("-");
    break;
  }
  // user permission rwx
  // printf("Mode:%o ", (unsigned)file_info.st_mode);
  switch (file_info.st_mode & S_IRWXU) {
  case 0000700:
    printf("rwx");
    break;
  case 0000600:
    printf("rw-");
    break;
  case 0000500:
    printf("r-x");
    break;
  case 0000300:
    printf("-wx");
    break;
  case S_IRUSR:
    printf("r--");
    break;
  case S_IWUSR:
    printf("-w-");
  case S_IXUSR:
    printf("--x");
  default:
    printf("---");
    break;
  }
  // group permission
  switch (file_info.st_mode & S_IRWXG) {
  case 0000070:
    printf("rwx");
    break;
  case 0000060:
    printf("rw-");
    break;
  case 0000050:
    printf("r-x");
    break;
  case 0000030:
    printf("-wx");
    break;
  case S_IRGRP:
    printf("r--");
    break;
  case S_IWGRP:
    printf("-w-");
    break;
  case S_IXGRP:
    printf("--x");
    break;
  default:
    printf("---");
    break;
  }
  // other permission
  switch (file_info.st_mode & S_IRWXO) {
  case 0000007:
    printf("rwx");
    break;
  case 0000006:
    printf("rw-");
    break;
  case 0000005:
    printf("r-x");
    break;
  case 0000003:
    printf("-wx");
    break;
  case S_IROTH:
    printf("r--");
    break;
  case S_IWOTH:
    printf("-w-");
    break;
  case S_IXOTH:
    printf("--x");
    break;
  default:
    printf("---");
    break;
  }
  printf(" ");
}
void print_user_info(struct stat file_info) {
  struct passwd *user_name = getpwuid(file_info.st_uid);
  printf("%s ", user_name->pw_name);
}
void print_group_info(struct stat file_info) {
  struct group *group_name = getgrgid(file_info.st_gid);
  printf("%s ", group_name->gr_name);
}
void print_access_time(struct stat file_info) {
  struct tm *last_access = localtime(&(file_info.st_atime));
  char *time = asctime(last_access);
  time[strlen(time) - 1] = '\0';
  printf("%s ", time);
}
void print_modify_time(struct stat file_info) {
  struct tm *last_modify = localtime(&(file_info.st_mtime));
  char *time = asctime(last_modify);
  time[strlen(time) - 1] = '\0';
  printf("%s ", time);
}
void print_change_time(struct stat file_info) {
  struct tm *last_change = localtime(&(file_info.st_ctime));
  char *time = asctime(last_change);
  time[strlen(time) - 1] = '\0';
  printf("%s ", time);
}
int space_judge(char *format[], char *buf) {
  int i;
  for (i = 0; *buf != '\0'; i++) {
    while (*buf == ' ') {
      *buf = '\0';
      buf++;
    }
    if (*buf == '\0') {
      break;
    }
    format[i] = buf;
    while (*buf != '\0' && *buf != ' ') {
      buf++;
    }
  }
  format[i] = NULL;
  return i;
}