#include "file.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

static int file_error(FILE* file, const char* filename, const char* msg) {
    int saved_errno = errno;
    if (file) {
        fclose(file);
    }
    fprintf(stderr, "Error %s '%s': %s\n", msg, filename, strerror(saved_errno));
    return -1;
}

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        file_error(NULL, filename, "opening file");
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        file_error(file, filename, "seeking to end");
        return NULL;
    }

    long file_size = ftell(file);
    if (file_size < 0) {
        file_error(file, filename, "getting file size");
        return NULL;
    }

    errno = 0;
    rewind(file);
    if (errno != 0) {
        file_error(file, filename, "rewinding file");
        return NULL;
    }

    size_t size = (size_t)file_size;
    char* buffer = malloc(size + 1);
    if (!buffer) {
        file_error(file, filename, "allocating memory");
        return NULL;
    }

    size_t read_bytes = fread(buffer, 1, size, file);
    if (read_bytes != size) {
        free(buffer);
        file_error(file, filename, "reading file");
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int create_file(const char* path, const char* content) {
    FILE* file = fopen(path, "w");
    if (!file) {
        return file_error(file, path, "opening file");
    }

    if (fputs(content, file) == EOF) {
        return file_error(file, path, "writing to file");
    }

    fclose(file);
    return 0;
}