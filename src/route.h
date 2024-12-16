#ifndef ROUTE_H
#define ROUTE_H

#include "template.h"

typedef struct {
    char method[8];
    char path[256];
    char file_path[256];
    char *cached_file;
    KeyValuePair *replacements;
    size_t replacements_count;
} Route;

#define MAX_ROUTES 100
extern Route routes[MAX_ROUTES];
extern int routes_count;

void register_route(const char *method, const char *path, const char *file_path, KeyValuePair *replacements, int replacements_count);
int try_send_file(char *path, int client);
char *get_file_content(const char *path);
void free_routes();
void send_404(char *path, int client);

#endif
