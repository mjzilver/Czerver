#include "file.h"
#include "http_codes.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

char *read_file(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return NULL;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *content = malloc(file_size + 1);
    if (content == NULL)
    {
        fclose(file);
        return NULL;
    }

    fread(content, 1, file_size, file);
    content[file_size] = '\0';

    fclose(file);
    return content;
}

int send_file(char *path, int client)
{
    FILE *file = fopen(path, "r");
    if (file == NULL)
    {
        return 0;
    }

    // Find the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *response = malloc(strlen(ok_200) + file_size + 1);
    if (response == NULL)
    {
        fclose(file);
        return 0;
    }

    strcpy(response, ok_200);
    fread(response + strlen(ok_200), 1, file_size, file);
    fclose(file);

    send(client, response, strlen(ok_200) + file_size, 0);
    free(response);
    return 1;
}
