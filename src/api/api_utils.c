#include "api_utils.h"

Arena* get_request_arena() {
    static Arena* arena = NULL;
    if (!arena) {
        arena = arena_new(API_ARENA_SIZE);
    }
    return arena;
}

Arena* get_fresh_request_arena() {
    Arena *arena = get_request_arena();
    arena_clear(arena);
    return arena;
}
