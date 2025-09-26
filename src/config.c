#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"
#include "file.h"
#include "globals.h"
#include "json.h"

typedef enum KeyType { TYPE_INT, TYPE_STRING } KeyType;

typedef struct ConfigKey {
    const char* name;
    KeyType type;
    void* ptr;
    size_t max_len;
} ConfigKey;

void parse_file_content(const char* content) {
    json_object* config_json = json_decode(content);
    Dict* server_obj = config_json->value.object;
    Dict* server = ((json_object*)dict_get(server_obj, "server"))->value.object;

    cfg->port = ((json_object*)dict_get(server, "port"))->value.number;
    cfg->get_dir = ((json_object*)dict_get(server, "get_dir"))->value.string;
    cfg->post_dir = ((json_object*)dict_get(server, "post_dir"))->value.string;
    cfg->site_title = ((json_object*)dict_get(server, "site_title"))->value.string;

    json_free();
}

static const char* default_config =
    "{\n"
    "  \"server\": {\n"
    "    \"port\": 8080,\n"
    "    \"get_dir\": \"./public\",\n"
    "    \"post_dir\": \"./post\",\n"
    "    \"site_title\": \"Czerver\"\n"
    "  }\n"
    "}\n";

void parse_json_config(const char* file_path) {
    char* file_content = read_file(file_path);

    if (file_content == NULL) {
        int cr_file_ret = create_file(file_path, default_config);
        assert(cr_file_ret == 0);
        file_content = read_file(file_path);
    }

#if DEBUG
    printf("Config file content: \n%s\n", file_content);
#endif

    cfg = malloc(sizeof(Config));

    parse_file_content(file_content);

    free(file_content);
}
