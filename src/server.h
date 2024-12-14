#ifndef SERVER_H
#define SERVER_H

typedef struct
{
    int port;
    int *success_flag;
} ServerArgs;

int start_server(int port);
void *start_server_wrapper(void *args);
void serve_routes(int client);

#endif
