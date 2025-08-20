#include "arr_list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

ArrayList *arraylist_new(size_t initial_capacity) {
    ArrayList *list = malloc(sizeof(ArrayList));
    assert(list != NULL);

    list->len = 0;
    list->capacity = initial_capacity > 0 ? initial_capacity : 4;
    list->items = malloc(sizeof(ArrayItem) * list->capacity);
    assert(list->items != NULL);

    return list;
}

void arraylist_free(ArrayList *list) {
    if (!list) return;

    for (size_t i = 0; i < list->len; i++) {
        if (list->items[i].owns_value && list->items[i].value) {
            free(list->items[i].value);
        }
    }
    free(list->items);
    free(list);
}

static void arraylist_grow(ArrayList *list) {
    list->capacity *= 2;
    list->items = realloc(list->items, sizeof(ArrayItem) * list->capacity);
    assert(list->items != NULL);
}

void arraylist_append(ArrayList *list, void *value, bool take_ownership) {
    if (list->len >= list->capacity) {
        arraylist_grow(list);
    }

    list->items[list->len].value = value;
    list->items[list->len].owns_value = take_ownership;
    list->len++;
}

void arraylist_remove(ArrayList *list, void *value_to_remove, CompareFunc cmp) {
    size_t j = 0;
    for (size_t i = 0; i < list->len; i++) {
        if (cmp(list->items[i].value, value_to_remove) == 0) {
            if (list->items[i].owns_value) {
                free(list->items[i].value);
            }
            continue;
        }
        if (i != j) {
            list->items[j] = list->items[i];
        }
        j++;
    }
    list->len = j;
}

void *arraylist_get(ArrayList *list, size_t index) {
    if (index >= list->len) return NULL;
    return list->items[index].value;
}

int str_cmp(const void *a, const void *b) { return strcmp((const char *)a, (const char *)b); }