#ifndef ARRAYLIST_UTILS_H
#define ARRAYLIST_UTILS_H

#include <string.h>

static inline int string_compare_function(const void *a, const void *b) {
    return strcmp((const char *)a, (const char *)b);
}

static inline int int_compare_function(const void *a, const void *b) { return *(int *)a - *(int *)b; }

#endif