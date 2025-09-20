#ifndef ARRAYLIST_UTILS_H
#define ARRAYLIST_UTILS_H

#include <string.h>

int string_compare_function(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }

int int_compare_function(const void *a, const void *b) { return *(int *)a - *(int *)b; }

#endif