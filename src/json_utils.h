#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "dict.h"
#include "json.h"

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

typedef enum { FIELD_STRING, FIELD_NUMBER } FieldType;

typedef struct {
    const char* name;
    FieldType type;
    size_t offset;
} FieldDescriptor;

json_object* json_number(Arena* arena, double value);
json_object* json_string(Arena* arena, const char* str);
json_object* json_object_new(Arena* arena, size_t count);
void json_object_set(json_object* obj, const char* key, json_object* value);

json_object* struct_to_json(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count);

#endif
