#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>
#include <string.h>

static inline size_t portable_strnlen(const char *s, size_t maxlen) {
    size_t i;
    for (i = 0; i < maxlen && s[i]; i++);
    return i;
}

static inline char* portable_strdup(const char *s) {
    if (!s) return NULL;
    size_t len = strlen(s) + 1;
    char* copy = malloc(len);
    if (!copy) return NULL;
    memcpy(copy, s, len);
    return copy;
}

static inline char* portable_strndup(const char *s, size_t n) {
    if (!s) return NULL;
    size_t len = portable_strnlen(s, n);
    char* copy = malloc(len + 1);
    if (!copy) return NULL;
    memcpy(copy, s, len);
    copy[len] = '\0';
    return copy;
}

#if !defined(strdup)
#define strdup portable_strdup
#endif

#if !defined(strndup)
#define strndup portable_strndup
#endif

#if !defined(strnlen)
#define strnlen portable_strnlen
#endif

#endif
