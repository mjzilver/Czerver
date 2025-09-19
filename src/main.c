#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "config.h"
#include "globals.h"
#include "route.h"
#include "server.h"

int main(int argc, char const *argv[]) {
    parse_toml_config("./config.toml");

    int port = cfg->port;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    var_dict = dict_new(10);

    dict_set(var_dict, "site_title", cfg->site_title);
    dict_set(var_dict, "item_one", "First item");
    dict_set(var_dict, "item_two", "Second item");
    dict_set(var_dict, "item_three", "Third item");

    ArrayList *list = arraylist_new(4);
    arraylist_append(list, strdup("Clean room"), true);
    arraylist_append(list, strdup("Get groceries"), true);
    arraylist_append(list, strdup("Walk dog"), true);
    arraylist_append(list, strdup("Mow grass"), true);

    dict_set(var_dict, "todos", list);

    register_folder("GET", "/", cfg->get_dir);
    register_folder("POST", "/", cfg->post_dir);

    pthread_t thread_id;
    int success_flag = 0;

    ServerArgs *args = malloc(sizeof(ServerArgs));
    args->port = port;
    args->success_flag = &success_flag;

    pthread_create(&thread_id, NULL, start_server_wrapper, args);

    pthread_join(thread_id, NULL);

    if (!success_flag) {
        printf("Failed to start server on port %d \n", port);
    }

    unregister_all_routes();
    dict_free_all(var_dict);
    free(cfg);
    return 0;
}
