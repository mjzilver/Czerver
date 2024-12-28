#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stddef.h>

typedef struct KeyValuePair {
    char* key;
    char* value;
} KeyValuePair;

char* replace_variables(const char* html_content, KeyValuePair replacements[]);
char* process_template(const char* html_content, KeyValuePair replacements[]);
char* process_parent_template(const char* html_content, const char* main_content);

#endif
