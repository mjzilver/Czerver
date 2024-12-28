#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "route.h"
#include "server.h"

int main(int argc, char const* argv[]) {
    int desired_port = 8080;
    if (argc > 1) {
        desired_port = atoi(argv[1]);
    }

    // Register global variables
    KeyValuePair globals[] = {
        {"site_title", "CZerver - Example site"},
    };

    // Register routes
    register_route("GET", "/", "./html/index.html", globals, 0);
    register_route("GET", "/about", "./html/about.html", globals, 0);
    register_route("GET", "/contact", "./html/contact.html", globals, 0);

    // Register a route with replacements
    KeyValuePair list_replacements[] = {
        globals[0],
        {"item_one", "First item"},
        {"item_two", "Second item"},
        {"item_three", "Third item"},
    };
    register_route("GET", "/list", "./html/list.html", list_replacements, 3);

    /* CSS */
    register_route("GET", "/style.css", "./html/css/style.css", NULL, 0);

    /* JS */
    // nothing here yet

    pthread_t thread_id;
    int success_flag = 0;
    int max_retries = 5;

    for (int i = 0; i <= max_retries; i++) {
        int port_to_try = desired_port + i;

        // Prepare arguments for the thread
        ServerArgs* args = malloc(sizeof(ServerArgs));
        args->port = port_to_try;
        args->success_flag = &success_flag;

        pthread_create(&thread_id, NULL, start_server_wrapper, args);

        // Wait for the thread to finish (server to stop)
        pthread_join(thread_id, NULL);

        if (!success_flag) {
            printf("Failed to start server on port %d, trying next port...\n", port_to_try);
        }

        if (i == max_retries) {
            printf("All attempts failed. Server could not start.\n");
            free_routes();
            return 1;
        }
    }

    free_routes();
    return 0;
}
