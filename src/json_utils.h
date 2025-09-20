#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include "dict.h"
#include "json.h"

static inline double get_number(Dict* obj, const char* key) {
    json_object* val = (json_object*)dict_get(obj, key);
    if (!val || val->type != JSON_NUMBER) return 0;
    return val->value.number;
}

static inline const char* get_string(Dict* obj, const char* key) {
    json_object* val = (json_object*)dict_get(obj, key);
    if (!val || val->type != JSON_STRING) return NULL;
    return val->value.string;
}

#endif
