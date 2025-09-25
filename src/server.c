#include "server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "dict.h"
#include "globals.h"
#include "http_codes.h"
#include "lua.h"
#include "route.h"
#include "template.h"

extern Dict *var_dict;

struct sockaddr_in addr;

struct sockaddr_in *create_address(int port) {
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return &addr;
}

void *start_server_wrapper(void *args) {
    ServerArgs *server_args = (ServerArgs *)args;
    int port = server_args->port;
    int *success_flag = server_args->success_flag;

    *success_flag = (start_server(port) == 0);

    free(server_args);
    return NULL;
}

int start_server(int port) {
    if (port < 1024) {
        perror("Port number must be greater than 1024");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Failed to create socket");
        return 1;
    }

    struct sockaddr_in *adrr = create_address(port);

    if (bind(sock, (struct sockaddr *)adrr, sizeof(*adrr)) < 0) {
        perror("Failed to bind the socket");
        return 1;
    }

    if (listen(sock, 5) < 0) {
        perror("Failed to listen to the socket");
        return 1;
    }

    printf("Server started on port %d\n", port);

    while (1) {
        int client = accept(sock, NULL, NULL);
        if (client < 0) {
            perror("Failed to accept the connection");
            continue;
        }

        serve_routes(client);
        close(client);
    }

    close(sock);
    printf("Server closed\n");
    return 0;
}

static char *extract_path(const char *buffer) {
    char path[256];
    sscanf(buffer, "%*s %s HTTP/1.1", path);
    return strdup(path);
}

static void send_response(int client, const char *body, const char *content_type) {
    if (!body) body = "";
    if (!content_type) content_type = "text/plain";

    size_t body_len = strlen(body);
    char response[8192] = {0};

    snprintf(response, sizeof(response),
             HTTP_200
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "\r\n"
             "%s",
             content_type, body_len, body);

#if DEBUG
    printf("Sending response:\n%s\n", response);
#endif

    send(client, response, strlen(response), 0);
}

static void send_redirect(int client, const char *url) {
    char response[512];
    snprintf(response, sizeof(response), "%sLocation: %s\r\n\r\n", HTTP_302, url);

#if DEBUG
    printf("Sending redirect:\n%s\n", response);
#endif

    send(client, response, strlen(response), 0);
}

static void handle_static_route_request(int client, const char *buffer, const char *method, Route *route) {
    if (strcmp(method, "GET") == 0) {
        char *file_content = NULL;

        if (route->type == LUA_TYPE) {
            char *response_body = execute_lua(route->cached_file, NULL);
            file_content = process_template(response_body);
            send_response(client, file_content, get_type_header(HTML_TYPE));
        } else {
            file_content = process_template(route->cached_file);
            char *type_header = get_type_header(route->type);
            send_response(client, file_content, type_header);
        }
        free(file_content);
    } else if (strcmp(method, "POST") == 0) {
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) body += 4;
        char *response_body = execute_lua(route->cached_file, body);

        if (strncmp(response_body, "REDIRECT:", 9) == 0) {
            const char *url = response_body + 9;
            send_redirect(client, url);
        } else {
            send_response(client, response_body, "text/html");
        }

        send_response(client, response_body, "text/html");
        free(response_body);
    }
}

static void handle_api_route_request(int client, const char *buffer, const char *method, Route *route) {
    (void)method;
    char *body = strstr(buffer, "\r\n\r\n");
    if (body) body += 4;

    char *response_json = route->handler(body);
    send_response(client, response_json, "application/json");
    free(response_json);
}

void send_404(char *path, int client) {
    char *body = NULL;
    Route *route = DICT_GET_AS(Route, routes_dict, "/404.html");
    if (route != NULL) {
        dict_set(var_dict, "current_path", strdup(path));
        body = process_template(route->cached_file);
    } else {
        const char *default_body_fmt = "<h1>404 Not Found</h1><p>%s not found</p>";
        size_t needed = snprintf(NULL, 0, default_body_fmt, path) + 1;
        body = malloc(needed);
        snprintf(body, needed, default_body_fmt, path);
    }

    size_t body_len = strlen(body);

    char header[512];
    int header_len = snprintf(header, sizeof(header),
                              HTTP_404
                              "Content-Type: text/html\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n"
                              "Access-Control-Allow-Origin: *\r\n"
                              "\r\n",
                              body_len);

    send(client, header, header_len, 0);
    send(client, body, body_len, 0);

#if DEBUG
    printf("404 for path %s\n", path);
#endif

    free(body);
}

static void handle_request(int client, const char *buffer, const char *method) {
    char *path = extract_path(buffer);
    Route *route = DICT_GET_AS(Route, routes_dict, path);

    if (!route || strcmp(route->method, method) != 0) {
        send_404(path, client);
        free(path);
        return;
    }

    if (route->kind == STATIC_ROUTE) {
        handle_static_route_request(client, buffer, method, route);
    } else if (route->kind == API_ROUTE) {
        handle_api_route_request(client, buffer, method, route);
    }
    free(path);
}

void serve_routes(int client) {
    char buffer[1024];
    int bytes_received = recv(client, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) return;

    buffer[bytes_received] = '\0';

#if DEBUG
    printf("Received:\n%s\n", buffer);
#endif

    if (strncmp(buffer, "GET", 3) == 0) {
        handle_request(client, buffer, "GET");
    } else if (strncmp(buffer, "POST", 4) == 0) {
        handle_request(client, buffer, "POST");
    }
}
