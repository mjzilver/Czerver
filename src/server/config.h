#ifndef CONFIG_H
#define CONFIG_H

#include <stddef.h>

typedef struct Config {
    int port;
    char* site_title;
    char* get_dir;
    char* post_dir;
} Config;

void parse_json_config(const char* file_path);

#endif