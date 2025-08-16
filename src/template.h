#ifndef TEMPLATE_H
#define TEMPLATE_H

#include <stddef.h>

typedef struct KeyValuePair {
    char *key;
    char *value;
} KeyValuePair;

char *process_template(const char *template_content, KeyValuePair *replacements);

#endif
