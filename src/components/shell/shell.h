#include <stdio.h>
#include <stdlib.h>

typedef struct Result {
    char* data;
    int size;
} Result;

Result run_shell_command(char *command);