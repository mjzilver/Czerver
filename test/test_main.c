#include <check.h>
#include <stdlib.h>

Suite* dict_suite(void);
Suite* arraylist_suite(void);
Suite* json_encode_suite(void);
Suite* json_decode_suite(void);
Suite* tokenizer_suite(void);

int main(void) {
    int number_failed;
    SRunner *sr = srunner_create(dict_suite());
    srunner_add_suite(sr, arraylist_suite());  
    srunner_add_suite(sr, json_encode_suite());
    srunner_add_suite(sr, json_decode_suite());
    srunner_add_suite(sr, tokenizer_suite());

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);
    
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
