#include "json.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arr_list.h"
#include "buff.h"

json_token* parse_json_string(const char* json_string, size_t* i, const char quote_char) {
    json_token* tok = malloc(sizeof(json_token));
    tok->type = String;

    Buffer* buff = buffer_new(100);
    size_t len = strlen(json_string);

    // Skip opening quote
    (*i)++;

    for (; *i < len; (*i)++) {
        const char c = json_string[*i];

        if (c == quote_char) {
            tok->value.string = strdup(buff->data);
            buffer_free(buff);
            return tok;
        }

        char temp[2] = {c, '\0'};
        buffer_append(buff, temp, 1);
    }

    buffer_free(buff);
    free(tok);

    fprintf(stderr, "Error: quote without closing quote found");
    assert(true);
    return NULL;
}

json_token* parse_json_number(const char* json_string, size_t* i) {
    json_token* tok = malloc(sizeof(json_token));
    tok->type = Number;

    size_t len = strlen(json_string);
    size_t start = *i;

    while (*i < len) {
        char c = json_string[*i];
        if (c >= '0' && c <= '9') {
            (*i)++;
        } else {
            break;
        }
    }

    size_t num_len = *i - start;
    char* num_str = malloc(num_len + 1);
    strncpy(num_str, json_string + start, num_len);
    num_str[num_len] = '\0';

    tok->value.number = atof(num_str);
    free(num_str);

    return tok;
}

ArrayList* json_tokenize(const char* json_string) {
    ArrayList* arr = arraylist_new(100);

    size_t len = strlen(json_string);
    for (size_t i = 0; i < len; i++) {
        char c = json_string[i];

        // special characters
        if (c == '{') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = LeftBrace;
            arraylist_append(arr, tok, true);
        } else if (c == '}') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = RightBrace;
            arraylist_append(arr, tok, true);
        } else if (c == '[') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = LeftBracket;
            arraylist_append(arr, tok, true);
        } else if (c == ']') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = RightBracket;
            arraylist_append(arr, tok, true);
        } else if (c == ':') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = Colon;
            arraylist_append(arr, tok, true);
        } else if (c == ',') {
            json_token* tok = malloc(sizeof(json_token));
            tok->type = Comma;
            arraylist_append(arr, tok, true);
        }

        // Strings
        else if (c == '"' || c == '\'') {
            json_token* tok = parse_json_string(json_string, &i, c);
            arraylist_append(arr, tok, true);
        }

        // Numbers
        if (c >= '0' && c <= '9') {
            json_token* tok = parse_json_number(json_string, &i);
            arraylist_append(arr, tok, true);
        }

        // Keywords (true, false, null)

        // Whitespace
        if (c == ' ') {
            // skip
        }
    }
    return arr;
}

json_object* process_object_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // {

    fprintf(stderr, "Error: opening { without closing }");
    assert(true);
    return NULL;
}

json_object* process_array_tokens(ArrayList* tokens, size_t* i) {
    (*i)++;  // [

    json_object* obj = malloc(sizeof(json_object));
    json_token* tok = arraylist_get(tokens, *i);

    if (tok->type == LeftBracket) {
        (*i)++; // consume ']'
        return obj;
    }

    fprintf(stderr, "Error: opening [ without closing ]");
    assert(true);
    return NULL;
}

json_object* process_tokens(ArrayList* tokens, size_t* i) {
    json_token* tok = arraylist_get(tokens, *i);

    switch (tok->type) {
        case LeftBrace:
            return process_object_tokens(tokens, i);
        case LeftBracket:
            return process_array_tokens(tokens, i);
        case String: {
            json_object* obj = malloc(sizeof(json_object));
            obj->type = JSON_STRING;
            obj->value.string = tok->value.string;
            (*i)++;
            return obj;
        }
        case Number: {
            json_object* obj = malloc(sizeof(json_object));
            obj->type = JSON_NUMBER;
            obj->value.number = tok->value.number;
            (*i)++;
            return obj;
        }
        default:
            return NULL;
    }
}

json_object* json_decode(const char* json_string) {
    size_t i = 0;
    ArrayList* tokens = json_tokenize(json_string);
    json_object* obj = process_tokens(tokens, &i);
    return obj;
}

char* json_encode(const json_object* obj) { return NULL; }

void json_free(json_object* obj) {}