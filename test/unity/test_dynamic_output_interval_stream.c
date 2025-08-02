#include "unity.h"
#include "dynamic_output_interval_stream.h"
#include <stdlib.h>
#include <string.h>

static char *test_xml = NULL;
static Stream streams[MAX_STREAMS];
static int num_streams = 0;

void setUp(void) {
    const char *xml_src =
        "<streams>"
        "  <stream name=\"dynamic1\" type=\"none\" filename_template=\"dyn1.nc\">"
        "    <output_interval>"
        "      <range start=\"0000_00:00:00\" end=\"0005_00:00:00\" interval=\"0_01:00:00\"/>"
        "      <range start=\"0005_00:00:00\" end=\"0010_00:00:00\" interval=\"0_03:00:00\"/>"
        "      <range start=\"0010_00:00:00\" interval=\"0_06:00:00\"/>"
        "    </output_interval>"
        "  </stream>"
        "  <stream name=\"static1\" type=\"none\" filename_template=\"stat1.nc\" output_interval=\"0_05:00:00\"/>"
        "</streams>";

    test_xml = strdup(xml_src);
    num_streams = parse_streams(streams, test_xml);
}

void tearDown(void) {
    free(test_xml);
    test_xml = NULL;
}

void test_stream_count(void) {
    TEST_ASSERT_EQUAL_INT(2, num_streams);
}

void test_static_stream_interval(void) {
    const Stream *s = &streams[1];
    const char *ival = get_output_interval(s, "0006_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);
    TEST_ASSERT_EQUAL_STRING("0_05:00:00", ival);
}

void test_dynamic_stream_interval_first_range(void) {
    const Stream *s = &streams[0];
    const char *ival = get_output_interval(s, "0001_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);
    TEST_ASSERT_EQUAL_STRING("0_01:00:00", ival);
}

void test_dynamic_stream_interval_second_range(void) {
    const Stream *s = &streams[0];
    const char *ival = get_output_interval(s, "0006_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);
    TEST_ASSERT_EQUAL_STRING("0_03:00:00", ival);
}

void test_dynamic_stream_interval_fallback_range(void) {
    const Stream *s = &streams[0];
    const char *ival = get_output_interval(s, "0012_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);
    TEST_ASSERT_EQUAL_STRING("0_06:00:00", ival);
}

void test_missing_stream_name(void) {
    const char *xml_src =
        "<streams>"
        "  <stream type=\"none\" filename_template=\"missingname.nc\" output_interval=\"0_02:00:00\"/>"
        "</streams>";

    int count = parse_streams(streams, xml_src);
    TEST_ASSERT_EQUAL_INT(1, count);
    // Expecting empty string due to missing "name"
    TEST_ASSERT_EQUAL_STRING("", streams[0].name);
}

void test_output_interval_too_long(void) {
    const char *xml_src =
        "<streams>"
        "  <stream name=\"toolong\" type=\"none\" filename_template=\"too_long.nc\""
        "          output_interval=\"0123456789012345678901234567890123456789\"/>"
        "</streams>";

    int count = parse_streams(streams, xml_src);
    TEST_ASSERT_EQUAL_INT(1, count);
    // Ensure null terminator is present at the end of static_output_interval
    TEST_ASSERT_EQUAL_CHAR('\0', streams[0].static_output_interval[TIME_LEN - 1]);
}

void test_dynamic_interval_missing_interval(void) {
    const char *xml_src =
        "<streams>"
        "  <stream name=\"partial\" type=\"none\" filename_template=\"partial.nc\">"
        "    <output_interval>"
        "      <range start=\"0000_00:00:00\" end=\"0010_00:00:00\"/>"
        "    </output_interval>"
        "  </stream>"
        "</streams>";

    int count = parse_streams(streams, xml_src);
    TEST_ASSERT_EQUAL_INT(1, count);
    const char *ival = get_output_interval(&streams[0], "0005_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);  // Should be empty string, not NULL
    TEST_ASSERT_EQUAL_STRING("", ival);
}

void test_out_of_order_dynamic_intervals(void) {
    const char *xml_src =
        "<streams>"
        "  <stream name=\"outoforder\" type=\"none\" filename_template=\"out.nc\">"
        "    <output_interval>"
        "      <range start=\"0010_00:00:00\" interval=\"0_06:00:00\"/>"
        "      <range start=\"0000_00:00:00\" end=\"0005_00:00:00\" interval=\"0_01:00:00\"/>"
        "    </output_interval>"
        "  </stream>"
        "</streams>";

    int count = parse_streams(streams, xml_src);
    TEST_ASSERT_EQUAL_INT(1, count);

    const Stream *s = &streams[0];
    const char *ival = get_output_interval(s, "0002_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);  // Ensure it still matches correct range
    TEST_ASSERT_EQUAL_STRING("0_01:00:00", ival);
}

void test_extra_attributes_ignored(void) {
    const char *xml_src =
        "<streams>"
        "  <stream name=\"extra\" type=\"none\" filename_template=\"extra.nc\" output_interval=\"0_05:00:00\" extra_attr=\"ignored\">"
        "    <extra_element>Should be ignored</extra_element>"
        "  </stream>"
        "</streams>";

    int count = parse_streams(streams, xml_src);
    TEST_ASSERT_EQUAL_INT(1, count);

    const char *ival = get_output_interval(&streams[0], "0001_00:00:00");
    TEST_ASSERT_NOT_NULL(ival);
    TEST_ASSERT_EQUAL_STRING("0_05:00:00", ival);
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_stream_count);
    RUN_TEST(test_static_stream_interval);
    RUN_TEST(test_dynamic_stream_interval_first_range);
    RUN_TEST(test_dynamic_stream_interval_second_range);
    RUN_TEST(test_dynamic_stream_interval_fallback_range);
    return UNITY_END();
}
