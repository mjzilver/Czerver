#include <check.h>
#include <stdlib.h>
#include "../src/json.h"

START_TEST(test_encode_string) {
    json_object obj = {.type = JSON_STRING, .value.string = "hello"};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "\"hello\"");

    free(encoded);
}
END_TEST

START_TEST(test_encode_number) {
    json_object obj = {.type = JSON_NUMBER, .value.number = 42.0};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "42");

    free(encoded);
}
END_TEST

START_TEST(test_encode_null) {
    json_object obj = {.type = JSON_NULL};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "null");

    free(encoded);
}
END_TEST

START_TEST(test_encode_array) {
    json_object a = {.type = JSON_NUMBER, .value.number = 1};
    json_object b = {.type = JSON_NUMBER, .value.number = 2};
    json_object c = {.type = JSON_NUMBER, .value.number = 3};

    json_object *items[] = {&a, &b, &c};
    json_object obj = {.type = JSON_ARRAY, .value.array.items = items, .value.array.length = 3};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "[1,2,3]");

    free(encoded);
}
END_TEST

START_TEST(test_encode_object) {
    json_object a = {.type = JSON_NUMBER, .value.number = 1};
    json_object b = {.type = JSON_NUMBER, .value.number = 2};

    char *keys[] = {"a", "b"};
    json_object *values[] = {&a, &b};

    json_object obj = {.type = JSON_OBJECT, .value.object.keys = keys, .value.object.values = values, .value.object.length = 2};

    char *encoded = json_encode(&obj);
    ck_assert_ptr_nonnull(encoded);
    ck_assert_str_eq(encoded, "{\"a\":1,\"b\":2}");

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
