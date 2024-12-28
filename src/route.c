#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "file.h"
#include "http_codes.h"

Route routes[MAX_ROUTES];
int routes_count = 0;

void register_route(const char* method, const char* path, const char* file_path, KeyValuePair* replacements, int replacements_count) {
    Route route;
    strcpy(route.method, method);
    strcpy(route.path, path);
    strcpy(route.file_path, file_path);
    route.cached_file = get_file_content(file_path);
    route.replacements = replacements;
    route.replacements_count = replacements_count;

    if (strstr(file_path, ".html") != NULL) {
        route.type = HTML_TYPE;
    } else if (strstr(file_path, ".css") != NULL) {
        route.type = CSS_TYPE;
    } else if (strstr(file_path, ".js") != NULL) {
        route.type = JS_TYPE;
    } else {
        route.type = UNKNOWN_TYPE;
    }

    if (route.cached_file == NULL) {
        perror("Failed to load file content");
        exit(1);
    }
    routes[routes_count++] = route;
}

char* get_type_header(FileType type) {
    switch (type) {
    case HTML_TYPE:
        return "text/html";
    case CSS_TYPE:
        return "text/css";
    case JS_TYPE:
        return "application/javascript";
    default:
        return "text/plain";
    }
}

char* get_file_content(const char* path) {
    return read_file(path);
}

void free_routes() {
    for (int i = 0; i < routes_count; i++) {
        if (routes[i].cached_file != NULL) {
            free(routes[i].cached_file);
        }
    }
}

void send_404(char* path, int client) {
    char* response = malloc(strlen(HTTP_404) + strlen(path) + 1);
    if (response == NULL)
        return;

    strcpy(response, HTTP_404);
    strcat(response, path);

    send(client, response, strlen(response), 0);
    free(response);
}
