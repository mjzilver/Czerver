#include "json.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "arr_list.h"
#include "buff.h"
#include "dict.h"
#include "string_utils.h"

Arena* get_json_arena() {
    static Arena* arena = NULL;
    static const int JSON_ARENA_SIZE = 1024 * 1024;  // 1 megabyte

    if (!arena) {
        arena = arena_new(JSON_ARENA_SIZE);
    }
    return arena;
}

char* get_string_token_type(JsonTokenType tt) {
    switch (tt) {
        case JSON_TOKEN_LEFT_BRACE:
            return "{";
        case JSON_TOKEN_RIGHT_BRACE:
            return "}";
        case JSON_TOKEN_LEFT_BRACKET:
            return "[";
        case JSON_TOKEN_RIGHT_BRACKET:
            return "]";
        case JSON_TOKEN_COLON:
            return ":";
        case JSON_TOKEN_COMMA:
            return ",";
        case JSON_TOKEN_STRING:
            return "STRING";
        case JSON_TOKEN_NUMBER:
            return "NUMBER";
        case JSON_TOKEN_BOOLEAN:
            return "BOOLEAN";
        case JSON_TOKEN_NULL:
            return "NULL";
        default:
            return "UNKNOWN";
    }
}

JsonToken* parse_json_string(const char* json_string, size_t* i, const char QUOTE_CHAR) {
    JsonToken* tok = arena_alloc(get_json_arena(), (sizeof(JsonToken)));
    tok->type = JSON_TOKEN_STRING;

    Buffer* buff = buffer_arena_new(get_json_arena(), 100);
    size_t len = strlen(json_string);

    (*i)++;  // Consume opening quote

    for (; *i < len; (*i)++) {
        const char C = json_string[*i];

        if (C == QUOTE_CHAR) {
            tok->value.string = arena_strdup(get_json_arena(), buff->data);
            (*i)++;  // Consume closing quote
            return tok;
        }

        char temp[2] = {C, '\0'};
        buffer_append(buff, temp, 1);
    }

    fprintf(stderr, "Error: quote without closing quote found");
    return NULL;
}

JsonToken* parse_json_number(const char* json_string, size_t* i) {
    JsonToken* tok = arena_alloc(get_json_arena(), sizeof(JsonToken));
    tok->type = JSON_TOKEN_NUMBER;

    const char* start_ptr = json_string + *i;
    char* end_ptr;

    double val = strtod(start_ptr, &end_ptr);

    if (start_ptr == end_ptr) {
        fprintf(stderr, "Error: Invalid number at position %zu\n", *i);
        return NULL;
    }

    tok->value.number = val;
    *i += (end_ptr - start_ptr);

    return tok;
}

void append_single_char_token(ArrayList* tokens, JsonTokenType type) {
    JsonToken* tok = arena_alloc(get_json_arena(), sizeof(JsonToken));
    tok->type = type;
    arraylist_append(tokens, tok, false);
}

ArrayList* json_lex_string(const char* json_string) {
    ArrayList* tokens = arraylist_arena_new(get_json_arena(), 100);
    size_t len = strlen(json_string);

    for (size_t i = 0; i < len;) {
        char c = json_string[i];

        // Skip whitespace
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            i++;
            continue;
        }

        // Special characters
        if (c == '{') {
            append_single_char_token(tokens, JSON_TOKEN_LEFT_BRACE);
            i++;
            continue;
        }
        if (c == '}') {
            append_single_char_token(tokens, JSON_TOKEN_RIGHT_BRACE);
            i++;
            continue;
        }
        if (c == '[') {
            append_single_char_token(tokens, JSON_TOKEN_LEFT_BRACKET);
            i++;
            continue;
        }
        if (c == ']') {
            append_single_char_token(tokens, JSON_TOKEN_RIGHT_BRACKET);
            i++;
            continue;
        }
        if (c == ':') {
            append_single_char_token(tokens, JSON_TOKEN_COLON);
            i++;
            continue;
        }
        if (c == ',') {
            append_single_char_token(tokens, JSON_TOKEN_COMMA);
            i++;
            continue;
        }

        // Strings
        if (c == '"' || c == '\'') {
            JsonToken* tok = parse_json_string(json_string, &i, c);
            arraylist_append(tokens, tok, false);
            continue;
        }

        // Numbers
        if ((c >= '0' && c <= '9') || c == '-') {
            JsonToken* tok = parse_json_number(json_string, &i);
            arraylist_append(tokens, tok, false);
            continue;
        }

        // Keywords: null, true, false
        if (i + 3 < len && strncmp(json_string + i, "null", 4) == 0) {
            JsonToken* tok = arena_alloc(get_json_arena(), sizeof(JsonToken));
            tok->type = JSON_TOKEN_NULL;
            arraylist_append(tokens, tok, false);
            i += 4;
            continue;
        }
        if (i + 3 < len && strncmp(json_string + i, "true", 4) == 0) {
            JsonToken* tok = arena_alloc(get_json_arena(), sizeof(JsonToken));
            tok->type = JSON_TOKEN_BOOLEAN;
            tok->value.boolean = true;
            arraylist_append(tokens, tok, false);
            i += 4;
            continue;
        }
        if (i + 4 < len && strncmp(json_string + i, "false", 5) == 0) {
            JsonToken* tok = arena_alloc(get_json_arena(), sizeof(JsonToken));
            tok->type = JSON_TOKEN_BOOLEAN;
            tok->value.boolean = false;
            arraylist_append(tokens, tok, false);
            i += 5;
            continue;
        }

        fprintf(stderr, "Unexpected character '%c' at position %zu\n", c, i);
        i++;
    }

    return tokens;
}

JsonValueNode* parse_value_type(JsonToken* tok) {
    JsonValueNode* obj = arena_alloc(get_json_arena(), sizeof(JsonValueNode));

    switch (tok->type) {
        case JSON_TOKEN_STRING: {
            obj->type = JSON_STRING;
            obj->value.string = tok->value.string;
            return obj;
        }
        case JSON_TOKEN_NUMBER: {
            obj->type = JSON_NUMBER;
            obj->value.number = tok->value.number;
            return obj;
        }
        case JSON_TOKEN_BOOLEAN: {
            obj->type = JSON_BOOLEAN;
            obj->value.boolean = tok->value.boolean;
        }
        default:
            fprintf(stderr, "Error: invalid value type ");
            return NULL;
    }

    return obj;
}

// forward declare
JsonValueNode* parse_tokens(ArrayList* tokens, size_t* i);

JsonValueNode* parse_object_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // consume {

    JsonValueNode* obj = arena_alloc(get_json_arena(), sizeof(JsonValueNode));
    obj->type = JSON_OBJECT;
    obj->value.object = dict_arena_new(get_json_arena(), 10);

    while (true) {
        JsonToken* tok = arraylist_get(tokens, *i);

        // Empty object
        if (tok->type == JSON_TOKEN_RIGHT_BRACE) {
            (*i)++;
            return obj;
        }

        if (tok->type != JSON_TOKEN_STRING) {
            fprintf(stderr, "Error: expected string key, got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
        char* key = tok->value.string;
        (*i)++;

        tok = arraylist_get(tokens, *i);
        if (tok->type != JSON_TOKEN_COLON) {
            fprintf(stderr, "Error: expected ':' after key\n");
            return NULL;
        }
        (*i)++;

        JsonValueNode* value = parse_tokens(tokens, i);
        if (!value) return NULL;

        dict_set(obj->value.object, key, value);

        tok = arraylist_get(tokens, *i);
        if (tok->type == JSON_TOKEN_COMMA) {
            (*i)++;  // skip comma
            continue;
        } else if (tok->type == JSON_TOKEN_RIGHT_BRACE) {
            (*i)++;  // consuming }
            break;
        } else {
            fprintf(stderr, "Error: expected ',' or '}', got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
    }

    return obj;
}

JsonValueNode* parse_array_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // [

    JsonValueNode* obj = arena_alloc(get_json_arena(), sizeof(JsonValueNode));
    obj->type = JSON_ARRAY;
    obj->value.array = arraylist_arena_new(get_json_arena(), 10);

    while (true) {
        JsonToken* tok = arraylist_get(tokens, *i);

        // Empty array
        if (tok->type == JSON_TOKEN_RIGHT_BRACKET) {
            (*i)++;  // consume ]
            return obj;
        }

        JsonValueNode* element = parse_tokens(tokens, i);
        if (!element) {
            fprintf(stderr, "Error: array item, got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
        arraylist_append(obj->value.array, element, false);

        tok = arraylist_get(tokens, *i);
        if (tok->type == JSON_TOKEN_COMMA) {
            (*i)++;  // skip comma
            continue;
        } else if (tok->type == JSON_TOKEN_RIGHT_BRACKET) {
            (*i)++;  // Consume closing ]
            break;
        } else {
            fprintf(stderr, "Error: expected ',' or ']', got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
    }

    return obj;
}

JsonValueNode* parse_tokens(ArrayList* tokens, size_t* i) {
    JsonToken* tok = arraylist_get(tokens, *i);

    switch (tok->type) {
        case JSON_TOKEN_LEFT_BRACE:
            return parse_object_tokens(tokens, i);
        case JSON_TOKEN_LEFT_BRACKET:
            return parse_array_tokens(tokens, i);
        case JSON_TOKEN_BOOLEAN:
        case JSON_TOKEN_STRING:
        case JSON_TOKEN_NUMBER: {
            JsonValueNode* obj = parse_value_type(tok);
            (*i)++;
            return obj;
        }
        case JSON_TOKEN_NULL: {
            JsonValueNode* obj = arena_alloc(get_json_arena(), sizeof(JsonValueNode));
            obj->type = JSON_NULL;
            (*i)++;
            return obj;
        }
        default:
            fprintf(stderr, "Error: cannot process token %s", get_string_token_type(tok->type));
            return NULL;
    }
}

JsonValueNode* json_parse_string(const char* json_string) {
    size_t i = 0;
    ArrayList* tokens = json_lex_string(json_string);
    JsonValueNode* obj = parse_tokens(tokens, &i);

    return obj;
}

void buffer_append_escaped_string(Buffer* buf, const char* str) {
    buffer_append(buf, "\"", 1);

    for (size_t i = 0; str[i]; i++) {
        char c = str[i];
        switch (c) {
            case '"':
                buffer_append(buf, "\\\"", 2);
                break;
            case '\\':
                buffer_append(buf, "\\\\", 2);
                break;
            case '\n':
                buffer_append(buf, "\\n", 2);
                break;
            case '\r':
                buffer_append(buf, "\\r", 2);
                break;
            case '\t':
                buffer_append(buf, "\\t", 2);
                break;
            default:
                buffer_append(buf, &c, 1);
                break;
        }
    }

    buffer_append(buf, "\"", 1);
}

// Forward declare
void json_encode_to_buffer(const JsonValueNode* obj, Buffer* buf);

typedef struct {
    Buffer* buf;
    bool first;
} EncodeContext;

void json_dict_iterate(const char* key, void* value, void* user_context) {
    EncodeContext* ctx = (EncodeContext*)user_context;
    JsonValueNode* json_value_obj = (JsonValueNode*)value;

    if (!ctx->first) {
        buffer_append(ctx->buf, ",", 1);
    }

    buffer_append_escaped_string(ctx->buf, key);
    buffer_append(ctx->buf, ":", 1);
    json_encode_to_buffer(json_value_obj, ctx->buf);

    ctx->first = false;
}

void json_arraylist_iterate(ArrayItem* item, size_t index, void* user_context) {
    (void)index;
    EncodeContext* ctx = (EncodeContext*)user_context;
    JsonValueNode* elem = (JsonValueNode*)item->value;

    if (!ctx->first) {
        buffer_append(ctx->buf, ",", 1);
    }
    json_encode_to_buffer(elem, ctx->buf);
    ctx->first = false;
}

void json_encode_to_buffer(const JsonValueNode* obj, Buffer* buf) {
    if (!obj) {
        buffer_append(buf, "null", 4);
        return;
    }

    switch (obj->type) {
        case JSON_NULL:
            buffer_append(buf, "null", 4);
            break;

        case JSON_BOOLEAN:
            if (obj->value.boolean) {
                buffer_append(buf, "true", 4);
            } else {
                buffer_append(buf, "false", 5);
            }
            break;

        case JSON_NUMBER: {
            char tmp[64];
            int len = snprintf(tmp, sizeof(tmp), "%.17g", obj->value.number);
            buffer_append(buf, tmp, len);
            break;
        }

        case JSON_STRING:
            buffer_append_escaped_string(buf, obj->value.string);
            break;

        case JSON_ARRAY: {
            buffer_append(buf, "[", 1);
            EncodeContext ctx = {.buf = buf, .first = true};
            arraylist_iterate(obj->value.array, json_arraylist_iterate, &ctx);
            buffer_append(buf, "]", 1);
            break;
        }
        case JSON_OBJECT: {
            buffer_append(buf, "{", 1);
            EncodeContext ctx = {.buf = buf, .first = true};
            dict_iterate(obj->value.object, json_dict_iterate, &ctx);
            buffer_append(buf, "}", 1);
            break;
        }
    }
}

char* json_stringify(const JsonValueNode* obj) {
    Buffer* buf = buffer_arena_new(get_json_arena(), 128);
    json_encode_to_buffer(obj, buf);
    char* result = strdup(buf->data);
    return result;
}

void json_arena_free() { arena_clear(get_json_arena()); }