#include <check.h>
#include <stdlib.h>
#include "../src/json.h"

START_TEST(test_encode_string) {
    JsonValueNode obj = {.type = JSON_STRING, .value.string = "hello"};

    char *encoded = json_stringify(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "\"hello\"");

    json_arena_free();
    free(encoded);
}
END_TEST

START_TEST(test_encode_number) {
    JsonValueNode obj = {.type = JSON_NUMBER, .value.number = 42.0};

    char *encoded = json_stringify(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "42");

    json_arena_free();
    free(encoded);
}
END_TEST

START_TEST(test_encode_null) {
    JsonValueNode obj = {.type = JSON_NULL};

    char *encoded = json_stringify(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "null");

    json_arena_free();
    free(encoded);
}
END_TEST

START_TEST(test_encode_array) {
    JsonValueNode *a = malloc(sizeof(JsonValueNode));
    a->type = JSON_NUMBER;
    a->value.number = 1;

    JsonValueNode *b = malloc(sizeof(JsonValueNode));
    b->type = JSON_NUMBER;
    b->value.number = 2;

    JsonValueNode *c = malloc(sizeof(JsonValueNode));
    c->type = JSON_NUMBER;
    c->value.number = 3;

    ArrayList *arr = arraylist_new(3);
    arraylist_append(arr, a, true);
    arraylist_append(arr, b, true);
    arraylist_append(arr, c, true);

    JsonValueNode obj;
    obj.type = JSON_ARRAY;
    obj.value.array = arr;

    char *encoded = json_stringify(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "[1,2,3]");

    json_arena_free();
    arraylist_free(arr);
    free(encoded);
}
END_TEST

START_TEST(test_encode_object) {
    JsonValueNode *a = malloc(sizeof(JsonValueNode));
    a->type = JSON_NUMBER;
    a->value.number = 1;

    JsonValueNode *b = malloc(sizeof(JsonValueNode));
    b->type = JSON_NUMBER;
    b->value.number = 2;

    Dict *dict = dict_new(2);
    dict_set(dict, "a", a);
    dict_set(dict, "b", b);

    JsonValueNode obj;
    obj.type = JSON_OBJECT;
    obj.value.object = dict;

    char *encoded = json_stringify(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "{\"a\":1,\"b\":2}");

    json_arena_free();
    dict_free_all(dict);
    free(a);
    free(b);
    free(encoded);
}
END_TEST


Suite *json_encode_suite(void) {
    Suite *s = suite_create("JSON Encode");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_encode_string);
    tcase_add_test(tc_core, test_encode_number);
    tcase_add_test(tc_core, test_encode_null);
    tcase_add_test(tc_core, test_encode_array);
    tcase_add_test(tc_core, test_encode_object);

    suite_add_tcase(s, tc_core);
    return s;
}
