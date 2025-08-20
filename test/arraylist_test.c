#include <check.h>
#include "../src/arr_list.h"
#include <string.h>
#include <stdlib.h>

START_TEST(test_append_and_get) {
    ArrayList *list = arraylist_new(2);

    int *a = malloc(sizeof(int));
    *a = 10;
    int *b = malloc(sizeof(int));
    *b = 20;

    arraylist_append(list, a, true);
    arraylist_append(list, b, false);

    ck_assert_int_eq(*(int *)arraylist_get(list, 0), 10);
    ck_assert_int_eq(*(int *)arraylist_get(list, 1), 20);
    ck_assert_ptr_eq(arraylist_get(list, 2), NULL);

    arraylist_free(list);
    free(b); 
}
END_TEST

int int_cmp(const void *a, const void *b) {
    return *(int *)a - *(int *)b;
}

START_TEST(test_remove) {
    ArrayList *list = arraylist_new(2);

    int *x = malloc(sizeof(int));
    *x = 5;
    int *y = malloc(sizeof(int));
    *y = 10;

    arraylist_append(list, x, true);
    arraylist_append(list, y, true);

    arraylist_remove(list, x, int_cmp);

    ck_assert_int_eq(list->len, 1);
    ck_assert_int_eq(*(int *)arraylist_get(list, 0), 10);

    arraylist_free(list);
}
END_TEST

START_TEST(test_grow) {
    ArrayList *list = arraylist_new(1);

    int vals[4] = {1,2,3,4};
    for (int i = 0; i < 4; i++) {
        int *v = malloc(sizeof(int));
        *v = vals[i];
        arraylist_append(list, v, true);
    }

    for (int i = 0; i < 4; i++) {
        ck_assert_int_eq(*(int *)arraylist_get(list, i), vals[i]);
    }

    arraylist_free(list);
}
END_TEST

Suite *arraylist_suite(void) {
    Suite *s;
    TCase *tc_core;

    s = suite_create("ArrayList");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_append_and_get);
    tcase_add_test(tc_core, test_remove);
    tcase_add_test(tc_core, test_grow);

    suite_add_tcase(s, tc_core);
    return s;
}
