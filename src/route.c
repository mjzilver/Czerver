#include "route.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <assert.h> 

#include "file.h"
#include "http_codes.h"
#include "dict.h"

Dict* routes_dict = NULL;

void register_route(const char* method, const char* path, const char* file_path, KeyValuePair* replacements, int replacements_count) {
    Route* route = malloc(sizeof(Route));
    assert(route != NULL);    

    strcpy(route->method, method);
    strcpy(route->path, path);
    strcpy(route->file_path, file_path);
    route->cached_file = get_file_content(file_path);
    route->replacements = replacements;
    route->replacements_count = replacements_count;

    if (strstr(file_path, ".html") != NULL) {
        route->type = HTML_TYPE;
    } else if (strstr(file_path, ".css") != NULL) {
        route->type = CSS_TYPE;
    } else if (strstr(file_path, ".js") != NULL) {
        route->type = JS_TYPE;
    } else {
        route->type = UNKNOWN_TYPE;
    }

    if (route->cached_file == NULL) {
        perror("Failed to load file content");
        exit(1);
    }

    if (routes_dict == NULL) {
        routes_dict = dict_new(INITIAL_DICT_CAPACITY);
    }

    dict_set(routes_dict, path, route);
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
    if (routes_dict == NULL) {
        return;
    }
    dict_free(routes_dict);
}

void send_404(char* path, int client) {
    char* response = malloc(strlen(HTTP_404) + strlen(path) + 1);
    if (response == NULL)
        return;

    strcpy(response, HTTP_404);
    strcat(response, path);

    // Check for optional 404 route
    Route* route = DICT_GET_AS(Route, routes_dict, "/404");
    if (route != NULL) {
        char* file_content = process_template(route->cached_file, route->replacements);
        response = realloc(response, strlen(response) + strlen(file_content) + 1);
        assert(response != NULL);
        strcat(response, file_content);
        free(file_content);
    }

    send(client, response, strlen(response), 0);
    free(response);
}
