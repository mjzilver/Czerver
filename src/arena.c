#include "arena.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ALIGN_UP(x, align) (((x) + (align) - 1) & ~((align) - 1))

Arena* arena_new(size_t size) {
    Arena* arena = malloc(sizeof(Arena));
    if (!arena) return NULL;

    arena->buffer = malloc(size);
    if (!arena->buffer) {
        free(arena);
        return NULL;
    }

    arena->size = size;
    arena->used = 0;
    return arena;
}

void* arena_alloc(Arena* arena, size_t n) {
    size_t offset = ALIGN_UP(arena->used, sizeof(void*));

    if (offset + n > arena->size) {
        fprintf(stderr, "Arena out of memory!\n");
        return NULL;
    }

    void* ptr = arena->buffer + offset;
    arena->used = offset + n;
    return ptr;
}

void arena_clear(Arena* arena) {
    if (!arena) return;
    arena->used = 0;
}

void arena_free(Arena* arena) {
    if (!arena) return;
    free(arena->buffer);
    free(arena);
}

char* arena_strdup(Arena* arena, const char* str) {
    size_t n = strlen(str) + 1;
    char* p = arena_alloc(arena, n);
    if (p) memcpy(p, str, n);
    return p;
}
