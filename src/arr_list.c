#include "arr_list.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

ArrayList* arraylist_new(size_t initial_capacity) {
    ArrayList* list = malloc(sizeof(ArrayList));
    assert(list != NULL);

    list->len = 0;
    list->capacity = initial_capacity > 0 ? initial_capacity : 4;
    list->items = malloc(sizeof(ArrayItem) * list->capacity);
    list->arena = NULL;
    assert(list->items != NULL);

    return list;
}

ArrayList* arraylist_arena_new(Arena* arena, size_t initial_capacity) {
    ArrayList* list = arena_alloc(arena, sizeof(ArrayList));

    list->len = 0;
    list->capacity = initial_capacity > 0 ? initial_capacity : 4;
    list->items = arena_alloc(arena, (sizeof(ArrayItem) * list->capacity));
    list->arena = arena;

    return list;
}

void arraylist_free(ArrayList* list) {
    if (!list) return;
    if (list->arena) return;

    for (size_t i = 0; i < list->len; i++) {
        if (list->items[i].owns_value && list->items[i].value) {
            free(list->items[i].value);
        }
    }
    free(list->items);
    free(list);
}

static void arraylist_grow(ArrayList* list) {
    list->capacity *= 2;
    if (list->arena) {
        ArrayItem* new_items = arena_alloc(list->arena, sizeof(ArrayItem) * list->capacity);
        memcpy(new_items, list->items, sizeof(ArrayItem) * list->len);
        list->items = new_items;
    } else {
        list->items = realloc(list->items, sizeof(ArrayItem) * list->capacity);
        assert(list->items != NULL);
    }
}

void arraylist_append(ArrayList* list, void* value, bool take_ownership) {
    if (list->len >= list->capacity) {
        arraylist_grow(list);
    }

    list->items[list->len].value = value;
    list->items[list->len].owns_value = take_ownership;
    list->len++;
}

void arraylist_remove(ArrayList* list, void* value_to_remove, CompareFunc cmp) {
    if (!list) return;

    size_t original_len = list->len;
    size_t j = 0;

    void** to_free = NULL;
    size_t free_count = 0;

    if (!list->arena) {
        to_free = malloc(sizeof(void*) * list->len);
    }

    for (size_t i = 0; i < original_len; i++) {
        if (cmp(list->items[i].value, value_to_remove) != 0) {
            if (i != j) list->items[j] = list->items[i];
            j++;
        } else if (!list->arena && list->items[i].owns_value && list->items[i].value) {
            to_free[free_count++] = list->items[i].value;
        }
    }

    list->len = j;

    if (!list->arena) {
        for (size_t k = 0; k < free_count; k++) {
            free(to_free[k]);
        }
        free(to_free);
    }
}

void* arraylist_get(ArrayList* list, size_t i) {
    if (i >= list->len) return NULL;
    return list->items[i].value;
}

void arraylist_iterate(ArrayList* list, ArrayListCallback cb, void* user_context) {
    for (size_t i = 0; i < list->len; i++) {
        cb(&list->items[i], i, user_context);
    }
}