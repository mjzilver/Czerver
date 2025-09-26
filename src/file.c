#include "file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

char* read_file(const char* path) {
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    if (fseek(file, 0, SEEK_SET) != 0) {
        perror("fseek failed");
        return NULL;
    }

    char* content = malloc(file_size + 1);
    if (content == NULL) {
        fclose(file);
        return NULL;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';

    fclose(file);
    return content;
}

int create_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        printf("Failed to open file for writing: %s\n", path);
        return 1;
    }

    int fput_ret = fputs(content, file);
    if (fput_ret != 0) {
        fclose(file);
        return fput_ret;
    }

    fclose(file);
    return 0;
}