#include "unity.h"
#include "xml_stream_parser.h"
#include "ezxml.h"
#include <stdlib.h>
#include <string.h>

static ezxml_t root = NULL;
static char *test_xml = NULL;

void setUp(void) {
}

void tearDown(void) {
    ezxml_free(root);
    free(test_xml);
    test_xml = NULL;
    root = NULL;
}

// -- Test: variable interval --
void test_variable_interval(void) {
    const char *xml = "<streams>"
            "  <stream name=\"s1\" type=\"output\" filename_template=\"f.nc\">"
            "    <output_interval>"
            "      <range start=\"0000_00:00:00\" end=\"0001_00:00:00\" interval=\"0_01:00:00\"/>"
            "    </output_interval>" "  </stream>" "</streams>";

    test_xml = strdup(xml);
    root = ezxml_parse_str(test_xml, strlen(test_xml));
    ezxml_t stream = ezxml_child(root, "stream");

    TEST_ASSERT_EQUAL_INT(
        INTERVAL_VARIABLE, get_interval_type(stream, "output_interval")
    );
}

// -- Test: fixed interval --
void test_fixed_interval(void) {
    const char *xml = "<streams>"
            "  <stream name=\"s2\" type=\"output\" output_interval=\"00:15:00\" />"
            "</streams>";

    test_xml = strdup(xml);
    root = ezxml_parse_str(test_xml, strlen(test_xml));
    ezxml_t stream = ezxml_child(root, "stream");

    TEST_ASSERT_EQUAL_INT(
        INTERVAL_FIXED, get_interval_type(stream, "output_interval")
    );
}

// -- Test: both fixed and variable (conflict) --
void test_conflicting_interval(void) {
    const char *xml = "<streams>"
            "  <stream name=\"s3\" type=\"output\" output_interval=\"00:30:00\">"
            "    <output_interval>"
            "      <range start=\"0000_00:00:00\" end=\"0002_00:00:00\" interval=\"0_01:00:00\"/>"
            "    </output_interval>" "  </stream>" "</streams>";

    test_xml = strdup(xml);
    root = ezxml_parse_str(test_xml, strlen(test_xml));
    ezxml_t stream = ezxml_child(root, "stream");

    TEST_ASSERT_EQUAL_INT(
        INTERVAL_CONFLICT, get_interval_type(stream, "output_interval")
    );
}

// -- Test: missing interval --
void test_missing_interval(void) {
    const char *xml = "<streams>"
            "  <stream name=\"s4\" type=\"output\" />" "</streams>";

    test_xml = strdup(xml);
    root = ezxml_parse_str(test_xml, strlen(test_xml));
    ezxml_t stream = ezxml_child(root, "stream");

    TEST_ASSERT_EQUAL_INT(
        INTERVAL_NOT_FOUND, get_interval_type(stream, "output_interval")
    );
}

// -- Test: NULL arguments --
void test_null_args(void) {
    TEST_ASSERT_EQUAL_INT(
        INTERVAL_NOT_FOUND, get_interval_type(NULL, "output_interval")
    );
    TEST_ASSERT_EQUAL_INT(
        INTERVAL_NOT_FOUND, get_interval_type(NULL, NULL)
    );
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_variable_interval);
    RUN_TEST(test_fixed_interval);
    RUN_TEST(test_conflicting_interval);
    RUN_TEST(test_missing_interval);
    RUN_TEST(test_null_args);
    return UNITY_END();
}
