#ifndef DICT_H
#define DICT_H

#include <stddef.h>

#define INITIAL_BUCKET_CAPACITY 5
#define INITIAL_DICT_CAPACITY 10
#define RESIZE_BUCKET 15

#define DICT_GET_AS(type, dict, key) ((type *)dict_get(dict, key))

typedef struct Dict Dict;

typedef struct DictItem {
    char *key;
    void *value;
} DictItem;

typedef struct DictBucket {
    DictItem *items;
    int size;
    int capacity;
} DictBucket;

typedef struct Dict {
    DictBucket *buckets;
    int size;
    int capacity;
} Dict;

typedef void (*DictCallback)(const char *key, void *value, void *user_context);

Dict *dict_new(int initial_capacity);
void dict_free_all(Dict *d);
void dict_print(Dict *d);

void dict_set(Dict *d, const char *key, void *value);
void *dict_get(Dict *d, const char *key);
void dict_remove(Dict *d, const char *key);
void dict_iterate(Dict *d, DictCallback cb, void *user_context);

void bucket_set(DictBucket *buck, const char *key, void *value);
void *bucket_get(DictBucket *buck, const char *key);
void bucket_remove(DictBucket *buck, const char *key);

#endif