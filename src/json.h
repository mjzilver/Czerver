#ifndef JSON_H
#define JSON_H

#include <stddef.h>

#include "arr_list.h"

typedef enum {
    JSON_STRING,  // "String"
    JSON_NUMBER,  // 123, 123.45
    JSON_ARRAY,   // []
    JSON_OBJECT,  // {}
    JSON_NULL     // NULL
} json_type;

struct json_object;

typedef union {
    char* string;
    double number;
    struct {
        struct json_object** items;
        size_t length;
    } array;
    struct {
        char** keys;
        struct json_object** values;
        size_t length;
    } object;
} json_value;

typedef struct json_object {
    json_type type;
    json_value value;
} json_object;

typedef enum json_token_type {
    LeftBrace,     // {
    RightBrace,    // }
    LeftBracket,   // [
    RightBracket,  // ]
    Colon,         // :
    Comma,         // ,
    String,        // "text" or 'text'
    Number,        // 123, 45.67, -89
    Boolean,       // true or false
    Null           // null
} json_token_type;

typedef struct json_token {
    json_token_type type;
    json_value value;
} json_token;

ArrayList* json_tokenize(const char* json_string);
json_object* json_decode(const char* json_string);
char* json_encode(const json_object* obj);
void json_free(json_object* obj);

#endif
