#ifndef API_UTILS_H
#define API_UTILS_H

#include "../arena.h"

static const size_t API_ARENA_SIZE = 516 * 1024;

Arena* get_request_arena();
Arena* get_fresh_request_arena();

#endif
