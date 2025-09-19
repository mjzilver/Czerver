#ifndef JSON_H
#define JSON_H

#include <stddef.h>

#include "arr_list.h"
#include "dict.h"

typedef enum {
    JSON_STRING,   // "String"
    JSON_NUMBER,   // 123, 123.45
    JSON_BOOLEAN,  // True, False
    JSON_ARRAY,    // []
    JSON_OBJECT,   // {}
    JSON_NULL      // NULL
} json_type;

struct json_object;

typedef union {
    char* string;
    double number;
    bool boolean;
    ArrayList* array;
    Dict* object;
} json_value;

typedef struct json_object {
    json_type type;
    json_value value;
} json_object;

typedef enum json_token_type {
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    LEFT_BRACKET,   // [
    RIGHT_BRACKET,  // ]
    COLON,          // :
    COMMA,          // ,
    STRING,         // "text" or 'text'
    NUMBER,         // 123, 45.67, -89
    BOOLEAN,        // true or false
    NULL_TOKEN      // null
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
