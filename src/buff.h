#ifndef BUFF_H
#define BUFF_H

#include <stddef.h>
#include "arena.h"

typedef struct {
    char *data;
    size_t size;
    size_t length;
    Arena *arena;
} Buffer;

Buffer *buffer_new(size_t initial_size);
Buffer *buffer_arena_new(Arena *arena, size_t initial_size);

void buffer_append(Buffer *buf, const char *content, size_t length);
void buffer_remove(Buffer *buf, size_t n);
void buffer_resize(Buffer *buf, size_t needed_size);
void buffer_free(Buffer *buf);
char *buffer_take_data(Buffer *buf);

#endif
