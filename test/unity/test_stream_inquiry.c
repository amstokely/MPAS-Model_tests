#include "unity.h"
#include "ezxml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include "stream_inquiry.h"


// Helper: create a temporary XML file
static char temp_filename[] = "test_streams_file.xml";

static const char *test_xml_content = "<streams>\n"
        "  <stream name=\"output\" type=\"mutable\" filename_template=\"output.nc\" />\n"
        "  <immutable_stream name=\"restart\" input_interval=\"initial_only\" />\n"
        "</streams>\n";
MPI_Comm comm;

void setUp(void) {
    FILE *fp = fopen(temp_filename, "w");
    TEST_ASSERT_NOT_NULL(fp);
    fwrite(
        test_xml_content, sizeof(char), strlen(test_xml_content), fp
    );
    fclose(fp);
    comm = MPI_COMM_WORLD; // Use the global MPI communicator
}

void tearDown(void) {
    unlink(temp_filename);
}

void test_parse_and_query_valid_stream(void) {
    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;
    int found = query_streams_file(
        root, "output", "filename_template", &attval
    );
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_NOT_NULL(attval);
    TEST_ASSERT_EQUAL_STRING("output.nc", attval);

    free_streams_file(root);
}

void test_parse_and_query_immutable_stream(void) {
    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;
    int found = query_streams_file(
        root, "restart", "input_interval", &attval
    );
    TEST_ASSERT_TRUE(found);
    TEST_ASSERT_NOT_NULL(attval);
    TEST_ASSERT_EQUAL_STRING("initial_only", attval);

    free_streams_file(root);
}

void test_query_nonexistent_stream(void) {
    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;
    int found = query_streams_file(
        root, "nonexistent", "filename_template", &attval
    );
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_NULL(attval);

    free_streams_file(root);
}

void test_query_existing_stream_missing_attr(void) {
    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;
    int found = query_streams_file(
        root, "output", "input_interval", &attval
    );
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_NULL(attval);

    free_streams_file(root);
}

void test_stream_missing_name_attribute(void) {
    const char *broken_xml = "<streams>\n"
            "  <stream filename_template=\"out.nc\" />\n"
            "</streams>\n";

    FILE *fp = fopen(temp_filename, "w");
    TEST_ASSERT_NOT_NULL(fp);
    fwrite(broken_xml, sizeof(char), strlen(broken_xml), fp);
    fclose(fp);

    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;

    int found = query_streams_file(
        root, "output", "filename_template", &attval
    );
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_NULL(attval);
    free_streams_file(root);
    unlink(temp_filename);
}

void test_stream_with_whitespace_in_name(void) {
    const char *xml_with_whitespace = "<streams>\n"
            "  <stream name=\" output \" filename_template=\"out.nc\" />\n"
            "</streams>\n";

    FILE *fp = fopen(temp_filename, "w");
    TEST_ASSERT_NOT_NULL(fp);
    fwrite(
        xml_with_whitespace, sizeof(char), strlen(xml_with_whitespace),
        fp
    );
    fclose(fp);

    ezxml_t root = parse_streams_file(comm, temp_filename);
    TEST_ASSERT_NOT_NULL(root);

    const char *attval = NULL;

    // Stream name has leading/trailing whitespace in XML, but we search without it
    int found = query_streams_file(
        root, "output", "filename_template", &attval
    );

    // This will fail unless the implementation trims whitespace
    TEST_ASSERT_FALSE(found);
    TEST_ASSERT_NULL(attval);

    free_streams_file(root);
}


int main(void) {
    int ierr = MPI_Init(NULL, NULL);
    UNITY_BEGIN();
    RUN_TEST(test_parse_and_query_valid_stream);
    RUN_TEST(test_parse_and_query_immutable_stream);
    RUN_TEST(test_query_nonexistent_stream);
    RUN_TEST(test_query_existing_stream_missing_attr);
    RUN_TEST(test_stream_with_whitespace_in_name);
    const int result = UNITY_END();
    ierr = MPI_Finalize();
    return result;
}
