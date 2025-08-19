#include "dict.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Dict *dict_new(int initial_capacity) {
    Dict *d = malloc(sizeof(Dict));
    assert(d != NULL);

    d->size = 0;
    d->capacity = initial_capacity;
    d->buckets = malloc(sizeof(Dict_bucket) * initial_capacity);
    assert(d->buckets != NULL);

    for (int i = 0; i < initial_capacity; i++) {
        d->buckets[i].size = 0;
        d->buckets[i].capacity = INITIAL_BUCKET_CAPACITY;
        d->buckets[i].items = malloc(sizeof(Dict_item) * INITIAL_BUCKET_CAPACITY);
        assert(d->buckets[i].items != NULL);
    }
    return d;
}

void dict_free_all(Dict *d) {
    for (int i = 0; i < d->capacity; i++) {
        for (int j = 0; j < d->buckets[i].size; j++) {
            free(d->buckets[i].items[j].key);
        }
        free(d->buckets[i].items);
    }
    free(d->buckets);
    free(d);
}

// djb2 hash function
unsigned int hash(const char *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

int get_index(int hashed_key, int capacity) { return (hashed_key % capacity + capacity) % capacity; }

void dict_set(Dict *d, const char *key, void *value) {
    dict_remove(d, key);

    int hashed_key = hash(key);
    int idx = get_index(hashed_key, d->capacity);
    bucket_set(&d->buckets[idx], key, value);
}

void *dict_get(Dict *d, const char *key) {
    int hashed_key = hash(key);
    int idx = get_index(hashed_key, d->capacity);

    return bucket_get(&d->buckets[idx], key);
}

void dict_remove(Dict *d, const char *key) {
    int hashed_key = hash(key);
    int idx = get_index(hashed_key, d->capacity);

    bucket_remove(&d->buckets[idx], key);
}

void dict_iterate(Dict *d, DictCallback cb, void *user_context) {
    for (int i = 0; i < d->capacity; i++) {
        Dict_bucket *b = &d->buckets[i];
        for (int j = 0; j < b->size; j++) {
            cb(b->items[j].key, b->items[j].value, user_context);
        }
    }
}

void bucket_set(Dict_bucket *b, const char *key, void *value) {
    Dict_item di;
    di.key = strdup(key);
    di.value = value;
    assert(di.key != NULL);

    if (b->size >= b->capacity) {
        b->capacity *= 2;
        b->items = realloc(b->items, sizeof(Dict_item) * b->capacity);
        assert(b->items != NULL);
    }

    b->items[b->size] = di;
    b->size++;
}

void *bucket_get(Dict_bucket *b, const char *key) {
    for (int i = 0; i < b->size; i++) {
        if (b->items[i].key != NULL && strcmp(b->items[i].key, key) == 0) {
            return b->items[i].value;
        }
    }
    return NULL;
}

void bucket_remove(Dict_bucket *b, const char *key) {
    for (int i = 0; i < b->size; i++) {
        if (b->items[i].key != NULL && strcmp(b->items[i].key, key) == 0) {
            free(b->items[i].key);

            // Shift the items
            for (int k = i + 1; k < b->size; k++) {
                b->items[k - 1] = b->items[k];
            }

            b->items[b->size - 1].key = NULL;
            b->items[b->size - 1].value = NULL;

            b->size--;
            return;
        }
    }
}

void dict_print(Dict *d) {
    printf("==== Dict Start ====\n");
    for (int i = 0; i < d->capacity; i++) {
        printf("Bucket %d (size=%d): ", i, d->buckets[i].size);
        for (int j = 0; j < d->buckets[i].size; j++) {
            printf("(%s=%p) ", d->buckets[i].items[j].key, d->buckets[i].items[j].value);
        }
        printf("\n");
    }
    printf("==== Dict End ====\n");
}
