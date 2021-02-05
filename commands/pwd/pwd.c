#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    const int BUFSIZE = 1024;
    char* dirname = getcwd(NULL, BUFSIZE);
    puts(dirname);
}
