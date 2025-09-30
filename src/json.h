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
} JsonType;

typedef union {
    char* string;
    double number;
    bool boolean;
    ArrayList* array;
    Dict* object;
} JsonValue;

typedef struct {
    JsonType type;
    JsonValue value;
} JsonValueNode;

typedef enum {
    JSON_TOKEN_LEFT_BRACE,     // {
    JSON_TOKEN_RIGHT_BRACE,    // }
    JSON_TOKEN_LEFT_BRACKET,   // [
    JSON_TOKEN_RIGHT_BRACKET,  // ]
    JSON_TOKEN_COLON,          // :
    JSON_TOKEN_COMMA,          // ,
    JSON_TOKEN_STRING,         // "text" or 'text'
    JSON_TOKEN_NUMBER,         // 123, 45.67, -89
    JSON_TOKEN_BOOLEAN,        // true or false
    JSON_TOKEN_NULL            // null
} JsonTokenType;

typedef struct {
    JsonTokenType type;
    JsonValue value;
} JsonToken;

ArrayList* json_lex_string(const char* json_string);
JsonValueNode* json_parse_string(const char* json_string);
char* json_stringify(const JsonValueNode* obj);

void json_arena_free();

#endif
