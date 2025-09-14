#include <check.h>
#include "../src/json.h"

START_TEST(test_single_punctuation_tokens) {
    const char* input = "{[]}:,";
    ArrayList* tokens = json_tokenize(input);

    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 0))->type, LeftBrace);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 1))->type, LeftBracket);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 2))->type, RightBracket);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 3))->type, RightBrace);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 4))->type, Colon);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 5))->type, Comma);

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_single_string_tokens) {
    const char* input = "\"hello\" 'world'";
    ArrayList* tokens = json_tokenize(input);

    json_token* t1 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t1->type, String);
    ck_assert_str_eq(t1->value.string, "hello");

    json_token* t2 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t2->type, String);
    ck_assert_str_eq(t2->value.string, "world");

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_integer_number_tokens) {
    const char* input = "0 123 4567 42";
    ArrayList* tokens = json_tokenize(input);

    json_token* t0 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, Number);
    ck_assert_int_eq((int)t0->value.number, 0);

    json_token* t1 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, Number);
    ck_assert_int_eq((int)t1->value.number, 123);

    json_token* t2 = (json_token*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, Number);
    ck_assert_int_eq((int)t2->value.number, 4567);

    json_token* t3 = (json_token*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, Number);
    ck_assert_int_eq((int)t3->value.number, 42);

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_mixed_tokens) {
    const char* input = "{ \"key\" : 'value' , [ ] }";
    ArrayList* tokens = json_tokenize(input);

    json_token* t0 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, LeftBrace);

    json_token* t1 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, String);
    ck_assert_str_eq(t1->value.string, "key");

    json_token* t2 = (json_token*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, Colon);

    json_token* t3 = (json_token*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, String);
    ck_assert_str_eq(t3->value.string, "value");

    json_token* t4 = (json_token*)arraylist_get(tokens, 4);
    ck_assert_int_eq(t4->type, Comma);

    json_token* t5 = (json_token*)arraylist_get(tokens, 5);
    ck_assert_int_eq(t5->type, LeftBracket);

    json_token* t6 = (json_token*)arraylist_get(tokens, 6);
    ck_assert_int_eq(t6->type, RightBracket);

    json_token* t7 = (json_token*)arraylist_get(tokens, 7);
    ck_assert_int_eq(t7->type, RightBrace);

    arraylist_free(tokens);
}
END_TEST

Suite* tokenizer_suite(void) {
    Suite* s;
    TCase* tc_core;

    s = suite_create("JSON Tokenizer");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_single_punctuation_tokens);
    tcase_add_test(tc_core, test_single_string_tokens);
    tcase_add_test(tc_core, test_integer_number_tokens);
    tcase_add_test(tc_core, test_mixed_tokens);

    suite_add_tcase(s, tc_core);
    return s;
}
