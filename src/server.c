#include "server.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http_codes.h"
#include "route.h"

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

    // Set success flag based on the server's return code
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

void serve_routes(int client) {
    char buffer[1024];
    int bytes_received = recv(client, buffer, 1023, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);

        if (strstr(buffer, "GET") != NULL) {
            char path[256];
            sscanf(buffer, "GET %s HTTP/1.1", path);

            for (int i = 0; i < routes_count; i++) {
                if (strcmp(routes[i].path, path) == 0) {
                    const char *requested_method = strtok(buffer, " ");
                    if (strcmp(routes[i].method, requested_method) != 0) {
                        continue;
                    }

                    char response[2048] = {0};

                    strcat(response, HTTP_200);

                    char *type_header = get_type_header(routes[i].type);

                    char *file_content = process_template(routes[i].cached_file, routes[i].replacements);

                    strcat(response, "Content-Type: ");
                    strcat(response, type_header);
                    strcat(response, "\n\n");

                    strcat(response, file_content);
                    
                    #ifdef DEBUG
                    printf("RESPONSE:\n%s\n", response);
                    #endif

                    send(client, response, strlen(response), 0);

                    free(file_content);
                
                    return;
                }
            }

            send_404(path, client);
        }
    }
}
