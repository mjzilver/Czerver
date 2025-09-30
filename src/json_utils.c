#include "json_utils.h"

#include "arena.h"
#include "arr_list.h"
#include "json.h"

JsonValueNode* json_new_number(Arena* arena, double value) {
    JsonValueNode* obj = arena_alloc(arena, sizeof(JsonValueNode));
    obj->type = JSON_NUMBER;
    obj->value.number = value;
    return obj;
}

JsonValueNode* json_new_string(Arena* arena, const char* str) {
    JsonValueNode* obj = arena_alloc(arena, sizeof(JsonValueNode));
    obj->type = JSON_STRING;
    obj->value.string = arena_strdup(arena, str);
    return obj;
}

JsonValueNode* json_new_array(Arena* arena, ArrayList* arr) {
    JsonValueNode* obj = arena_alloc(arena, sizeof(JsonValueNode));
    obj->type = JSON_ARRAY;
    obj->value.array = arr;
    return obj;
}

JsonValueNode* json_new_object_from_dict(Arena* arena, Dict* dict) {
    JsonValueNode* obj = arena_alloc(arena, sizeof(JsonValueNode));
    obj->type = JSON_OBJECT;
    obj->value.object = dict;
    return obj;
}

JsonValueNode* json_new_empty_object(Arena* arena, size_t count) {
    JsonValueNode* obj = arena_alloc(arena, sizeof(JsonValueNode));
    obj->type = JSON_OBJECT;
    obj->value.object = dict_arena_new(arena, count);
    return obj;
}

void json_value_node_set(JsonValueNode* obj, const char* key, JsonValueNode* value) {
    dict_set(obj->value.object, key, value);
}

JsonValueNode* struct_to_json_value_node(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count) {
    JsonValueNode* obj = json_new_empty_object(arena, count);
    for (size_t i = 0; i < count; i++) {
        FieldDescriptor* f = &fields[i];
        void* field_ptr = (char*)struct_ptr + f->offset;
        switch (f->type) {
            case FIELD_TYPE_STRING:
                json_value_node_set(obj, f->name, json_new_string(arena, *(const char**)field_ptr));
                break;
            case FIELD_TYPE_NUMBER:
                json_value_node_set(obj, f->name, json_new_number(arena, *(double*)field_ptr));
                break;
            case FIELD_TYPE_ARRAY:
                json_value_node_set(obj, f->name, json_new_array(arena, *(ArrayList**)field_ptr));
                break;
            case FIELD_TYPE_DICT: {
                json_value_node_set(obj, f->name, json_new_object_from_dict(arena, *(Dict**)field_ptr));
                break;
            }
            case FIELD_TYPE_OBJECT: {
                JsonValueNode* nested = struct_to_json_value_node(arena, field_ptr, (FieldDescriptor*)f->object->fields,
                                                                  f->object->field_count);
                json_value_node_set(obj, f->name, nested);
                break;
            }
        }
    }
    return obj;
}