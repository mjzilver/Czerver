#include "math.h"

#include <stdbool.h>
#include <string.h>

#include "../json.h"
#include "../json_utils.h"
#include "api_utils.h"

char* math_api_handler(const char* request_body) {
    json_object* req = json_decode(request_body);
    if (!req || req->type != JSON_OBJECT) {
        return strdup("{\"error\":\"invalid JSON\"}");
    }

    Arena* arena = get_fresh_request_arena();

    Dict* obj = req->value.object;

    double a = get_number(obj, "a");
    double b = get_number(obj, "b");
    const char* op = get_string(obj, "op");

    double result = 0;
    bool valid_op = true;

    if (!op)
        valid_op = false;
    else if (strcmp(op, "+") == 0)
        result = a + b;
    else if (strcmp(op, "-") == 0)
        result = a - b;
    else if (strcmp(op, "*") == 0)
        result = a * b;
    else if (strcmp(op, "/") == 0)
        result = (b != 0) ? a / b : 0;
    else
        valid_op = false;

    json_object* resp = arena_alloc(arena, sizeof(json_object));
    resp->type = JSON_OBJECT;
    resp->value.object = dict_arena_new(arena, 10);

    if (valid_op) {
        json_object* res_val = arena_alloc(arena, sizeof(json_object));
        res_val->type = JSON_NUMBER;
        res_val->value.number = result;
        dict_set(resp->value.object, "result", res_val);
    } else {
        json_object* err_val = arena_alloc(arena, sizeof(json_object));
        err_val->type = JSON_STRING;
        err_val->value.string = arena_strdup(arena, "invalid operation");
        dict_set(resp->value.object, "error", err_val);
    }

    char* encoded = json_encode(resp);

    json_free();

    return encoded;
}
