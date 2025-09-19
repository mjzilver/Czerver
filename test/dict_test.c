#include <check.h>
#include "../src/dict.h"
#include <stdlib.h>

void count_callback(const char *key, void *value, void *user_context) {
    (void)key;
    (void)value;
    int *count = (int *)user_context;
    (*count)++;
}

START_TEST(test_insert_and_get) {
    Dict *d = dict_new(INITIAL_DICT_CAPACITY);
    dict_set(d, "name", "Alice");
    dict_set(d, "lang", "C");

    ck_assert_str_eq(dict_get(d, "name"), "Alice");
    ck_assert_str_eq(dict_get(d, "lang"), "C");
    ck_assert_ptr_eq(dict_get(d, "missing"), NULL);

    dict_free_all(d);
}
END_TEST

START_TEST(test_overwrite_behavior) {
    Dict *d = dict_new(INITIAL_DICT_CAPACITY);
    dict_set(d, "name", "Alice");
    dict_set(d, "name", "Bob");

    ck_assert_str_eq(dict_get(d, "name"), "Bob");

    dict_free_all(d);
}
END_TEST

START_TEST(test_remove) {
    Dict *d = dict_new(INITIAL_DICT_CAPACITY);
    dict_set(d, "city", "Paris");
    ck_assert_str_eq(dict_get(d, "city"), "Paris");

    dict_remove(d, "city");
    ck_assert_ptr_eq(dict_get(d, "city"), NULL);

    dict_remove(d, "not_there");

    dict_free_all(d);
}
END_TEST

START_TEST(test_iterate) {
    Dict *d = dict_new(INITIAL_DICT_CAPACITY);
    dict_set(d, "a", "1");
    dict_set(d, "b", "2");
    dict_set(d, "c", "3");

    int count = 0;
    dict_iterate(d, count_callback, &count);
    ck_assert_int_eq(count, 3);

    dict_free_all(d);
}
END_TEST

Suite *dict_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("Dict");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_insert_and_get);
    tcase_add_test(tc_core, test_overwrite_behavior);
    tcase_add_test(tc_core, test_remove);
    tcase_add_test(tc_core, test_iterate);

    suite_add_tcase(s, tc_core);
    return s;
}