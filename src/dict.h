#ifndef DICT_H
#define DICT_H

#define INITIAL_BUCKET_CAPACITY 5
#define INITIAL_DICT_CAPACITY 10
#define RESIZE_BUCKET 15

// Macro to get a value from a dictionary as a specific type
#define DICT_GET_AS(type, dict, key) ((type *)dict_get(dict, key))

typedef struct Dict_item {
    char *key;
    void *value;
} Dict_item;

typedef struct Dict_bucket {
    Dict_item *items;
    int size;
    int capacity;
} Dict_bucket;

typedef struct Dict {
    Dict_bucket *buckets;
    int size;
    int capacity;
} Dict;

Dict *dict_new(int initial_capacity);
void dict_free_all(Dict *d);
void dict_print(Dict *d);

void dict_set(Dict *d, const char *key, void *value);
void *dict_get(Dict *d, const char *key);
void dict_remove(Dict *d, const char *key);
void dict_set_or_replace(Dict *d, const char *key, void *value);

void bucket_set(Dict_bucket *buck, const char *key, void *value);
void *bucket_get(Dict_bucket *buck, const char *key);
void bucket_remove(Dict_bucket *buck, const char *key);

#endif