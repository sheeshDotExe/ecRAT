#include "shell.h"
#define MAX_BUFFER_SIZE 1024

Result run_shell_command(char *command){
    char* data = NULL;
    char buffer[MAX_BUFFER_SIZE];
    Result result; 

    FILE* stream = popen(command, "r");

    int current_size = 0;
    if (stream) {
        while (!feof(stream)) {
            int read_bytes = fread(buffer, MAX_BUFFER_SIZE, stream);
            if (read_bytes != NULL){
                data = realloc(data, current_size + read_bytes);
                memcpy(data + current_size, buffer, read_bytes);
                current_size += read_bytes;
            }
        }
        pclose(stream);
    }

    result.data = data;
    result.size = current_size;

    return result;
}