#include "route.h"

#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "dict.h"
#include "file.h"
#include "http_codes.h"

Dict *routes_dict = NULL;

char *join_url_path(const char *url_path, const char *name) {
    size_t len;
    char *result;

    if (strcmp(url_path, "/") == 0) {
        len = 1 + strlen(name) + 1;
        result = malloc(len);
        snprintf(result, len, "/%s", name);
    } else {
        len = strlen(url_path) + 1 + strlen(name) + 1;
        result = malloc(len);
        snprintf(result, len, "%s/%s", url_path, name);
    }

    return result;
}

void register_folder(const char *url_path, const char *file_path, KeyValuePair *replacements) {
    DIR *dir = opendir(file_path);
    assert(dir != NULL);

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        size_t full_len = strlen(file_path) + 1 + strlen(entry->d_name) + 1;
        char *full_path = malloc(full_len);
        snprintf(full_path, full_len, "%s/%s", file_path, entry->d_name);

        struct stat st;
        assert(stat(full_path, &st) != -1);

        if (S_ISDIR(st.st_mode)) {
            char *sub_url_path = join_url_path(url_path, entry->d_name);
            printf("Recursively registering subfolder %s as %s\n", full_path, sub_url_path);
            register_folder(sub_url_path, full_path, replacements);
            free(sub_url_path);
        } else if (S_ISREG(st.st_mode)) {
            char *file_url_path = join_url_path(url_path, entry->d_name);
            printf("Registering file: %s as URL path %s\n", full_path, file_url_path);
            register_route("GET", file_url_path, full_path, replacements);

            if (strcmp(entry->d_name, "index.html") == 0) {
                char *dir_url_path;
                if (strcmp(url_path, "/") == 0)
                    dir_url_path = strdup("/");
                else {
                    size_t len = strlen(url_path) + 2;
                    dir_url_path = malloc(len);
                    snprintf(dir_url_path, len, "%s/", url_path);
                }

                printf("Registering index for %s\n", dir_url_path);
                register_route("GET", dir_url_path, full_path, replacements);
                free(dir_url_path);
            }

            free(file_url_path);
        }

        free(full_path);
    }

    closedir(dir);
}

void register_route(const char *method, const char *url_path, const char *file_path, KeyValuePair *replacements) {
    Route *route = malloc(sizeof(Route));
    assert(route != NULL);

    strcpy(route->method, method);
    strcpy(route->url_path, url_path);
    strcpy(route->file_path, file_path);
    route->cached_file = get_file_content(file_path);
    route->replacements = replacements;

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

    unregister_route(url_path);
    dict_set(routes_dict, url_path, route);
}

void free_route_callback(const char *key, void *value) {
    Route *route = value;
    if (route->cached_file) free(route->cached_file);
    free(route);
}

void unregister_route(const char *url_path) {
    Route *route = DICT_GET_AS(Route, routes_dict, url_path);
    if (route != NULL) {
        free_route_callback(url_path, route);
    }
}

void unregister_all_routes() {
    if (!routes_dict) return;

    dict_iterate(routes_dict, free_route_callback);
    dict_free_all(routes_dict);
    routes_dict = NULL;
}

char *get_type_header(FileType type) {
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

char *get_file_content(const char *path) { return read_file(path); }

void send_404(char *path, int client) {
    char *response = malloc(strlen(HTTP_404) + strlen(path) + 1);
    if (response == NULL) return;

    strcpy(response, HTTP_404);
    strcat(response, path);

    Route *route = DICT_GET_AS(Route, routes_dict, "/404");
    if (route != NULL) {
        char *file_content = process_template(route->cached_file, route->replacements);
        response = realloc(response, strlen(response) + strlen(file_content) + 1);
        assert(response != NULL);
        strcat(response, file_content);
        free(file_content);
    }

    send(client, response, strlen(response), 0);
    free(response);
}
