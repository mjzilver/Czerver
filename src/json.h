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
    LEFT_BRACE_TOKEN,     // {
    RIGHT_BRACE_TOKEN,    // }
    LEFT_BRACKET_TOKEN,   // [
    RIGHT_BRACKET_TOKEN,  // ]
    COLON_TOKEN,          // :
    COMMA_TOKEN,          // ,
    STRING_TOKEN,         // "text" or 'text'
    NUMBER_TOKEN,         // 123, 45.67, -89
    BOOLEAN_TOKEN,        // true or false
    NULL_TOKEN            // null
} json_token_type;

typedef struct json_token {
    json_token_type type;
    json_value value;
} json_token;

ArrayList* json_tokenize(const char* json_string);
json_object* json_decode(const char* json_string);
char* json_encode(const json_object* obj);

void json_free();

#endif
