#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stddef.h>

typedef struct {
    char *key;
    char *value;
} KeyValuePair;

char *replace_variables(const char *html_content, KeyValuePair replacements[], size_t count);

#endif
