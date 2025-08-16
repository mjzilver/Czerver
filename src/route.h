#ifndef ROUTE_H
#define ROUTE_H

#include "dict.h"
#include "template.h"

#define HTML "text/html"
#define CSS "text/css"
#define JS "application/javascript"

typedef enum FileType { HTML_TYPE, CSS_TYPE, JS_TYPE, UNKNOWN_TYPE } FileType;

typedef struct Route {
    char method[8];
    char url_path[256];
    char file_path[256];
    char *cached_file;
    FileType type;
    KeyValuePair *replacements;
} Route;

extern Dict *routes_dict;

void register_route(const char *method, const char *url_path, const char *file_path, KeyValuePair *replacements);
void register_folder(const char *url_path, const char *file_path, KeyValuePair *replacements);
void unregister_route(const char *url_path);
char *get_file_content(const char *path);
void free_routes();
void send_404(char *path, int client);
char *get_type_header(FileType type);

#endif
