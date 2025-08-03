#include <string.h>

#include "unity.h"
#include "ezxml.h"
#include "xml_stream_parser.h"

void setUp(void) {
}

void tearDown(void) {
}

// Helper to construct and parse an XML stream tag
static ezxml_t make_stream(const char *xml) {
    return ezxml_parse_str(strdup(xml), strlen(xml));
}

static ezxml_t make_streams(const char *xml) {
    return ezxml_parse_str(strdup(xml), strlen(xml));
}


static ezxml_t load_xml(const char *xml) {
    char *buf = strdup(xml);  // ezxml_parse_str requires writable input
    ezxml_t root = ezxml_parse_str(buf, strlen(buf));
    return root;
}


// Dummy implementation for fmt_err if it's not linked from production
void fmt_err(const char *msg) {
    // Just print; not tested
    // printf("fmt_err: %s\n", msg);
}

void test_uniqueness_check_same_name_should_fail(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"output1\" type=\"output\" filename_template=\"file1.nc\"/>"
        "  <stream name=\"output1\" type=\"output\" filename_template=\"file2.nc\"/>"
        "</streams>";

    ezxml_t root = ezxml_parse_str(strdup(xml), strlen(xml));
    ezxml_t s1 = ezxml_child(root, "stream");
    ezxml_t s2 = ezxml_next(s1);
    TEST_ASSERT_EQUAL_INT(1, uniqueness_check(s1, s2)); // Should fail due to same name
    ezxml_free(root);
}

void test_uniqueness_check_same_filename_output_should_fail(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"output1\" type=\"output\" filename_template=\"shared.nc\"/>"
        "  <stream name=\"output2\" type=\"output\" filename_template=\"shared.nc\"/>"
        "</streams>";

    ezxml_t root = ezxml_parse_str(strdup(xml), strlen(xml));
    ezxml_t s1 = ezxml_child(root, "stream");
    ezxml_t s2 = ezxml_next(s1);
    TEST_ASSERT_EQUAL_INT(1, uniqueness_check(s1, s2)); // Same filename + output
    ezxml_free(root);
}

void test_uniqueness_check_different_output_streams_should_pass(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"out1\" type=\"output\" filename_template=\"a.nc\"/>"
        "  <stream name=\"out2\" type=\"output\" filename_template=\"b.nc\"/>"
        "</streams>";

    ezxml_t root = ezxml_parse_str(strdup(xml), strlen(xml));
    ezxml_t s1 = ezxml_child(root, "stream");
    ezxml_t s2 = ezxml_next(s1);
    TEST_ASSERT_EQUAL_INT(0, uniqueness_check(s1, s2)); // Should succeed
    ezxml_free(root);
}

void test_uniqueness_check_input_streams_same_filename_should_pass(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"in1\" type=\"input\" filename_template=\"shared.nc\"/>"
        "  <stream name=\"in2\" type=\"input\" filename_template=\"shared.nc\"/>"
        "</streams>";

    ezxml_t root = ezxml_parse_str(strdup(xml), strlen(xml));
    ezxml_t s1 = ezxml_child(root, "stream");
    ezxml_t s2 = ezxml_next(s1);
    TEST_ASSERT_EQUAL_INT(0, uniqueness_check(s1, s2)); // Input streams can share filenames
    ezxml_free(root);
}

void test_parse_xml_tag_name_basic_tag(void) {
    char tag_buf[] = "stream name=\"foo\"";
    char tag_name[32] = {0};

    parse_xml_tag_name(tag_buf, tag_name);
    TEST_ASSERT_EQUAL_STRING("stream", tag_name);
}

void test_parse_xml_tag_name_tag_with_no_space(void) {
    char tag_buf[] = "stream";
    char tag_name[32] = {0};

    parse_xml_tag_name(tag_buf, tag_name);
    TEST_ASSERT_EQUAL_STRING("stream", tag_name);
}

void test_parse_xml_tag_name_leading_space(void) {
    char tag_buf[] = " stream name=\"foo\"";  // starts with space
    char tag_name[32] = {0};

    parse_xml_tag_name(tag_buf + 1, tag_name);  // skip leading space manually
    TEST_ASSERT_EQUAL_STRING("stream", tag_name);
}

void test_parse_xml_tag_name_empty_string(void) {
    char tag_buf[] = "";
    char tag_name[32] = {0};

    parse_xml_tag_name(tag_buf, tag_name);
    TEST_ASSERT_EQUAL_STRING("", tag_name);
}

void test_parse_xml_tag_simple_tag(void) {
    char xml[] = "<stream name=\"foo\">";
    char tag[128];
    size_t tag_len;
    int line = 1, start_line;
    size_t offset;

    offset = parse_xml_tag(xml, strlen(xml), tag, &tag_len, &line, &start_line);

    TEST_ASSERT_EQUAL_STRING("stream name=\"foo\"", tag);
    TEST_ASSERT_EQUAL_UINT(strlen("stream name=\"foo\""), tag_len);
    TEST_ASSERT_EQUAL(1, start_line);
    TEST_ASSERT_EQUAL(strlen(xml), offset);
}

void test_parse_xml_tag_skips_comment(void) {
    char xml[] = "<!-- this is a comment -->\n<stream name=\"bar\">";
    char tag[128];
    size_t tag_len;
    int line = 1, start_line;
    size_t offset;

    offset = parse_xml_tag(xml, strlen(xml), tag, &tag_len, &line, &start_line);

    TEST_ASSERT_EQUAL_STRING("stream name=\"bar\"", tag);
    TEST_ASSERT_EQUAL(2, line);        // newline after comment
    TEST_ASSERT_EQUAL(2, start_line);  // tag starts on line 2
}

void test_parse_xml_tag_handles_newlines(void) {
    char xml[] = "\n\n  \n<stream type=\"output\">";
    char tag[128];
    size_t tag_len;
    int line = 1, start_line;
    size_t offset;

    offset = parse_xml_tag(xml, strlen(xml), tag, &tag_len, &line, &start_line);

    TEST_ASSERT_EQUAL_STRING("stream type=\"output\"", tag);
    TEST_ASSERT_EQUAL(4, start_line);  // opening tag starts after 3 newlines
}

void test_parse_xml_tag_missing_closing_bracket(void) {
    char xml[] = "<stream";  // never closes
    char tag[128];
    size_t tag_len = 999;
    int line = 1, start_line;
    size_t offset;

    offset = parse_xml_tag(xml, strlen(xml), tag, &tag_len, &line, &start_line);

    TEST_ASSERT_EQUAL_UINT(0, offset);
    TEST_ASSERT_EQUAL_UINT(0, tag_len);
}
void test_parse_xml_tag_only_comment(void) {
    char xml[] = "<!-- only comment -->";
    char tag[128];
    size_t tag_len = 999;
    int line = 1, start_line;
    size_t offset;

    offset = parse_xml_tag(xml, strlen(xml), tag, &tag_len, &line, &start_line);

    TEST_ASSERT_EQUAL_UINT(0, offset);
    TEST_ASSERT_EQUAL_UINT(0, tag_len);
}


void test_missing_name_attribute(void) {
    const char *xml = "<stream type=\"input\" filename_template=\"file.nc\" input_interval=\"0_01:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_missing_type_attribute(void) {
    const char *xml = "<stream name=\"foo\" filename_template=\"file.nc\" input_interval=\"0_01:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_missing_filename_template(void) {
    const char *xml = "<stream name=\"foo\" type=\"input\" input_interval=\"0_01:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_input_missing_input_interval(void) {
    const char *xml = "<stream name=\"foo\" type=\"input\" filename_template=\"file.nc\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_output_missing_output_interval(void) {
    const char *xml = "<stream name=\"foo\" type=\"output\" filename_template=\"file.nc\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_illegal_filename_variable(void) {
    const char *xml = "<stream name=\"foo\" type=\"input\" filename_template=\"file_$X.nc\" input_interval=\"0_01:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(1, attribute_check(stream));
    ezxml_free(stream);
}

void test_valid_input_stream(void) {
    const char *xml = "<stream name=\"foo\" type=\"input\" filename_template=\"file_$Y.nc\" input_interval=\"0_01:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(0, attribute_check(stream));
    ezxml_free(stream);
}

void test_valid_output_stream(void) {
    const char *xml = "<stream name=\"bar\" type=\"output\" filename_template=\"out_$h.nc\" output_interval=\"0_03:00:00\"/>";
    ezxml_t stream = make_stream(xml);
    TEST_ASSERT_EQUAL_INT(0, attribute_check(stream));
    ezxml_free(stream);
}

void test_valid_streams(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"s1\" type=\"output\" filename_template=\"s1_$Y.nc\" output_interval=\"0_01:00:00\"/>"
        "  <stream name=\"s2\" type=\"input\" filename_template=\"s2_$Y.nc\" input_interval=\"0_06:00:00\"/>"
        "</streams>";
    ezxml_t root = make_streams(xml);
    TEST_ASSERT_EQUAL_INT(0, check_streams(root));
    ezxml_free(root);
}

void test_duplicate_stream_names(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"dup\" type=\"output\" filename_template=\"dup1.nc\" output_interval=\"0_01:00:00\"/>"
        "  <stream name=\"dup\" type=\"output\" filename_template=\"dup2.nc\" output_interval=\"0_02:00:00\"/>"
        "</streams>";
    ezxml_t root = make_streams(xml);
    TEST_ASSERT_NOT_EQUAL(0, check_streams(root));
    ezxml_free(root);
}

void test_immutable_stream_with_variable(void) {
    const char *xml =
        "<streams>"
        "  <immutable_stream name=\"init\" type=\"input\" filename_template=\"init.nc\" input_interval=\"0_01:00:00\">"
        "    <var name=\"temp\"/>"
        "  </immutable_stream>"
        "</streams>";
    ezxml_t root = make_streams(xml);
    TEST_ASSERT_NOT_EQUAL(0, check_streams(root));
    ezxml_free(root);
}

void test_stream_missing_required_attribute(void) {
    const char *xml =
        "<streams>"
        "  <stream type=\"output\" filename_template=\"missing_name.nc\" output_interval=\"0_01:00:00\"/>"
        "</streams>";
    ezxml_t root = make_streams(xml);
    TEST_ASSERT_NOT_EQUAL(0, check_streams(root));
    ezxml_free(root);
}

void test_stream_with_unreadable_file_reference(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"sfile\" type=\"input\" filename_template=\"sfile.nc\" input_interval=\"0_01:00:00\">"
        "    <file name=\"nonexistent.yaml\"/>"
        "  </stream>"
        "</streams>";
    ezxml_t root = make_streams(xml);
    TEST_ASSERT_NOT_EQUAL(0, check_streams(root));
    ezxml_free(root);
}

void test_valid_xml_balanced_tags(void) {
    const char *xml = "<root><child name=\"foo\"/></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_unbalanced_angle_brackets(void) {
    const char *xml = "<root><child></root";  // missing closing >
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_unclosed_tag(void) {
    const char *xml = "<root><child></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_mismatched_closing_tag(void) {
    const char *xml = "<root><child></chid></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_unbalanced_quotes(void) {
    const char *xml = "<root><child name=\"foo></child></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_nested_comments_with_tags(void) {
    const char *xml = "<root><!-- <child> --><child/></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_unclosed_comment(void) {
    const char *xml = "<root><!-- comment<child></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_starting_with_gt_should_fail(void) {
    const char *xml = "><root></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_self_closing_tag(void) {
    const char *xml = "<root><child/></root>";
    int result = xml_syntax_check((char *)xml, strlen(xml));
    TEST_ASSERT_EQUAL_INT(0, result);
}

void test_valid_interval_reference(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"A\" input_interval=\"00:15:00\"/>"
        "  <stream name=\"B\" input_interval=\"stream:A:input_interval\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    ezxml_t streamB = ezxml_child(root, "stream")->next;

    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:A:input_interval", "input_interval", &interval2, "B", root);
    TEST_ASSERT_EQUAL_INT(0, rc);
    TEST_ASSERT_NOT_NULL(interval2);
    TEST_ASSERT_EQUAL_STRING("00:15:00", interval2);

    ezxml_free(root);
}

void test_self_reference_should_fail(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"A\" input_interval=\"stream:A:input_interval\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    ezxml_t streamA = ezxml_child(root, "stream");

    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:A:input_interval", "input_interval", &interval2, "A", root);
    TEST_ASSERT_NOT_EQUAL(0, rc);
    TEST_ASSERT_NULL(interval2);

    ezxml_free(root);
}

void test_invalid_attribute_reference(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"A\" output_interval=\"01:00:00\"/>"
        "  <stream name=\"B\" input_interval=\"stream:A:bad_attr\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    ezxml_t streamB = ezxml_child(root, "stream")->next;

    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:A:bad_attr", "input_interval", &interval2, "B", root);
    TEST_ASSERT_NOT_EQUAL(0, rc);
    TEST_ASSERT_NULL(interval2);

    ezxml_free(root);
}

void test_reference_to_undefined_stream(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"B\" input_interval=\"stream:Missing:input_interval\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:Missing:input_interval", "input_interval", &interval2, "B", root);
    TEST_ASSERT_NOT_EQUAL(0, rc);
    TEST_ASSERT_NULL(interval2);

    ezxml_free(root);
}

void test_reference_to_null_interval_attribute(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"A\" output_interval=\"01:00:00\"/>"
        "  <stream name=\"B\" input_interval=\"stream:A:input_interval\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:A:input_interval", "input_interval", &interval2, "B", root);
    TEST_ASSERT_NOT_EQUAL(0, rc);
    TEST_ASSERT_NULL(interval2);

    ezxml_free(root);
}

void test_unexpandable_interval_should_fail(void) {
    const char *xml =
        "<streams>"
        "  <stream name=\"A\" input_interval=\"stream:B:output_interval\"/>"
        "  <stream name=\"B\" output_interval=\"output_interval\"/>"
        "</streams>";

    ezxml_t root = load_xml(xml);
    const char *interval2 = NULL;
    int rc = extract_stream_interval("stream:B:output_interval", "input_interval", &interval2, "A", root);
    TEST_ASSERT_NOT_EQUAL(0, rc);
    TEST_ASSERT_NULL(interval2);

    ezxml_free(root);
}





int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_uniqueness_check_same_name_should_fail);
    RUN_TEST(test_uniqueness_check_same_filename_output_should_fail);
    RUN_TEST(test_uniqueness_check_different_output_streams_should_pass);
    RUN_TEST(test_uniqueness_check_input_streams_same_filename_should_pass);
    RUN_TEST(test_parse_xml_tag_name_basic_tag);
    RUN_TEST(test_parse_xml_tag_name_tag_with_no_space);
    RUN_TEST(test_parse_xml_tag_name_leading_space);
    RUN_TEST(test_parse_xml_tag_name_empty_string);
    RUN_TEST(test_parse_xml_tag_simple_tag);
    RUN_TEST(test_parse_xml_tag_skips_comment);
    RUN_TEST(test_parse_xml_tag_handles_newlines);
    RUN_TEST(test_parse_xml_tag_missing_closing_bracket);
    RUN_TEST(test_parse_xml_tag_only_comment);
    RUN_TEST(test_missing_name_attribute);
    RUN_TEST(test_missing_type_attribute);
    RUN_TEST(test_missing_filename_template);
    RUN_TEST(test_input_missing_input_interval);
    RUN_TEST(test_output_missing_output_interval);
    RUN_TEST(test_illegal_filename_variable);
    RUN_TEST(test_valid_input_stream);
    RUN_TEST(test_valid_output_stream);
    RUN_TEST(test_valid_streams);
    RUN_TEST(test_duplicate_stream_names);
    RUN_TEST(test_immutable_stream_with_variable);
    RUN_TEST(test_stream_missing_required_attribute);
    RUN_TEST(test_stream_with_unreadable_file_reference);
    RUN_TEST(test_valid_xml_balanced_tags);
    RUN_TEST(test_unbalanced_angle_brackets);
    RUN_TEST(test_unclosed_tag);
    RUN_TEST(test_mismatched_closing_tag);
    RUN_TEST(test_unbalanced_quotes);
    RUN_TEST(test_nested_comments_with_tags);
    RUN_TEST(test_unclosed_comment);
    RUN_TEST(test_starting_with_gt_should_fail);
    RUN_TEST(test_self_closing_tag);
    RUN_TEST(test_valid_interval_reference);
    // RUN_TEST(test_self_reference_should_fail);
    // RUN_TEST(test_invalid_attribute_reference);
    // RUN_TEST(test_reference_to_undefined_stream);
    // RUN_TEST(test_reference_to_null_interval_attribute);
    // RUN_TEST(test_unexpandable_interval_should_fail);
    return UNITY_END();
}