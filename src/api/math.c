#include "math.h"

#include <stdbool.h>
#include <string.h>

#include "../json.h"
#include "../json_utils.h"
#include "api_utils.h"

typedef struct {
    double result;
} MathResult;

char* math_api_handler(const char* request_body) {
    Arena* arena = get_fresh_request_arena();

    JsonValueNode* req = json_parse_string(request_body);
    if (!req || req->type != JSON_OBJECT) {
        return make_error(arena, "Invalid JSON");
    }

    Dict* obj = req->value.object;

    double a = json_value_node_get_number(obj, "a");
    double b = json_value_node_get_number(obj, "b");
    const char* op = json_value_node_get_string(obj, "op");

    double result = 0;

    if (!op)
        return make_error(arena, "No operator found");
    else if (strcmp(op, "+") == 0)
        result = a + b;
    else if (strcmp(op, "-") == 0)
        result = a - b;
    else if (strcmp(op, "*") == 0)
        result = a * b;
    else if (strcmp(op, "/") == 0)
        result = (b != 0) ? a / b : 0;
    else
        return make_error(arena, "Invalid operator");

    MathResult m = {.result = result};

    FieldDescriptor math_fields[] = {
        {"result", FIELD_TYPE_NUMBER, offsetof(MathResult, result)},
    };

    JsonValueNode* j = struct_to_json_value_node(arena, &m, math_fields, 1);

    return json_stringify(j);
}
