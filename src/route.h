#ifndef ROUTE_H
#define ROUTE_H

#include "dict.h"
#include "template.h"

// Type headers
#define HTML "text/html"
#define CSS "text/css"
#define JS "application/javascript"

// Type enum
typedef enum FileType {
    HTML_TYPE,
    CSS_TYPE,
    JS_TYPE,
    UNKNOWN_TYPE
} FileType;

typedef struct Route {
    char method[8];
    char path[256];
    char file_path[256];
    char* cached_file;
    FileType type;
    KeyValuePair* replacements;
    size_t replacements_count;
} Route;

extern Dict* routes_dict;

void register_route(const char* method, const char* path, const char* file_path, KeyValuePair* replacements, int replacements_count);
char* get_file_content(const char* path);
void free_routes();
void send_404(char* path, int client);
char* get_type_header(FileType type);

#endif
