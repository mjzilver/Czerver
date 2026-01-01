#include "api_utils.h"

#include "../datastructure/arena.h"
#include "../json/json.h"

Arena* get_request_arena() {
    static Arena* arena = NULL;
    if (!arena) {
        arena = arena_new(API_ARENA_SIZE);
    }
    return arena;
}

Arena* get_fresh_request_arena() {
    Arena* arena = get_request_arena();
    arena_clear(arena);
    json_arena_free();
    return arena;
}

char* make_error(Arena* arena, const char* err_msg) {
    JsonValueNode* err_obj = arena_alloc(arena, sizeof(JsonValueNode));
    err_obj->type = JSON_OBJECT;
    err_obj->value.object = dict_arena_new(arena, 1);

    JsonValueNode* err_val = arena_alloc(arena, sizeof(JsonValueNode));
    err_val->type = JSON_STRING;
    err_val->value.string = arena_strdup(arena, err_msg);
    dict_set(err_obj->value.object, "error", err_val);

    return json_stringify(err_obj);
}