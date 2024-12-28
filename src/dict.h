#ifndef DICT_H
#define DICT_H

#define INITIAL_BUCKET_CAPACITY 5
#define INITIAL_DICT_CAPACITY 10
#define RESIZE_BUCKET 15

typedef struct {
    char* key;
    char* value;
} Dict_item;

typedef struct {
    Dict_item* items;
    int size;
    int capacity;
} Dict_bucket;

typedef struct {
    Dict_bucket* buckets;
    int size;
    int capacity;
} Dict;

Dict* dict_new(int initial_capacity);
void dict_free(Dict* d);
void dict_print(Dict* d);

void dict_set(Dict* d, const char* key, const char* value);
char* dict_get(Dict* d, const char* key);
void dict_remove(Dict* d, const char* key);

void bucket_set(Dict_bucket* buck, const char* key, const char* value);
char* bucket_get(Dict_bucket* buck, const char* key);
void bucket_remove(Dict_bucket* buck, const char* key);

#endif  // HASHMAP_H