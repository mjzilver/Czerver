#include "buff.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

Buffer *buffer_new(size_t initial_size) {
    Buffer *buf = malloc(sizeof(Buffer));
    assert(buf != NULL);
    buf->data = malloc(initial_size);
    assert(buf->data != NULL);
    buf->data[0] = '\0';
    buf->size = initial_size;
    buf->length = 0;
    buf->arena = NULL;

    return buf;
}

Buffer *buffer_arena_new(Arena *arena, size_t initial_size) {
    Buffer *buf = arena_alloc(arena, (sizeof(Buffer)));
    buf->data = arena_alloc(arena, initial_size);
    buf->data[0] = '\0';
    buf->size = initial_size;
    buf->length = 0;
    buf->arena = arena;

    return buf;
}

void buffer_resize(Buffer *buf, size_t needed_size) {
    if (needed_size <= buf->size) return;

    size_t new_size = needed_size * 2;

    if (buf->arena) {
        char *new_data = arena_alloc(buf->arena, new_size);
        if (!new_data) {
            fprintf(stderr, "Buffer resize failed: arena out of space\n");
            assert(0);
        }
        memcpy(new_data, buf->data, buf->length + 1);
        buf->data = new_data;
    } else {
        buf->data = realloc(buf->data, new_size);
        assert(buf->data != NULL);
    }

    buf->size = new_size;
}

void buffer_append(Buffer *buf, const char *content, size_t length) {
    buffer_resize(buf, buf->length + length + 1);
    memcpy(buf->data + buf->length, content, length);
    buf->length += length;
    buf->data[buf->length] = '\0';
}

void buffer_remove(Buffer *buf, size_t n) {
    if (n >= buf->length) {
        buf->length = 0;
    } else {
        buf->length -= n;
    }
    buf->data[buf->length] = '\0';
}

void buffer_free(Buffer *buf) {
    if (!buf) return;
    free(buf->data);
    free(buf);
}

char *buffer_take_data(Buffer *buf) {
    char *data = buf->data;
    free(buf);
    return data;
}