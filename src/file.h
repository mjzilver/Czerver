#ifndef FILE_H
#define FILE_H

#include "route.h"

char *read_file(const char *path);
int create_file(const char *path, const char *content);

#endif
