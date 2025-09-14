#include <check.h>
#include "../src/json.h"

START_TEST(test_decode_string) {
    const char *input = "\"hello\"";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_STRING);
    ck_assert_str_eq(obj->value.string, "hello");

    json_free(obj);
}
END_TEST

START_TEST(test_decode_number) {
    const char *input = "42";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_NUMBER);
    ck_assert_double_eq_tol(obj->value.number, 42.0, 1e-9);

    json_free(obj);
}
END_TEST

START_TEST(test_decode_null) {
    const char *input = "null";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_NULL);

    json_free(obj);
}
END_TEST

START_TEST(test_decode_array) {
    const char *input = "[1,2,3]";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_ARRAY);
    ck_assert_int_eq(obj->value.array.length, 3);

    ck_assert_double_eq_tol(obj->value.array.items[0]->value.number, 1.0, 1e-9);
    ck_assert_double_eq_tol(obj->value.array.items[1]->value.number, 2.0, 1e-9);
    ck_assert_double_eq_tol(obj->value.array.items[2]->value.number, 3.0, 1e-9);

    json_free(obj);
}
END_TEST

START_TEST(test_decode_object) {
    const char *input = "{\"a\":1,\"b\":2}";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_OBJECT);
    ck_assert_int_eq(obj->value.object.length, 2);

    ck_assert_str_eq(obj->value.object.keys[0], "a");
    ck_assert_double_eq_tol(obj->value.object.values[0]->value.number, 1.0, 1e-9);

    ck_assert_str_eq(obj->value.object.keys[1], "b");
    ck_assert_double_eq_tol(obj->value.object.values[1]->value.number, 2.0, 1e-9);

    json_free(obj);
}
END_TEST

Suite *json_decode_suite(void) {
    Suite *s = suite_create("JSON Decode");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_decode_string);
    tcase_add_test(tc_core, test_decode_number);
    tcase_add_test(tc_core, test_decode_null);
    tcase_add_test(tc_core, test_decode_array);
    tcase_add_test(tc_core, test_decode_object);

    suite_add_tcase(s, tc_core);
    return s;
}