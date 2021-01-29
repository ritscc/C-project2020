#include <stdio.h>
#include <string.h>

static void unescape(char *p){
    for (; *p; p++){
        if(p != '\\' || p[1] == '\0' ){
            printf("%c", *p);
            continue;
        }
        p++;
        if (*p == 'n')
            printf("\n");
        else if (*p == 't')
            printf("\t");
        else printf("\\%c", *p);
    }
}

int main(int argc, char **argv) {
    if(argc == 1)
        return 0;

    char **args = argv + 1;
    if (strcmp(args[0], "-e") == 0){
        args++;
    for (char **p = args; *p; p++){
        if (p != args)
            printf(" ");
        unescape(*p);
    }
    } else {
    for(char **p = args; *p; p++)
        printf("%s%s", (p == args ? "" : ""), *p);
}
}
