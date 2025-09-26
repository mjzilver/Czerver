#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "api/math.h"
#include "api/weather.h"
#include "arr_list.h"
#include "config.h"
#include "globals.h"
#include "route.h"
#include "server.h"
#include "string_utils.h"

int main(int argc, char const* argv[]) {
    parse_json_config("./config.json");

    int port = cfg->port;
    if (argc > 1) {
        port = atoi(argv[1]);
    }
    var_dict = dict_new(10);

    dict_set(var_dict, "site_title", cfg->site_title);
    dict_set(var_dict, "item_one", "First item");
    dict_set(var_dict, "item_two", "Second item");
    dict_set(var_dict, "item_three", "Third item");

    ArrayList* todo_list = arraylist_new(4);
    arraylist_append(todo_list, strdup("Clean room"), true);
    arraylist_append(todo_list, strdup("Get groceries"), true);
    arraylist_append(todo_list, strdup("Walk dog"), true);
    arraylist_append(todo_list, strdup("Mow grass"), true);

    dict_set(var_dict, "todos", todo_list);

    register_folder("GET", "/", cfg->get_dir);
    register_folder("POST", "/", cfg->post_dir);

    register_api_route("POST", "/api/math", math_api_handler);
    register_api_route("GET", "/api/weather", weather_api_handler);

    pthread_t thread_id;
    int success_flag = 0;

    ServerArgs* args = malloc(sizeof(ServerArgs));
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
    arraylist_free(todo_list);
    return 0;
}
