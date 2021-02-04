#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **get_internal_cmd(int *cmd_num) {
    const int MAX_CMD_COUNT = 32;
    const int MAX_CMD_LENGTH = 128;
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

char *check_inner_cmd(int cmd_cnt, char **cmds, char **argv) {
    if (cmds == NULL) {
        return argv[0];
    }

    const int MAX_CMD_LENGTH = 128;
    char *cmd = malloc(sizeof(char) * MAX_CMD_LENGTH);
    memset(cmd, 0, sizeof(char) * MAX_CMD_LENGTH);

    for (int i = 0; i < cmd_cnt; i++) {
        if (strcmp(argv[0], cmds[i])) {
            continue;
        }
        snprintf(cmd, MAX_CMD_LENGTH, "./commands/%s/%s", cmds[i], cmds[i]);
        break;
    }
    if (cmd[0] == '\0') {
        free(cmd);
        cmd = argv[0];
    }

    return cmd;
}

void free_inner_cmds(int cnt, char **cmds) {
    for (int i = 0; i < cnt; i++) {
        free(cmds[i]);
    }
    free(cmds);
}
