#ifndef ARENA_H
#define ARENA_H

#include <stdlib.h>

typedef struct Arena {
    char* buffer;
    size_t size;
    size_t used;
} Arena;

Arena* arena_new(size_t size);
void* arena_alloc(Arena* arena, size_t n);
void arena_clear(Arena* arena);
void arena_free(Arena* arena);

char* arena_strdup(Arena* a, const char* s);

#endif