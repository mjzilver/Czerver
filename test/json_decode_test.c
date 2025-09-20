#include <check.h>
#include "../src/json.h"

START_TEST(test_decode_string) {
    const char *input = "\"hello\"";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_STRING);
    ck_assert_str_eq(obj->value.string, "hello");

    json_free();
}
END_TEST

START_TEST(test_decode_number) {
    const char *input = "42";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_NUMBER);
    ck_assert_double_eq_tol(obj->value.number, 42.0, 1e-9);

    json_free();
}
END_TEST

START_TEST(test_decode_null) {
    const char *input = "null";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_NULL);

    json_free();
}
END_TEST

START_TEST(test_decode_array) {
    const char *input = "[1,2,3]";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_ARRAY);

    ArrayList *arr = obj->value.array;
    ck_assert_int_eq(arr->len, 3);

    for (size_t i = 0; i < arr->len; i++) {
        ArrayItem *item = arraylist_get(arr, i);
        ck_assert_ptr_nonnull(item);
        ck_assert_ptr_nonnull(item->value);

        json_object *elem = (json_object *)arraylist_get(arr, i);
        ck_assert_int_eq(elem->type, JSON_NUMBER);
        ck_assert_double_eq_tol(elem->value.number, (double)(i + 1), 1e-9);
    }

    json_free();
}
END_TEST

START_TEST(test_decode_object) {
    const char *input = "{\"a\":1,\"b\":2}";
    json_object *obj = json_decode(input);

    ck_assert_ptr_nonnull(obj);
    ck_assert_int_eq(obj->type, JSON_OBJECT);

    json_object *val_a = (json_object *)dict_get(obj->value.object, "a");
    ck_assert_ptr_nonnull(val_a);
    ck_assert_int_eq(val_a->type, JSON_NUMBER);
    ck_assert_double_eq_tol(val_a->value.number, 1.0, 1e-9);

    json_object *val_b = (json_object *)dict_get(obj->value.object, "b");
    ck_assert_ptr_nonnull(val_b);
    ck_assert_int_eq(val_b->type, JSON_NUMBER);
    ck_assert_double_eq_tol(val_b->value.number, 2.0, 1e-9);

    json_free();
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