#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"
#include "server.h"

int main(int argc, char const *argv[]) {
    int desired_port = 8080;
    if (argc > 1) {
        desired_port = atoi(argv[1]);
    }

    KeyValuePair globals[] = {
        {"site_title", "CZerver - Example site"},
    };

    register_folder("/", "./public", globals);

    KeyValuePair list_replacements[] = {
        globals[0],
        {"item_one", "First item"},
        {"item_two", "Second item"},
        {"item_three", "Third item"},
    };

    register_route("GET", "/list.html", "./public/list.html", list_replacements);

    pthread_t thread_id;
    int success_flag = 0;
    int max_retries = 5;

    for (int i = 0; i <= max_retries; i++) {
        int port_to_try = desired_port + i;

        ServerArgs *args = malloc(sizeof(ServerArgs));
        args->port = port_to_try;
        args->success_flag = &success_flag;

        pthread_create(&thread_id, NULL, start_server_wrapper, args);

        pthread_join(thread_id, NULL);

        if (!success_flag) {
            printf("Failed to start server on port %d, trying next port...\n", port_to_try);
        }

        if (i == max_retries) {
            printf("All attempts failed. Server could not start.\n");
            unregister_all_routes();
            return 1;
        }
    }

    unregister_all_routes();
    return 0;
}
