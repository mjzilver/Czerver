#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "arr_list.h"
#include "dict.h"
#include "json.h"

#define FIELD_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

static inline double json_value_node_get_number(Dict* obj, const char* key) {
    JsonValueNode* val = (JsonValueNode*)dict_get(obj, key);
    if (!val || val->type != JSON_NUMBER) return 0;
    return val->value.number;
}

static inline const char* json_value_node_get_string(Dict* obj, const char* key) {
    JsonValueNode* val = (JsonValueNode*)dict_get(obj, key);
    if (!val || val->type != JSON_STRING) return NULL;
    return val->value.string;
}

typedef enum { FIELD_TYPE_STRING, FIELD_TYPE_NUMBER, FIELD_TYPE_ARRAY, FIELD_TYPE_OBJECT, FIELD_TYPE_DICT } FieldType;

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

void json_value_node_set(JsonValueNode* obj, const char* key, JsonValueNode* value);

JsonValueNode* struct_to_json_value_node(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count);

#endif
