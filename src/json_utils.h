#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "arr_list.h"
#include "dict.h"
#include "json.h"

#define FIELD_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

static inline double get_number(Dict* obj, const char* key) {
    json_object* val = (json_object*)dict_get(obj, key);
    if (!val || val->type != JSON_NUMBER) return 0;
    return val->value.number;
}

static inline const char* get_string(Dict* obj, const char* key) {
    json_object* val = (json_object*)dict_get(obj, key);
    if (!val || val->type != JSON_STRING) return NULL;
    return val->value.string;
}

typedef enum { FIELD_STRING, FIELD_NUMBER, FIELD_ARRAY, FIELD_OBJECT, FIELD_DICT } FieldType;

typedef struct FieldDescriptor FieldDescriptor;

typedef struct {
    const FieldDescriptor* fields;
    size_t field_count;
} ObjectMeta;

struct FieldDescriptor {
    const char* name;
    FieldType type;
    size_t offset;
    ObjectMeta* object;
    ArrayList* array;
};

json_object* json_number(Arena* arena, double value);
json_object* json_string(Arena* arena, const char* str);
json_object* json_new_array(Arena* arena, ArrayList* arr);
json_object* json_new_object(Arena* arena, Dict* dict);

json_object* json_object_new(Arena* arena, size_t count);
void json_object_set(json_object* obj, const char* key, json_object* value);

json_object* struct_to_json(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count);

#endif
