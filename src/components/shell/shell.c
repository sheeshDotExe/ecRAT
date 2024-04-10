#include "shell.h"
#define MAX_BUFFER_SIZE 1024

Result run_shell_command(char *command){
    char* data = NULL;
    char buffer[MAX_BUFFER_SIZE];
    Result result; 

    printf("Running command: %s\n", command);
    FILE* stream = popen(command, "r");

    int current_size = 0;
    if (stream) {
        printf("Command executed successfully\n");
        while (!feof(stream)) {
            if (fgets(buffer, MAX_BUFFER_SIZE, stream) != NULL){
                int read_bytes = strlen(buffer);
                printf("Read bytes: %d, %s\n", read_bytes, buffer);
                if (read_bytes != NULL){
                    data = realloc(data, current_size + read_bytes);
                    memcpy(data + current_size, buffer, read_bytes);
                    current_size += read_bytes;
                }
            }
        }
        pclose(stream);
    }

    result.data = data;
    result.size = current_size;

    return result;
}