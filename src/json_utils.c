#include "json_utils.h"

#include "arena.h"
#include "json.h"

json_object* json_number(Arena* arena, double value) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_NUMBER;
    obj->value.number = value;
    return obj;
}

json_object* json_string(Arena* arena, const char* str) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_STRING;
    obj->value.string = arena_strdup(arena, str);
    return obj;
}

json_object* json_object_new(Arena* arena, size_t count) {
    json_object* obj = arena_alloc(arena, sizeof(json_object));
    obj->type = JSON_OBJECT;
    obj->value.object = dict_arena_new(arena, count);
    return obj;
}

void json_object_set(json_object* obj, const char* key, json_object* value) { dict_set(obj->value.object, key, value); }

json_object* struct_to_json(Arena* arena, void* struct_ptr, FieldDescriptor* fields, size_t count) {
    json_object* obj = json_object_new(arena, count);
    for (size_t i = 0; i < count; i++) {
        FieldDescriptor* f = &fields[i];
        void* field_ptr = (char*)struct_ptr + f->offset;
        switch (f->type) {
            case FIELD_STRING:
                json_object_set(obj, f->name, json_string(arena, *(const char**)field_ptr));
                break;
            case FIELD_NUMBER:
                json_object_set(obj, f->name, json_number(arena, *(double*)field_ptr));
                break;
        }
    }
    return obj;
}