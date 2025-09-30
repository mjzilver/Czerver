#include <check.h>
#include "../src/json.h"

START_TEST(test_single_punctuation_tokens) {
    const char* input = "{[]}:,";
    ArrayList* tokens = json_lex_string(input);

    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 0))->type, JSON_TOKEN_LEFT_BRACE);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 1))->type, JSON_TOKEN_LEFT_BRACKET);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 2))->type, JSON_TOKEN_RIGHT_BRACKET);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 3))->type, JSON_TOKEN_RIGHT_BRACE);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 4))->type, JSON_TOKEN_COLON);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 5))->type, JSON_TOKEN_COMMA);

    json_arena_free();
}
END_TEST

START_TEST(test_single_string_tokens) {
    const char* input = "\"hello\" 'world'";
    ArrayList* tokens = json_lex_string(input);

    JsonToken* t1 = (JsonToken*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t1->type, JSON_TOKEN_STRING);
    ck_assert_str_eq(t1->value.string, "hello");

    JsonToken* t2 = (JsonToken*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t2->type, JSON_TOKEN_STRING);
    ck_assert_str_eq(t2->value.string, "world");

    json_arena_free();
}
END_TEST

START_TEST(test_integer_number_tokens) {
    const char* input = "0 123 4567 42";
    ArrayList* tokens = json_lex_string(input);

    JsonToken* t0 = (JsonToken*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq((int)t0->value.number, 0);

    JsonToken* t1 = (JsonToken*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq((int)t1->value.number, 123);

    JsonToken* t2 = (JsonToken*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq((int)t2->value.number, 4567);

    JsonToken* t3 = (JsonToken*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq((int)t3->value.number, 42);

    json_arena_free();
}
END_TEST

START_TEST(test_mixed_tokens) {
    const char* input = "{ \"key\" : 'value' , [ ] }";
    ArrayList* tokens = json_lex_string(input);

    JsonToken* t0 = (JsonToken*)arraylist_get(tokens, 0);
    ck_assert_int_eq(t0->type, JSON_TOKEN_LEFT_BRACE);

    JsonToken* t1 = (JsonToken*)arraylist_get(tokens, 1);
    ck_assert_int_eq(t1->type, JSON_TOKEN_STRING);
    ck_assert_str_eq(t1->value.string, "key");

    JsonToken* t2 = (JsonToken*)arraylist_get(tokens, 2);
    ck_assert_int_eq(t2->type, JSON_TOKEN_COLON);

    JsonToken* t3 = (JsonToken*)arraylist_get(tokens, 3);
    ck_assert_int_eq(t3->type, JSON_TOKEN_STRING);
    ck_assert_str_eq(t3->value.string, "value");

    JsonToken* t4 = (JsonToken*)arraylist_get(tokens, 4);
    ck_assert_int_eq(t4->type, JSON_TOKEN_COMMA);

    JsonToken* t5 = (JsonToken*)arraylist_get(tokens, 5);
    ck_assert_int_eq(t5->type, JSON_TOKEN_LEFT_BRACKET);

    JsonToken* t6 = (JsonToken*)arraylist_get(tokens, 6);
    ck_assert_int_eq(t6->type, JSON_TOKEN_RIGHT_BRACKET);

    JsonToken* t7 = (JsonToken*)arraylist_get(tokens, 7);
    ck_assert_int_eq(t7->type, JSON_TOKEN_RIGHT_BRACE);

    json_arena_free();
}
END_TEST

START_TEST(test_array_with_numbers_and_strings) {
    const char* input = "[1, 2, 3, 'video games']";
    ArrayList* tokens = json_lex_string(input);

    ck_assert_int_eq(tokens->len, 9); 

    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 0))->type, JSON_TOKEN_LEFT_BRACKET);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 1))->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 2))->type, JSON_TOKEN_COMMA);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 3))->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 4))->type, JSON_TOKEN_COMMA); 
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 5))->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 6))->type, JSON_TOKEN_COMMA);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 7))->type, JSON_TOKEN_STRING);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 8))->type, JSON_TOKEN_RIGHT_BRACKET);

    json_arena_free();
}

START_TEST(test_object_with_numbers_and_commas) {
    const char* input = "{\"a\":1,\"b\":2}";
    ArrayList* tokens = json_lex_string(input);

    ck_assert_int_eq(tokens->len, 9); 

    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 0))->type, JSON_TOKEN_LEFT_BRACE);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 1))->type, JSON_TOKEN_STRING);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 2))->type, JSON_TOKEN_COLON);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 3))->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 4))->type, JSON_TOKEN_COMMA);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 5))->type, JSON_TOKEN_STRING);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 6))->type, JSON_TOKEN_COLON);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 7))->type, JSON_TOKEN_NUMBER);
    ck_assert_int_eq(((JsonToken*)arraylist_get(tokens, 8))->type, JSON_TOKEN_RIGHT_BRACE);

    json_arena_free();
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
