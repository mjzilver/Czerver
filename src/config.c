#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "globals.h"

typedef enum KeyType { TYPE_INT, TYPE_STRING } KeyType;

typedef struct ConfigKey {
    const char *name;
    KeyType type;
    void *ptr;
    size_t max_len;
} ConfigKey;

ConfigKey keys[] = {
    {"port", TYPE_INT, NULL, 0},
    {"site_title", TYPE_STRING, NULL, 128},
    {"get_dir", TYPE_STRING, NULL, 128},
    {"post_dir", TYPE_STRING, NULL, 128},
};

size_t key_count = sizeof(keys) / sizeof(keys[0]);

void parse_config_line(const char *line) {
    while (*line == ' ' || *line == '\t') line++;
    if (*line == '\0' || *line == '#') return;

    for (size_t i = 0; i < key_count; i++) {
        size_t key_len = strlen(keys[i].name);

        if (strncmp(line, keys[i].name, key_len) == 0 && (line[key_len] == ' ' || line[key_len] == '=')) {
            const char *value_str = line + key_len;
            while (*value_str == ' ' || *value_str == '\t' || *value_str == '=') value_str++;

            if (keys[i].type == TYPE_INT) {
                *(int *)keys[i].ptr = atoi(value_str);
            } else if (keys[i].type == TYPE_STRING) {
                char fmt[255];
                snprintf(fmt, sizeof(fmt), " \"%%%zu[^\"]\"", keys[i].max_len - 1);

                char tmp[keys[i].max_len];
                if (sscanf(value_str, fmt, tmp) == 1) {
                    *(char **)keys[i].ptr = strdup(tmp);
                }
            }
#if DEBUG
            if (keys[i].type == TYPE_INT) {
                printf("Parsed %s : %d\n", keys[i].name, *(int *)keys[i].ptr);
            } else if (keys[i].type == TYPE_STRING) {
                printf("Parsed %s : %s\n", keys[i].name, *(char **)keys[i].ptr);
            }
#endif
            return;
        }
    }
}

void parse_file_content(const char *content) {
    char *copy = strdup(content);
    char *line = strtok(copy, "\n");
    while (line) {
        parse_config_line(line);
        line = strtok(NULL, "\n");
    }
    free(copy);
}

static const char *default_config =
    "[server]"
    "port = 8080"
    "get_dir = \"./public\""
    "post_dir = \"./post\""
    "site_title = \"Czerver\"";

void parse_toml_config(const char *file_path) {
    char *file_content = read_file(file_path);

    if (file_content == NULL) {
        int cr_file_ret = create_file(file_path, default_config);
        assert(cr_file_ret == 0);
        file_content = read_file(file_path);
    }

#if DEBUG
    printf("Config file content: \n%s\n", file_content);
#endif

    cfg = malloc(sizeof(Config));

    keys[0].ptr = &cfg->port;
    keys[1].ptr = &cfg->site_title;
    keys[2].ptr = &cfg->get_dir;
    keys[3].ptr = &cfg->post_dir;

    parse_file_content(file_content);

    free(file_content);
}
