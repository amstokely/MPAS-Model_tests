#include "unity.h"
#include <string.h>
#include "regex_matching.h"


void setUp(void) {}
void tearDown(void) {}

void test_exact_match_should_succeed(void) {
    int imatch;
    check_regex_match("foo", "foo", &imatch);
    TEST_ASSERT_EQUAL_INT(1, imatch);
}

void test_partial_match_should_fail_due_to_anchors(void) {
    int imatch;
    check_regex_match("foo", "foobar", &imatch);
    TEST_ASSERT_EQUAL_INT(0, imatch);
}

void test_non_matching_pattern_should_fail(void) {
    int imatch;
    check_regex_match("bar", "baz", &imatch);
    TEST_ASSERT_EQUAL_INT(0, imatch);
}

void test_invalid_regex_should_return_minus1(void) {
    int imatch;
    check_regex_match("[unclosed", "unclosed", &imatch);
    TEST_ASSERT_EQUAL_INT(-1, imatch);
}

void test_long_pattern_should_return_minus1(void) {
    int imatch;
    char long_pattern[1100];
    memset(long_pattern, 'a', 1023); // 1023 + "^" + "$" = 1025 > MAX_LEN
    long_pattern[1023] = '\0';
    check_regex_match(long_pattern, "aaaa", &imatch);
    TEST_ASSERT_EQUAL_INT(-1, imatch);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_exact_match_should_succeed);
    RUN_TEST(test_partial_match_should_fail_due_to_anchors);
    RUN_TEST(test_non_matching_pattern_should_fail);
    RUN_TEST(test_invalid_regex_should_return_minus1);
    RUN_TEST(test_long_pattern_should_return_minus1);
    return UNITY_END();
}