#include "buff.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Buffer *buffer_new(size_t initial_size) {
    Buffer *buf = malloc(sizeof(Buffer));
    assert(buf != NULL);

    buf->data = malloc(initial_size);
    assert(buf->data != NULL);
    buf->data[0] = '\0';

    buf->size = initial_size;
    buf->length = 0;
    return buf;
}

void buffer_resize(Buffer *buf, size_t needed_size) {
    if (buf->size < needed_size) {
        buf->size = needed_size * 2;
        buf->data = realloc(buf->data, buf->size);
        assert(buf->data != NULL);
    }
}

void buffer_append(Buffer *buf, const char *content, size_t length) {
    buffer_resize(buf, buf->length + length + 1);
    memcpy(buf->data + buf->length, content, length);
    buf->length += length;
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
