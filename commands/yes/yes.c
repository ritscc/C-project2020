#include <stdio.h>
#include <stdbool.h>

#define EXPLETIVE_MAX_LEN 256

int main(int argc, char *argv[])
{
    char expletive[EXPLETIVE_MAX_LEN + 1] = "y";

    // User Custom Expletive
    if (argc > 1)
    {
        snprintf(expletive, EXPLETIVE_MAX_LEN, "%s", argv[1]);
    }

    while (true)
    {
        puts(expletive);
    }

    return 0;
}
