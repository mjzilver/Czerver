#include "json.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "buff.h"
#include "dict.h"

char* get_string_token_type(json_token_type tt) {
    switch (tt) {
        case LEFT_BRACE:
            return "{";
        case RIGHT_BRACE:
            return "}";
        case LEFT_BRACKET:
            return "[";
        case RIGHT_BRACKET:
            return "]";
        case COLON:
            return ":";
        case COMMA:
            return ",";
        case STRING:
            return "STRING";
        case NUMBER:
            return "NUMBER";
        case BOOLEAN:
            return "BOOLEAN";
        case NULL_TOKEN:
            return "NULL";
        default:
            return "UNKNOWN";
    }
}

json_token* parse_json_string(const char* json_string, size_t* i, const char QUOTE_CHAR) {
    json_token* tok = malloc(sizeof(json_token));
    tok->type = STRING;

    Buffer* buff = buffer_new(100);
    size_t len = strlen(json_string);

    (*i)++;  // Consume opening quote

    for (; *i < len; (*i)++) {
        const char C = json_string[*i];

        if (C == QUOTE_CHAR) {
            tok->value.string = strdup(buff->data);
            buffer_free(buff);

            (*i)++;  // Consume closing quote
            return tok;
        }

        char temp[2] = {C, '\0'};
        buffer_append(buff, temp, 1);
    }

    buffer_free(buff);
    free(tok);

    fprintf(stderr, "Error: quote without closing quote found");
    return NULL;
}

json_token* parse_json_number(const char* json_string, size_t* i) {
    json_token* tok = malloc(sizeof(json_token));
    tok->type = NUMBER;

    const char* start_ptr = json_string + *i;
    char* end_ptr;

    double val = strtod(start_ptr, &end_ptr);

    if (start_ptr == end_ptr) {
        fprintf(stderr, "Error: Invalid number at position %zu\n", *i);
        free(tok);
        return NULL;
    }

    tok->value.number = val;
    *i += (end_ptr - start_ptr);

    return tok;
}

void append_single_char_token(ArrayList* tokens, json_token_type type) {
    json_token* tok = malloc(sizeof(json_token));
    tok->type = type;
    arraylist_append(tokens, tok, true);
}

ArrayList* json_tokenize(const char* json_string) {
    ArrayList* tokens = arraylist_new(100);
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
            append_single_char_token(tokens, LEFT_BRACE);
            i++;
            continue;
        }
        if (c == '}') {
            append_single_char_token(tokens, RIGHT_BRACE);
            i++;
            continue;
        }
        if (c == '[') {
            append_single_char_token(tokens, LEFT_BRACKET);
            i++;
            continue;
        }
        if (c == ']') {
            append_single_char_token(tokens, RIGHT_BRACKET);
            i++;
            continue;
        }
        if (c == ':') {
            append_single_char_token(tokens, COLON);
            i++;
            continue;
        }
        if (c == ',') {
            append_single_char_token(tokens, COMMA);
            i++;
            continue;
        }

        // Strings
        if (c == '"' || c == '\'') {
            json_token* tok = parse_json_string(json_string, &i, c);
            arraylist_append(tokens, tok, true);
            continue;
        }

        // Numbers
        if ((c >= '0' && c <= '9') || c == '-') {
            json_token* tok = parse_json_number(json_string, &i);
            arraylist_append(tokens, tok, true);
            continue;
        }

        // Keywords: null, true, false
        if (i + 3 < len && strncmp(json_string + i, "null", 4) == 0) {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = NULL_TOKEN;
            arraylist_append(tokens, tok, true);
            i += 4;
            continue;
        }
        if (i + 3 < len && strncmp(json_string + i, "true", 4) == 0) {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = BOOLEAN;
            tok->value.boolean = true;
            arraylist_append(tokens, tok, true);
            i += 4;
            continue;
        }
        if (i + 4 < len && strncmp(json_string + i, "false", 5) == 0) {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = BOOLEAN;
            tok->value.boolean = false;
            arraylist_append(tokens, tok, true);
            i += 5;
            continue;
        }

        fprintf(stderr, "Unexpected character '%c' at position %zu\n", c, i);
        i++;
    }

    return tokens;
}

json_object* proces_value_type(json_token* tok) {
    json_object* obj = malloc(sizeof(json_object));

    switch (tok->type) {
        case STRING: {
            obj->type = JSON_STRING;
            obj->value.string = tok->value.string;
            return obj;
        }
        case NUMBER: {
            obj->type = JSON_NUMBER;
            obj->value.number = tok->value.number;
            return obj;
        }
        case BOOLEAN: {
            obj->type = JSON_BOOLEAN;
            obj->value.boolean = tok->value.boolean;
        }
        default:
            fprintf(stderr, "Error: invalid value type ");
            free(obj);
            return NULL;
    }

    return obj;
}

// forward declare
json_object* process_tokens(ArrayList* tokens, size_t* i);

json_object* process_object_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // consume {

    json_object* obj = malloc(sizeof(json_object));
    obj->type = JSON_OBJECT;
    obj->value.object = dict_new(10);

    while (true) {
        json_token* tok = arraylist_get(tokens, *i);

        // Empty object
        if (tok->type == RIGHT_BRACE) {
            (*i)++;
            return obj;
        }

        if (tok->type != STRING) {
            fprintf(stderr, "Error: expected string key, got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
        char* key = tok->value.string;
        (*i)++;

        tok = arraylist_get(tokens, *i);
        if (tok->type != COLON) {
            fprintf(stderr, "Error: expected ':' after key\n");
            return NULL;
        }
        (*i)++;

        json_object* value = process_tokens(tokens, i);
        if (!value) return NULL;

        dict_set(obj->value.object, key, value);

        tok = arraylist_get(tokens, *i);
        if (tok->type == COMMA) {
            (*i)++;  // skip comma
            continue;
        } else if (tok->type == RIGHT_BRACE) {
            (*i)++;  // consuming }
            break;
        } else {
            fprintf(stderr, "Error: expected ',' or '}', got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
    }

    return obj;
}

json_object* process_array_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // [

    json_object* obj = malloc(sizeof(json_object));
    obj->type = JSON_ARRAY;
    obj->value.array = arraylist_new(10);

    while (true) {
        json_token* tok = arraylist_get(tokens, *i);

        // Empty array
        if (tok->type == RIGHT_BRACKET) {
            (*i)++;  // consume ]
            return obj;
        }

        json_object* element = process_tokens(tokens, i);
        if (!element) {
            fprintf(stderr, "Error: array item, got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
        arraylist_append(obj->value.array, element, true);

        tok = arraylist_get(tokens, *i);
        if (tok->type == COMMA) {
            (*i)++;  // skip comma
            continue;
        } else if (tok->type == RIGHT_BRACKET) {
            (*i)++;  // Consume closing ]
            break;
        } else {
            fprintf(stderr, "Error: expected ',' or ']', got %s\n", get_string_token_type(tok->type));
            return NULL;
        }
    }

    return obj;
}

json_object* process_tokens(ArrayList* tokens, size_t* i) {
    json_token* tok = arraylist_get(tokens, *i);

    switch (tok->type) {
        case LEFT_BRACE:
            return process_object_tokens(tokens, i);
        case LEFT_BRACKET:
            return process_array_tokens(tokens, i);
        case BOOLEAN:
        case STRING:
        case NUMBER: {
            json_object* obj = proces_value_type(tok);
            (*i)++;
            return obj;
        }
        case NULL_TOKEN: {
            json_object* obj = malloc(sizeof(json_object));
            obj->type = JSON_NULL;
            (*i)++;
            return obj;
        }
        default:
            fprintf(stderr, "Error: cannot process token %s", get_string_token_type(tok->type));
            return NULL;
    }
}

json_object* json_decode(const char* json_string) {
    size_t i = 0;
    ArrayList* tokens = json_tokenize(json_string);
    json_object* obj = process_tokens(tokens, &i);
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
void json_encode_to_buffer(const json_object* obj, Buffer* buf);

void json_dict_iterate(const char* key, void* value, void* user_context) {
    Buffer* buf = (Buffer*)user_context;
    buffer_append_escaped_string(buf, key);
    buffer_append(buf, ":", 1);
    json_object* json_value_obj = (json_object*)value;
    json_encode_to_buffer(json_value_obj, buf);
    buffer_append(buf, ",", 1);
}

void json_array_list_iterate(ArrayItem* item, size_t index, void* user_context) {
    (void)index;
    Buffer* buf = (Buffer*)user_context;
    json_object* elem = (json_object*)item->value;
    json_encode_to_buffer(elem, buf);
    buffer_append(buf, ",", 1);
}

void json_encode_to_buffer(const json_object* obj, Buffer* buf) {
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

        case JSON_NUMBER: {
            char tmp[64];
            int len = snprintf(tmp, sizeof(tmp), "%.17g", obj->value.number);
            buffer_append(buf, tmp, len);
            break;
        }

        case JSON_STRING:
            buffer_append_escaped_string(buf, obj->value.string);
            break;

        case JSON_ARRAY:
            buffer_append(buf, "[", 1);
            arraylist_iterate(obj->value.array, json_array_list_iterate, buf);
            buffer_remove(buf, 1);  // Remove trailing comma
            buffer_append(buf, "]", 1);
            break;
        case JSON_OBJECT:
            buffer_append(buf, "{", 1);
            dict_iterate(obj->value.object, json_dict_iterate, buf);
            buffer_remove(buf, 1);  // Remove trailing comma
            buffer_append(buf, "}", 1);
            break;
    }
}

char* json_encode(const json_object* obj) {
    Buffer* buf = buffer_new(128);
    json_encode_to_buffer(obj, buf);
    char* result = buffer_take_data(buf);
    return result;
}

void json_free(json_object* obj) {
    switch (obj->type) {
        case JSON_BOOLEAN:
        case JSON_NULL:
        case JSON_NUMBER:
            break;
        case JSON_STRING:
            free(obj->value.string);
            break;
        case JSON_ARRAY:
            arraylist_free(obj->value.array);
            break;
        case JSON_OBJECT:
            dict_free_all(obj->value.object);
            break;
    }
}