#include <check.h>
#include <stdlib.h>
#include "../src/json.h"

START_TEST(test_encode_string) {
    json_object obj = {.type = JSON_STRING, .value.string = "hello"};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "\"hello\"");

    json_free();
}
END_TEST

START_TEST(test_encode_number) {
    json_object obj = {.type = JSON_NUMBER, .value.number = 42.0};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "42");

    json_free();
}
END_TEST

START_TEST(test_encode_null) {
    json_object obj = {.type = JSON_NULL};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "null");

    json_free();
}
END_TEST

START_TEST(test_encode_array) {
    json_object *a = malloc(sizeof(json_object));
    a->type = JSON_NUMBER;
    a->value.number = 1;

    json_object *b = malloc(sizeof(json_object));
    b->type = JSON_NUMBER;
    b->value.number = 2;

    json_object *c = malloc(sizeof(json_object));
    c->type = JSON_NUMBER;
    c->value.number = 3;

    ArrayList *arr = arraylist_new(3);
    arraylist_append(arr, a, true);
    arraylist_append(arr, b, true);
    arraylist_append(arr, c, true);

    json_object obj;
    obj.type = JSON_ARRAY;
    obj.value.array = arr;

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "[1,2,3]");

    json_free();
    arraylist_free(arr);
}
END_TEST

START_TEST(test_encode_object) {
    json_object *a = malloc(sizeof(json_object));
    a->type = JSON_NUMBER;
    a->value.number = 1;

    json_object *b = malloc(sizeof(json_object));
    b->type = JSON_NUMBER;
    b->value.number = 2;

    Dict *dict = dict_new(2);
    dict_set(dict, "a", a);
    dict_set(dict, "b", b);

    json_object obj;
    obj.type = JSON_OBJECT;
    obj.value.object = dict;

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "{\"a\":1,\"b\":2}");

    json_free();
    dict_free_all(dict);
    free(a);
    free(b);
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
