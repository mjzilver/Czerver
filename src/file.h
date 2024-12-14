#ifndef FILE_H
#define FILE_H

char *read_file(const char *path);
int send_file(char *path, int client);

#endif
