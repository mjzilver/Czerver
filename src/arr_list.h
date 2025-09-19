#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

typedef struct ArrayItem {
    void *value;
    bool owns_value;
} ArrayItem;

typedef struct ArrayList {
    ArrayItem *items;
    size_t len;
    size_t capacity;
} ArrayList;

ArrayList *arraylist_new(size_t initial_capacity);
void arraylist_free(ArrayList *list);

void arraylist_append(ArrayList *list, void *value, bool take_ownership);

typedef void (*ArrayListCallback)(ArrayItem *item, size_t index, void *user_context);
void arraylist_iterate(ArrayList *list, ArrayListCallback cb, void *user_context);

typedef int (*CompareFunc)(const void *a, const void *b);
void arraylist_remove(ArrayList *list, void *value_to_remove, CompareFunc cmp);
void *arraylist_get(ArrayList *list, size_t index);

int str_cmp(const void *a, const void *b);

#endif
