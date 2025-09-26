#include "json_utils.h"

#include "arena.h"
#include "arr_list.h"
#include "json.h"

json_object* json_new_number(Arena* arena, double value) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_NUMBER;
    obj->value.number = value;
    return obj;
}

json_object* json_new_string(Arena* arena, const char* str) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_STRING;
    obj->value.string = arena_strdup(arena, str);
    return obj;
}

json_object* json_new_array(Arena* arena, ArrayList* arr) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_ARRAY;
    obj->value.array = arr;
    return obj;
}

json_object* json_new_object(Arena* arena, Dict* dict) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_OBJECT;
    obj->value.object = dict;
    return obj;
}

json_object* json_new_empty_object(Arena* arena, size_t count) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_OBJECT;
    obj->value.object = dict_arena_new(arena, count);
    return obj;
}

void json_object_set_key(json_object* obj, const char* key, json_object* value) {
    dict_set(obj->value.object, key, value);
}

json_object* struct_to_json(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count) {
    json_object* obj = json_new_empty_object(arena, count);
    for (size_t i = 0; i < count; i++) {
        FieldDescriptor* f = &fields[i];
        void* field_ptr = (char*)struct_ptr + f->offset;
        switch (f->type) {
            case FIELD_STRING:
                json_object_set_key(obj, f->name, json_new_string(arena, *(const char**)field_ptr));
                break;
            case FIELD_NUMBER:
                json_object_set_key(obj, f->name, json_new_number(arena, *(double*)field_ptr));
                break;
            case FIELD_ARRAY:
                json_object_set_key(obj, f->name, json_new_array(arena, *(ArrayList**)field_ptr));
                break;
            case FIELD_DICT: {
                json_object_set_key(obj, f->name, json_new_object(arena, *(Dict**)field_ptr));
                break;
            }
            case FIELD_OBJECT: {
                json_object* nested =
                    struct_to_json(arena, field_ptr, (FieldDescriptor*)f->object->fields, f->object->field_count);
                json_object_set_key(obj, f->name, nested);
                break;
            }
        }
    }
    return obj;
}