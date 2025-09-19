#include <check.h>
#include "../src/json.h"

START_TEST(test_single_punctuation_tokens) {
    const char* input = "{[]}:,";
    ArrayList* tokens = json_tokenize(input);

    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 0))->type, LEFT_BRACE);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 1))->type, LEFT_BRACKET);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 2))->type, RIGHT_BRACKET);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 3))->type, RIGHT_BRACE);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 4))->type, COLON);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 5))->type, COMMA);

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_single_string_tokens) {
    const char* input = "\"hello\" 'world'";
    ArrayList* tokens = json_tokenize(input);

    json_token* t1 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t1->type, STRING);
    ck_assert_str_eq(t1->value.string, "hello");

    json_token* t2 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t2->type, STRING);
    ck_assert_str_eq(t2->value.string, "world");

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_integer_number_tokens) {
    const char* input = "0 123 4567 42";
    ArrayList* tokens = json_tokenize(input);

    json_token* t0 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, NUMBER);
    ck_assert_int_eq((int)t0->value.number, 0);

    json_token* t1 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, NUMBER);
    ck_assert_int_eq((int)t1->value.number, 123);

    json_token* t2 = (json_token*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, NUMBER);
    ck_assert_int_eq((int)t2->value.number, 4567);

    json_token* t3 = (json_token*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, NUMBER);
    ck_assert_int_eq((int)t3->value.number, 42);

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_mixed_tokens) {
    const char* input = "{ \"key\" : 'value' , [ ] }";
    ArrayList* tokens = json_tokenize(input);

    json_token* t0 = (json_token*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, LEFT_BRACE);

    json_token* t1 = (json_token*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, STRING);
    ck_assert_str_eq(t1->value.string, "key");

    json_token* t2 = (json_token*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, COLON);

    json_token* t3 = (json_token*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, STRING);
    ck_assert_str_eq(t3->value.string, "value");

    json_token* t4 = (json_token*)arraylist_get(tokens, 4);
    ck_assert_int_eq(t4->type, COMMA);

    json_token* t5 = (json_token*)arraylist_get(tokens, 5);
    ck_assert_int_eq(t5->type, LEFT_BRACKET);

    json_token* t6 = (json_token*)arraylist_get(tokens, 6);
    ck_assert_int_eq(t6->type, RIGHT_BRACKET);

    json_token* t7 = (json_token*)arraylist_get(tokens, 7);
    ck_assert_int_eq(t7->type, RIGHT_BRACE);

    arraylist_free(tokens);
}
END_TEST

START_TEST(test_array_with_numbers_and_strings) {
    const char* input = "[1, 2, 3, 'video games']";
    ArrayList* tokens = json_tokenize(input);

    ck_assert_int_eq(tokens->len, 9); 

    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 0))->type, LEFT_BRACKET);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 1))->type, NUMBER);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 2))->type, COMMA);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 3))->type, NUMBER);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 4))->type, COMMA); 
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 5))->type, NUMBER);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 6))->type, COMMA);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 7))->type, STRING);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 8))->type, RIGHT_BRACKET);

    arraylist_free(tokens);
}

START_TEST(test_object_with_numbers_and_commas) {
    const char* input = "{\"a\":1,\"b\":2}";
    ArrayList* tokens = json_tokenize(input);

    ck_assert_int_eq(tokens->len, 9); 

    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 0))->type, LEFT_BRACE);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 1))->type, STRING);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 2))->type, COLON);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 3))->type, NUMBER);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 4))->type, COMMA); 
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 5))->type, STRING);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 6))->type, COLON);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 7))->type, NUMBER);
    ck_assert_int_eq(((json_token*)arraylist_get(tokens, 8))->type, RIGHT_BRACE);

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
    tcase_add_test(tc_core, test_array_with_numbers_and_strings);
    tcase_add_test(tc_core, test_object_with_numbers_and_commas);

    suite_add_tcase(s, tc_core);
    return s;
}
